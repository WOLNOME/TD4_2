#pragma once
#include <wrl.h>
#include <xaudio2.h>
#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <set>
#include <memory>

#pragma comment(lib,"xaudio2.lib")

namespace Norm {

	/// <summary>
	/// オーディオ機能の共通処理
	/// </summary>
	class AudioCommon {
	private:
		/// ============================== ///
		///		構造体(private)
		/// ============================== ///

		/// <summary>
		/// チャンクヘッダー
		/// </summary>
		struct ChunkHeader {
			char id[4];
			int32_t size;
		};

		/// <summary>
		/// RIFFヘッダー
		/// </summary>
		struct RiffHeader {
			ChunkHeader chunk;
			char type[4];
		};

		/// <summary>
		/// フォーマットチャンク
		/// </summary>
		struct FormatChunk {
			ChunkHeader chunk;
			WAVEFORMATEX fmt;
		};

	public:
		/// ============================== ///
		///		構造体(public)
		/// ============================== ///

		/// <summary>
		/// サウンドデータ
		/// </summary>
		struct SoundData {
			//波形フォーマット
			WAVEFORMATEX wfex;
			//バッファ
			std::vector<BYTE> buffer;
			//ファイルパス
			std::string name;
		};
		/// <summary>
		/// ボイスデータ
		/// </summary>
		struct VoiceData {
			//アクセスハンドル
			uint32_t handle = 0u;
			//ソースボイス
			IXAudio2SourceVoice* sourceVoice = nullptr;
		};

	private://シングルトン
		static std::unique_ptr<AudioCommon> instance_;

		AudioCommon() = default;//コンストラクタ隠蔽
		~AudioCommon() = default;//デストラクタ隠蔽
		AudioCommon(AudioCommon&) = delete;//コピーコンストラクタ封印
		AudioCommon& operator=(AudioCommon&) = delete;//コピー代入演算子封印

		friend struct std::default_delete<AudioCommon>;
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// シングルトンインスタンスの取得
		/// </summary>
		/// <returns>シングルトンインスタンス</returns>
		static AudioCommon* GetInstance();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();
		/// <summary>
		/// 音声データの読み込み
		/// </summary>
		/// <param name="filename">ファイルパス</param>
		/// <returns>音声データハンドル</returns>
		uint32_t SoundLoadFile(const std::string& filename);
		/// <summary>
		/// サウンドの再生
		/// </summary>
		/// <param name="soundDataHandle">音声データハンドル</param>
		/// <param name="loop">ループするか</param>
		/// <param name="volume">ボリューム(1.0fが通常)</param>
		/// <returns>ボイスハンドル</returns>
		uint32_t SoundPlayWave(uint32_t soundDataHandle, bool loop = false, float volume = 1.0f);
		/// <summary>
		/// サウンドの停止
		/// </summary>
		/// <param name="voiceHandle">ボイスハンドル</param>
		void SoundStop(uint32_t voiceHandle);
		/// <summary>
		/// サウンドの一時停止
		/// </summary>
		/// <param name="voiceHandle">ボイスハンドル</param>
		void SoundPause(uint32_t voiceHandle);
		/// <summary>
		/// サウンドの再開
		/// </summary>
		/// <param name="voiceHandle">ボイスハンドル</param>
		void SoundResume(uint32_t voiceHandle);


		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// 再生中かどうかを取得
		/// </summary>
		/// <param name="voiceHandle">ボイスハンドル</param>
		/// <returns>再生中かどうか</returns>
		bool GetIsPlaying(uint32_t voiceHandle);

		/// ============================== ///
		///		setter
		/// ============================== ///

		/// <summary>
		/// サウンドのボリューム設定
		/// </summary>
		/// <param name="voiceHandle">ボイスハンドル</param>
		/// <param name="volume">ボリューム(1.0fが通常)</param>
		void SetVolume(uint32_t voiceHandle, float volume);

		/// ============================== ///
		///		メンバ変数(public)
		/// ============================== ///

		// サウンドデータの最大数
		static const int kMaxSoundData = 128;

	private:
		/// ============================== ///
		///		メンバ関数(private)
		/// ============================== ///

		/// <summary>
		/// サウンドデータの解放
		/// </summary>
		/// <param name="soundData">サウンドデータ</param>
		void SoundUnload(SoundData* soundData);
		/// <summary>
		/// サウンドデータコンテナのクリア
		/// </summary>
		void ClearSoundData();
		/// <summary>
		/// ボイスデータコンテナのクリア
		/// </summary>
		void ClearVoiceData();
		/// <summary>
		/// コンテナのシャットダウン
		/// </summary>
		void ShutdownContainer();

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//xAudio2
		Microsoft::WRL::ComPtr<IXAudio2> xAudio2_ = nullptr;
		//マスターボイス
		IXAudio2MasteringVoice* masterVoice;
		//サウンドデータコンテナ
		std::array<SoundData, kMaxSoundData> soundDatas_;
		//ボイスデータコンテナ
		std::set<VoiceData*> voiceDatas_;


	};

}