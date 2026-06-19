#include "AudioCommon.h"
#include <cassert>
#include <fstream>
#include <StringUtility.h>
#include <filesystem>
#include <wrl.h>

//MediaFoundation
#include <mfapi.h>
#include <mfobjects.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

using namespace Microsoft::WRL;

namespace Norm {

	//サウンドデータコンテナの開始位置
	const uint32_t kStartSoundDataIndex = 1;

	std::unique_ptr<AudioCommon> AudioCommon::instance_ = nullptr;

	AudioCommon* AudioCommon::GetInstance() {
		if (!instance_) {
			instance_ = std::unique_ptr<AudioCommon>(new AudioCommon());
		}
		return instance_.get();
	}

	void AudioCommon::Initialize() {
		HRESULT hr;
		//XAudio2エンジンのインスタンス作成
		hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
		assert(SUCCEEDED(hr));
		//マスターボイスを作成
		hr = xAudio2_->CreateMasteringVoice(&masterVoice);
		assert(SUCCEEDED(hr));

		//Windows Media Foundationの初期化（ローカルファイル版）
		hr = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
		assert(SUCCEEDED(hr));

	}

	void AudioCommon::Finalize() {
		HRESULT hr;
		//コンテナの全開放
		ShutdownContainer();
		//インスタンスを削除
		instance_.reset();
		//Windows Media Foundationの終了
		hr = MFShutdown();
		assert(SUCCEEDED(hr));
	}

	uint32_t AudioCommon::SoundLoadFile(const std::string& filename) {
		// 既存データ検索
		for (uint32_t i = kStartSoundDataIndex; i < soundDatas_.size(); ++i) {
			if (soundDatas_[i].name == filename) {
				return i;
			}
		}

		//相対パス
		std::wstring filePathW = StringUtility::ConvertString(filename);

		HRESULT hr;

		// SourceReader作成
		ComPtr<IMFSourceReader> pReader;
		hr = MFCreateSourceReaderFromURL(filePathW.c_str(), nullptr, &pReader);
		assert(SUCCEEDED(hr));

		// PCM指定
		ComPtr<IMFMediaType> pPCMType;
		MFCreateMediaType(&pPCMType);
		pPCMType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
		pPCMType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);

		hr = pReader->SetCurrentMediaType(
			MF_SOURCE_READER_FIRST_AUDIO_STREAM,
			nullptr,
			pPCMType.Get()
		);
		assert(SUCCEEDED(hr));

		// 出力フォーマット取得
		ComPtr<IMFMediaType> pOutType;
		pReader->GetCurrentMediaType(
			MF_SOURCE_READER_FIRST_AUDIO_STREAM,
			&pOutType
		);

		// WaveFormat取得
		WAVEFORMATEX* waveFormat = nullptr;
		MFCreateWaveFormatExFromMFMediaType(
			pOutType.Get(),
			&waveFormat,
			nullptr
		);

		SoundData soundData{};
		soundData.wfex = *waveFormat;
		soundData.name = filename;

		CoTaskMemFree(waveFormat);

		//音声サイズを事前取得
		PROPVARIANT var;
		PropVariantInit(&var);
		
		hr = pReader->GetPresentationAttribute(
			MF_SOURCE_READER_MEDIASOURCE,
			MF_PD_DURATION,
			&var
		);

		UINT64 duration = var.hVal.QuadPart;
		PropVariantClear(&var);

		UINT32 bytesPerSec = 0;
		pOutType->GetUINT32(
			MF_MT_AUDIO_AVG_BYTES_PER_SECOND,
			&bytesPerSec
		);

		UINT64 estimatedSize =
			(duration * bytesPerSec) / 10000000;

		soundData.buffer.reserve((size_t)estimatedSize);

		//PCMデータ読み込み
		while (true) {

			ComPtr<IMFSample> pSample;
			DWORD streamIndex = 0;
			DWORD flags = 0;
			LONGLONG timeStamp = 0;

			hr = pReader->ReadSample(
				MF_SOURCE_READER_FIRST_AUDIO_STREAM,
				0,
				&streamIndex,
				&flags,
				&timeStamp,
				&pSample
			);

			assert(SUCCEEDED(hr));

			if (flags & MF_SOURCE_READERF_ENDOFSTREAM) {
				break;
			}

			if (!pSample) {
				continue;
			}

			ComPtr<IMFMediaBuffer> pBuffer;
			pSample->ConvertToContiguousBuffer(&pBuffer);

			BYTE* pData = nullptr;
			DWORD maxLength = 0;
			DWORD currentLength = 0;

			pBuffer->Lock(&pData, &maxLength, &currentLength);

			size_t oldSize = soundData.buffer.size();
			soundData.buffer.resize(oldSize + currentLength);

			memcpy(
				soundData.buffer.data() + oldSize,
				pData,
				currentLength
			);

			pBuffer->Unlock();
		}

		// 登録
		for (uint32_t i = kStartSoundDataIndex; i < soundDatas_.size(); ++i) {

			if (soundDatas_[i].buffer.empty()) {
				soundDatas_[i] = std::move(soundData);
				return i;
			}
		}

		assert(false && "No available space in soundDatas_");
		return -1;
	}

	uint32_t AudioCommon::SoundPlayWave(uint32_t soundDataHandle, bool loop, float volume) {
		assert(soundDataHandle < soundDatas_.size());
		SoundData& soundData = soundDatas_[soundDataHandle];

		// vector版チェック
		assert(!soundData.buffer.empty());

		// 既存Voice検索
		VoiceData* targetVoiceData = nullptr;

		for (VoiceData* voiceData : voiceDatas_) {
			if (voiceData->handle == soundDataHandle) {
				targetVoiceData = voiceData;
				break;
			}
		}

		// 新規Voice作成
		if (targetVoiceData == nullptr) {

			targetVoiceData = new VoiceData();
			targetVoiceData->handle = soundDataHandle;

			HRESULT result = xAudio2_->CreateSourceVoice(
				&targetVoiceData->sourceVoice,
				&soundData.wfex
			);

			assert(SUCCEEDED(result));

			voiceDatas_.insert(targetVoiceData);
		}

		assert(targetVoiceData->sourceVoice != nullptr);

		// バッファ設定
		XAUDIO2_BUFFER buf{};
		buf.pAudioData = soundData.buffer.data();
		buf.AudioBytes = static_cast<UINT32>(soundData.buffer.size());
		buf.Flags = XAUDIO2_END_OF_STREAM;

		if (loop) {
			buf.LoopCount = XAUDIO2_LOOP_INFINITE;
		}
		else {
			buf.LoopCount = 0;
		}

		// 再生準備
		HRESULT result = targetVoiceData->sourceVoice->Stop();
		result = targetVoiceData->sourceVoice->FlushSourceBuffers();

		result = targetVoiceData->sourceVoice->SubmitSourceBuffer(&buf);
		assert(SUCCEEDED(result));

		// 音量設定
		result = targetVoiceData->sourceVoice->SetVolume(volume);
		assert(SUCCEEDED(result));

		// 再生
		result = targetVoiceData->sourceVoice->Start();
		assert(SUCCEEDED(result));

		return soundDataHandle;
	}

	void AudioCommon::SoundUnload(SoundData* soundData) {
		//バッファのメモリを解放
		soundData->buffer.clear();
		soundData->wfex = {};
	}

	void AudioCommon::SoundStop(uint32_t voiceHandle) {
		auto it = std::find_if(voiceDatas_.begin(), voiceDatas_.end(),
			[voiceHandle](VoiceData* data) { return data->handle == voiceHandle; });

		if (it != voiceDatas_.end() && (*it)->sourceVoice) {
			(*it)->sourceVoice->Stop(0);  // サウンドの停止
			(*it)->sourceVoice->FlushSourceBuffers();  // バッファをクリア
		}
	}

	void AudioCommon::SoundPause(uint32_t voiceHandle) {
		auto it = std::find_if(voiceDatas_.begin(), voiceDatas_.end(),
			[voiceHandle](VoiceData* data) { return data->handle == voiceHandle; });

		if (it != voiceDatas_.end() && (*it)->sourceVoice) {
			(*it)->sourceVoice->Stop(0);  // 再生を停止（ポーズ）
		}
	}

	void AudioCommon::SoundResume(uint32_t voiceHandle) {
		auto it = std::find_if(voiceDatas_.begin(), voiceDatas_.end(),
			[voiceHandle](VoiceData* data) { return data->handle == voiceHandle; });

		if (it != voiceDatas_.end() && (*it)->sourceVoice) {
			(*it)->sourceVoice->Start(0);  // 再生を再開
		}
	}

	bool AudioCommon::GetIsPlaying(uint32_t voiceHandle) {
		auto it = std::find_if(voiceDatas_.begin(), voiceDatas_.end(),
			[voiceHandle](VoiceData* data) { return data->handle == voiceHandle; });

		if (it != voiceDatas_.end() && (*it)->sourceVoice) {
			XAUDIO2_VOICE_STATE state{};
			(*it)->sourceVoice->GetState(&state);

			return state.BuffersQueued > 0;
		}

		return false;
	}

	void AudioCommon::SetVolume(uint32_t voiceHandle, float volume) {
		auto it = std::find_if(voiceDatas_.begin(), voiceDatas_.end(),
			[voiceHandle](VoiceData* data) { return data->handle == voiceHandle; });

		if (it != voiceDatas_.end() && (*it)->sourceVoice) {
			(*it)->sourceVoice->SetVolume(volume);  // 音量の設定
		}
	}

	void AudioCommon::ClearSoundData() {
		for (auto& soundData : soundDatas_) {
			soundData.buffer.clear();
			soundData.name.clear();
		}
	}

	void AudioCommon::ClearVoiceData() {
		for (auto it = voiceDatas_.begin(); it != voiceDatas_.end(); ) {
			VoiceData* voiceData = *it;

			if (voiceData->sourceVoice) {
				voiceData->sourceVoice->Stop();         // 再生を停止
				voiceData->sourceVoice->DestroyVoice(); // ボイスリソースを解放
			}

			delete voiceData; // 動的に確保しているため解放
			it = voiceDatas_.erase(it); // イテレーターを安全に次へ進める
		}
	}

	void AudioCommon::ShutdownContainer() {
		ClearSoundData(); // SoundDataの解放
		ClearVoiceData(); // VoiceDataの解放
	}

}