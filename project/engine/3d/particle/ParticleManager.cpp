#include "ParticleManager.h"
#include "DirectXCommon.h"
#include "MainRender.h"
#include "GPUDescriptorManager.h"
#include "Logger.h"
#include "RandomStringUtil.h"
#include "Particle.h"
#include "SceneManager.h"
#include <numbers>
#include <random>
#include <algorithm>
#include <map>

#undef min
#undef max

namespace Norm {

	std::unique_ptr<ParticleManager> ParticleManager::instance_ = nullptr;

	ParticleManager* ParticleManager::GetInstance() {
		if (!instance_) {
			instance_ = std::unique_ptr<ParticleManager>(new ParticleManager());
		}
		return instance_.get();
	}

	void ParticleManager::Initialize() {
		//グラフィックスパイプラインの設定
		GenerateGraphicsPipeline();
		//コンピュートパイプラインの設定
		GenerateComputePipeline();
		//共通のCS用リソース初期化
		commonResourceForCS_ = CreateCommonResourceForCS();
		//UAVバッファの初期化
		InitUAVBuffer();

	}

	void ParticleManager::Update() {
		MainRender* mainRender = MainRender::GetInstance();
		GPUDescriptorManager* gpuDescriptorManager = GPUDescriptorManager::GetInstance();
		//UAVバリア挿入用ラムダ式
		auto InsertUAVBarriers = [&](std::vector<ID3D12Resource*>& resources) {
			std::vector<D3D12_RESOURCE_BARRIER> barriers;
			for (ID3D12Resource* resource : resources) {
				D3D12_RESOURCE_BARRIER barrier{};
				barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
				barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				barrier.UAV.pResource = resource;
				barriers.push_back(barrier);
			}
			mainRender->GetCommandList()->ResourceBarrier(UINT(barriers.size()), barriers.data());
			};

		//パーティクルが一つもセットされていなかったら抜ける
		if (particles.empty()) return;

		//GPUDescriptorHeapをコマンドリストにセット
		gpuDescriptorManager->SetDescriptorHeap(mainRender->GetCommandList());

		//タイムを更新
		commonResourceForCS_.mappedGeneralInfo[0].time += kDeltaTime;

		//std::mapにエミッターとJSONデータを転送（番号順になるため）
		std::map<uint32_t, EmitterForCS> emittersForCS;
		std::map<uint32_t, JsonInfoForCS> jsonInfosForCS;

		//各パーティクルの更新
		for (const auto& particle : particles) {
			//パーティクルのシーンタグが現在のシーンタグと違うなら次へ
			if (particle.second->sceneTag_ != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
				continue;
			}
			//パーティクルがアクティブでないなら次へ
			if (!particle.second->emitter_.isDraw) {
				continue;
			}

			//エミッターの更新（GPU送信用データに変換）
			particle.second->TraceEmitterForCS();

			//マップに格納
			emittersForCS[particle.second->emitterID_] = particle.second->emitterForCS_;
			jsonInfosForCS[particle.second->emitterID_] = particle.second->jsonInfoForCS_;
		}

		//送信用データの中身をクリア（データがない場所に0を入れる）
		std::memset(commonResourceForCS_.mappedEmitter, 0, sizeof(EmitterForCS) * kMaxNumEmitters);
		std::memset(commonResourceForCS_.mappedJsonInfo, 0, sizeof(JsonInfoForCS) * kMaxNumEmitters);

		//マップから各リソースに転送
		for (const auto& [id, emitter] : emittersForCS) {
			if (id >= kMaxNumEmitters) continue;

			commonResourceForCS_.mappedEmitter[id] = emitter;
			commonResourceForCS_.mappedJsonInfo[id] = jsonInfosForCS[id];
		}

		//バリア遷移(生成前状態保証)
		{
			std::vector<ID3D12Resource*> uavResources = {
				commonResourceForCS_.grainsResource.Get(),
				commonResourceForCS_.freeListIndexResource.Get(),
				commonResourceForCS_.freeListResource.Get()
			};

			InsertUAVBarriers(uavResources);
		}

		//粒の生成処理
		mainRender->GetCommandList()->SetComputeRootSignature(cRootSignature[1].Get());
		mainRender->GetCommandList()->SetPipelineState(computePipelineState[1].Get());
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(0, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.grainsUavIndex));
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(1, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.freeListIndexUavIndex));
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(2, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.freeListUavIndex));
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(3, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.emitterRangeUavIndex));
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(4, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.emitterSrvIndex));
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(5, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.jsonInfoSrvIndex));
		mainRender->GetCommandList()->SetComputeRootConstantBufferView(6, commonResourceForCS_.generalInfoResource->GetGPUVirtualAddress());

		//生成をGPUに依頼
		mainRender->GetCommandList()->Dispatch(kMaxNumEmitters, 1, 1);

		//生成処理が終わったのでOneShotのパーティクルを停止
		for (auto& particle : particles) {
			int effectStyle = particle.second->param_["EffectStyle"];
			if ((Particle::EffectStyle)effectStyle == Particle::EffectStyle::OneShot) {
				particle.second->emitter_.isPlay = false;
			}
		}

		//バリア遷移(更新前状態保証)
		{
			std::vector<ID3D12Resource*> uavResources = {
				commonResourceForCS_.grainsResource.Get(),
				commonResourceForCS_.freeListIndexResource.Get(),
				commonResourceForCS_.freeListResource.Get(),
				commonResourceForCS_.grainIndicesResource.Get(),
				commonResourceForCS_.emitterRangeResource.Get()
			};

			InsertUAVBarriers(uavResources);
		}

		//粒の更新処理
		mainRender->GetCommandList()->SetComputeRootSignature(cRootSignature[2].Get());
		mainRender->GetCommandList()->SetPipelineState(computePipelineState[2].Get());
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(0, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.grainsUavIndex));
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(1, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.freeListIndexUavIndex));
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(2, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.freeListUavIndex));
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(3, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.grainIndicesUavIndex));
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(4, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.emitterRangeUavIndex));
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(5, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.emitterSrvIndex));
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(6, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.jsonInfoSrvIndex));
		mainRender->GetCommandList()->SetComputeRootConstantBufferView(7, commonResourceForCS_.generalInfoResource->GetGPUVirtualAddress());

		//更新をGPUに依頼
		mainRender->GetCommandList()->Dispatch(UINT(kMaxNumGrains + 1023) / 1024, 1, 1);

		//バリア遷移(粒情報解析前状態保証)
		{
			std::vector<ID3D12Resource*> uavResources = {
				commonResourceForCS_.grainsResource.Get(),
				commonResourceForCS_.grainIndicesResource.Get(),
				commonResourceForCS_.emitterRangeResource.Get()
			};

			InsertUAVBarriers(uavResources);
		}

		//粒情報解析処理
		mainRender->GetCommandList()->SetComputeRootSignature(cRootSignature[3].Get());
		mainRender->GetCommandList()->SetPipelineState(computePipelineState[3].Get());
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(0, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.grainsUavIndex));
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(1, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.grainIndicesUavIndex));
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(2, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.emitterRangeUavIndex));
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(3, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.emitterSrvIndex));
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(4, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.jsonInfoSrvIndex));
		mainRender->GetCommandList()->SetComputeRootConstantBufferView(5, commonResourceForCS_.generalInfoResource->GetGPUVirtualAddress());

		//粒情報解析をGPUに依頼
		mainRender->GetCommandList()->Dispatch(UINT(kMaxNumGrains + 1023) / 1024, 1, 1);

		//描画に使うUAVリソースをSRV用にリソース遷移
		mainRender->TransitionResource(commonResourceForCS_.grainsResource.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ);
		mainRender->TransitionResource(commonResourceForCS_.grainIndicesResource.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ);
		mainRender->TransitionResource(commonResourceForCS_.emitterRangeResource.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ);

		//形状更新
		for (auto& particle : particles) {
			particle.second->shape_->Update();
		}
	}

	void ParticleManager::Draw() {
		//パーティクルが一つもセットされてなかったら抜ける
		if (particles.empty()) return;

		auto mainRender = MainRender::GetInstance();
		auto gpuDescriptorManager = GPUDescriptorManager::GetInstance();
		//カメラの有無チェック
		if (!camera_) {
			assert(0 && "カメラがセットされていません。");
		}
		//ルートシグネチャをセットするコマンド
		mainRender->GetCommandList()->SetGraphicsRootSignature(gRootSignature.Get());
		//プリミティブトポロジーをセットするコマンド
		mainRender->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//カメラ情報をVSに送信(一括)
		mainRender->GetCommandList()->SetGraphicsRootConstantBufferView(2, camera_->GetViewProjectionConstBuffer()->GetGPUVirtualAddress());
		//各パーティクルの粒配列情報をVSに送信
		mainRender->GetCommandList()->SetGraphicsRootDescriptorTable(1, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.grainsSrvIndex));
		//エミッター情報をVSに送信
		mainRender->GetCommandList()->SetGraphicsRootDescriptorTable(4, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.emitterSrvIndex));
		//JSON情報をVSに送信
		mainRender->GetCommandList()->SetGraphicsRootDescriptorTable(5, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.jsonInfoSrvIndex));
		//粒のIndex配列情報をVSに送信
		mainRender->GetCommandList()->SetGraphicsRootDescriptorTable(6, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.grainIndicesSrvIndex));
		//エミッターの範囲情報をVSに送信
		mainRender->GetCommandList()->SetGraphicsRootDescriptorTable(7, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.emitterRangeSrvIndex));
		//総合情報をVSに送信
		mainRender->GetCommandList()->SetGraphicsRootConstantBufferView(8, commonResourceForCS_.generalInfoResource->GetGPUVirtualAddress());

		//パーティクル個別の設定
		for (const auto& particle : particles) {
			//パーティクルのシーンタグが現在のシーンタグと違うなら次へ
			if (particle.second->sceneTag_ != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
				continue;
			}
			//パーティクルがアクティブでないなら次へ
			if (!particle.second->emitter_.isDraw) {
				continue;
			}

			//各パーティクルのブレンドモード情報からパイプラインステートを選択
			mainRender->GetCommandList()->SetPipelineState(graphicsPipelineState[particle.second->GetParam()["BlendMode"]].Get());
			//各パーティクルのエミッターIDをVSに送信
			mainRender->GetCommandList()->SetGraphicsRootConstantBufferView(9, particle.second->eachResourceForCS_.emitterIDResource->GetGPUVirtualAddress());

			//各パーティクル形状の描画
			int maxNumGrains = particle.second->GetParam()["MaxGrains"];
			particle.second->shape_->Draw(0, 3, (uint32_t)maxNumGrains, particle.second->textureHandle_);
		}

		//描画に使うSRVリソースをUAV用にリソース遷移
		mainRender->TransitionResource(commonResourceForCS_.grainsResource.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		mainRender->TransitionResource(commonResourceForCS_.grainIndicesResource.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		mainRender->TransitionResource(commonResourceForCS_.emitterRangeResource.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}

	void ParticleManager::Finalize() {
		//インスタンスを削除
		instance_.reset();
	}

	void ParticleManager::Regist(const std::string& name, Particle* particle) {
		//重複チェック
		if (particles.find(name) != particles.end()) {
			return;
		}

		//エミッターIDを設定
		particle->emitterID_ = AllocateEmitterID();
		particle->eachResourceForCS_.mappedEmitterID[0].id = particle->emitterID_;

		//登録
		particles[name] = particle;
	}

	bool ParticleManager::Rename(const std::string& preName, const std::string& postName) {
		//古い名前がコンテナ内に存在するかチェック
		auto it = particles.find(preName);
		if (it != particles.end()) {
			//新しい名前がコンテナ内に存在しないかチェック
			if (particles.find(postName) != particles.end()) {
				return false;
			}
			//名前の変更
			particles[postName] = it->second;  //新しい名前で登録
			particles.erase(it);	//古い名前での登録を削除

			return true;
		}

		return false;

	}

	void ParticleManager::Delete(const std::string& name) {
		// 名前がコンテナ内に存在するかチェック
		auto it = particles.find(name);
		if (it != particles.end()) {
			// エミッターIDの解放
			DiscardEmitterID(it->second->emitterID_);
			//コンテナから削除
			particles.erase(it);
		}
	}

	std::string ParticleManager::GenerateName(const std::string& name) {
		// 出力する名前
		std::string outputName = name + "_" + RandomStringUtil::GenerateRandomString(3);

		// 重複チェック用のラムダ式
		std::function<void(const std::string&)> checkDuplicate = [&](const std::string& name) {
			// 重複しているかチェック
			if (particles.find(name) != particles.end()) {
				// 重複しているので名前を変更
				outputName = name + "_" + RandomStringUtil::GenerateRandomString(3);
				checkDuplicate(outputName);
			}
			};

		// 重複チェック
		checkDuplicate(outputName);

		// 最終的に出力
		return outputName;
	}

	void ParticleManager::GenerateGraphicsPipeline() {
		HRESULT hr;
		auto dxCommon = DirectXCommon::GetInstance();

		// RootSignature作成
		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
		descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		//レジスタカウント
		int registerCountVS = 0;
		int registerCountPS = 0;
		//使用するデスクリプタの数
		int numDescriptors = 0;

		//DescriptorRangeの設定
		D3D12_DESCRIPTOR_RANGE descriptorRanges[6] = {};
		// 粒配列用の設定
		numDescriptors = 1;
		descriptorRanges[0].BaseShaderRegister = registerCountVS;
		descriptorRanges[0].NumDescriptors = numDescriptors;
		descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountVS += numDescriptors;
		// テクスチャ情報用の設定
		numDescriptors = 1;
		descriptorRanges[1].BaseShaderRegister = registerCountPS;
		descriptorRanges[1].NumDescriptors = numDescriptors;
		descriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountPS += numDescriptors;
		// エミッター情報用の設定
		numDescriptors = 1;
		descriptorRanges[2].BaseShaderRegister = registerCountVS;
		descriptorRanges[2].NumDescriptors = numDescriptors;
		descriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountVS += numDescriptors;
		// JSON情報用の設定
		numDescriptors = 1;
		descriptorRanges[3].BaseShaderRegister = registerCountVS;
		descriptorRanges[3].NumDescriptors = numDescriptors;
		descriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountVS += numDescriptors;
		// 粒のIndex情報用の設定
		numDescriptors = 1;
		descriptorRanges[4].BaseShaderRegister = registerCountVS;
		descriptorRanges[4].NumDescriptors = numDescriptors;
		descriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountVS += numDescriptors;
		// エミッターの範囲情報用の設定
		numDescriptors = 1;
		descriptorRanges[5].BaseShaderRegister = registerCountVS;
		descriptorRanges[5].NumDescriptors = numDescriptors;
		descriptorRanges[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRanges[5].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountVS += numDescriptors;

		// RootParameter作成
		std::vector<D3D12_ROOT_PARAMETER> rootParameters;

		// マテリアル情報用の設定(0)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			param.Descriptor.ShaderRegister = 0;
			rootParameters.push_back(param);
		}
		// 粒配列情報用の設定(1)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[0];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		// カメラ情報用の設定(2)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			param.Descriptor.ShaderRegister = 0;
			rootParameters.push_back(param);
		}
		// テクスチャ情報用の設定(3)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[1];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//エミッター情報用の設定(4)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[2];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//JSON情報用の設定(5)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[3];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//粒のIndex情報用の設定(6)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[4];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//エミッターの範囲情報用の設定(7)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[5];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//総合情報用の設定(8)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			param.Descriptor.ShaderRegister = 1;
			rootParameters.push_back(param);
		}
		//エミッターID用の設定(9)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			param.Descriptor.ShaderRegister = 2;
			rootParameters.push_back(param);
		}

		// Sampler作成
		std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers(1);
		staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
		staticSamplers[0].ShaderRegister = 0;
		staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		//Signatureに反映
		descriptionRootSignature.pParameters = rootParameters.data();
		descriptionRootSignature.NumParameters = static_cast<UINT>(rootParameters.size());
		descriptionRootSignature.pStaticSamplers = staticSamplers.data();
		descriptionRootSignature.NumStaticSamplers = static_cast<UINT>(staticSamplers.size());

		//シリアライズしてバイナリにする
		Microsoft::WRL::ComPtr<ID3D10Blob> signatireBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		hr = D3D12SerializeRootSignature(&descriptionRootSignature,
			D3D_ROOT_SIGNATURE_VERSION_1, &signatireBlob, &errorBlob);
		if (FAILED(hr)) {
			Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
			assert(false);
		}
		//バイナリをもとに生成
		hr = dxCommon->GetDevice()->CreateRootSignature(0, signatireBlob->GetBufferPointer(),
			signatireBlob->GetBufferSize(), IID_PPV_ARGS(&gRootSignature));
		assert(SUCCEEDED(hr));

		//InputLayout
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
		inputElementDescs[0].SemanticName = "POSITION";
		inputElementDescs[0].SemanticIndex = 0;
		inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElementDescs[1].SemanticName = "TEXCOORD";
		inputElementDescs[1].SemanticIndex = 0;
		inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElementDescs[2].SemanticName = "NORMAL";
		inputElementDescs[2].SemanticIndex = 0;
		inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = inputElementDescs;
		inputLayoutDesc.NumElements = _countof(inputElementDescs);

		//BlendStateの設定
		std::array<D3D12_BLEND_DESC, (int)BlendMode::kMaxBlendModeNum> blendDesc{};
		for (int i = 0; i < (int)BlendMode::kMaxBlendModeNum; i++) {
			//共通部分の設定
			blendDesc[i].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
			blendDesc[i].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
			blendDesc[i].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
			blendDesc[i].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

			switch (i) {
			case (int)BlendMode::None:
				blendDesc[i].RenderTarget[0].BlendEnable = FALSE;
				break;
			case (int)BlendMode::Normal:
				blendDesc[i].RenderTarget[0].BlendEnable = TRUE;
				blendDesc[i].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
				blendDesc[i].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
				blendDesc[i].RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
				break;
			case (int)BlendMode::Add:
				blendDesc[i].RenderTarget[0].BlendEnable = TRUE;
				blendDesc[i].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
				blendDesc[i].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
				blendDesc[i].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
				break;
			case (int)BlendMode::Subtract:
				blendDesc[i].RenderTarget[0].BlendEnable = TRUE;
				blendDesc[i].RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
				blendDesc[i].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
				blendDesc[i].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
				break;
			case (int)BlendMode::Multiply:
				blendDesc[i].RenderTarget[0].BlendEnable = TRUE;
				blendDesc[i].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
				blendDesc[i].RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
				blendDesc[i].RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
				break;
			case (int)BlendMode::Screen:
				blendDesc[i].RenderTarget[0].BlendEnable = TRUE;
				blendDesc[i].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
				blendDesc[i].RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
				blendDesc[i].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
				break;
			case (int)BlendMode::Execlution:
				blendDesc[i].RenderTarget[0].BlendEnable = TRUE;
				blendDesc[i].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
				blendDesc[i].RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
				blendDesc[i].RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_COLOR;
				break;
			default:
				break;
			}
		}

		//RasterizerStateの設定
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		//裏面も表示する
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		//三角形の中を塗りつぶす
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

		//Shaderをコンパイルする
		Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = dxCommon->CompileShader(L"Resources/shaders/particle/Particle.VS.hlsl",
			L"vs_6_0");
		assert(vertexShaderBlob != nullptr);

		Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = dxCommon->CompileShader(L"Resources/shaders/particle/Particle.PS.hlsl",
			L"ps_6_0");
		assert(pixelShaderBlob != nullptr);

		//DepthStencilStateの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		std::array<D3D12_GRAPHICS_PIPELINE_STATE_DESC, (int)BlendMode::kMaxBlendModeNum> graphicsPipelineStateDesc{};
		for (int i = 0; i < (int)BlendMode::kMaxBlendModeNum; i++) {
			graphicsPipelineStateDesc[i].pRootSignature = gRootSignature.Get();
			graphicsPipelineStateDesc[i].InputLayout = inputLayoutDesc;
			graphicsPipelineStateDesc[i].VS = { vertexShaderBlob->GetBufferPointer(),
			vertexShaderBlob->GetBufferSize() };
			graphicsPipelineStateDesc[i].PS = { pixelShaderBlob->GetBufferPointer(),
			pixelShaderBlob->GetBufferSize() };
			graphicsPipelineStateDesc[i].BlendState = blendDesc[i];
			graphicsPipelineStateDesc[i].RasterizerState = rasterizerDesc;
			//書き込むRTVの情報
			graphicsPipelineStateDesc[i].NumRenderTargets = 1;
			graphicsPipelineStateDesc[i].RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			//利用するトポロジのタイプ。三角形
			graphicsPipelineStateDesc[i].PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			//どのように画面に色を打ち込むかの設定
			graphicsPipelineStateDesc[i].SampleDesc.Count = 1;
			graphicsPipelineStateDesc[i].SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
			//DepthStencilの設定
			graphicsPipelineStateDesc[i].DepthStencilState = depthStencilDesc;
			graphicsPipelineStateDesc[i].DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

			//実際に生成
			hr = dxCommon->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc[i],
				IID_PPV_ARGS(&graphicsPipelineState[i]));
			assert(SUCCEEDED(hr));
		}
	}

	void ParticleManager::GenerateComputePipeline() {
		//初期化用CPSOの設定
		InitCPSOOption();
		//エミット用CPSOの設定
		EmitCPSOOption();
		//更新用CPSOの設定
		UpdateCPSOOption();
		//粒情報解析用CPSOの設定
		LocksmithCPSOOption();
	}

	void ParticleManager::InitUAVBuffer() {
		MainRender* mainRender = MainRender::GetInstance();
		GPUDescriptorManager* gpuDescriptorManager = GPUDescriptorManager::GetInstance();
		//GPUDescriptorHeapをコマンドリストにセット
		gpuDescriptorManager->SetDescriptorHeap(mainRender->GetCommandList());
		//CSでUAVリソースの初期化処理
		mainRender->GetCommandList()->SetComputeRootSignature(cRootSignature[0].Get());
		mainRender->GetCommandList()->SetPipelineState(computePipelineState[0].Get());
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(0, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.grainsUavIndex));
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(1, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.freeListIndexUavIndex));
		mainRender->GetCommandList()->SetComputeRootDescriptorTable(2, gpuDescriptorManager->GetGPUDescriptorHandle(commonResourceForCS_.freeListUavIndex));
		mainRender->GetCommandList()->SetComputeRootConstantBufferView(3, commonResourceForCS_.generalInfoResource->GetGPUVirtualAddress());

		mainRender->GetCommandList()->Dispatch(UINT(kMaxNumGrains + 1023) / 1024, 1, 1);
	}

	void ParticleManager::InitCPSOOption() {
		HRESULT hr;
		//RootSignature作成
		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
		descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		//レジスタカウント
		int registerCountT = 0;		//structuredBuffer専用
		int registerCountU = 0;		//RWStructuredBuffer専用
		//使用するデスクリプタの数
		int numDescriptors = 0;

		//DescriptorRangeの設定
		D3D12_DESCRIPTOR_RANGE descriptorRanges[3] = {};
		// 粒配列用の設定
		numDescriptors = 1;
		descriptorRanges[0].BaseShaderRegister = registerCountU;
		descriptorRanges[0].NumDescriptors = numDescriptors;
		descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountU += numDescriptors;
		// フリーリストインデックス用の設定
		numDescriptors = 1;
		descriptorRanges[1].BaseShaderRegister = registerCountU;
		descriptorRanges[1].NumDescriptors = numDescriptors;
		descriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		descriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountU += numDescriptors;
		// フリーリスト用の設定
		numDescriptors = 1;
		descriptorRanges[2].BaseShaderRegister = registerCountU;
		descriptorRanges[2].NumDescriptors = numDescriptors;
		descriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		descriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountU += numDescriptors;

		std::vector<D3D12_ROOT_PARAMETER> rootParameters;
		//粒配列用の設定(0)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[0];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//フリーリストインデックス用の設定(1)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[1];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//フリーリスト用の設定(2)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[2];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//総合情報用の設定(3)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.Descriptor.ShaderRegister = 0;
			rootParameters.push_back(param);
		}
		//Signatureに反映
		descriptionRootSignature.pParameters = rootParameters.data();
		descriptionRootSignature.NumParameters = static_cast<UINT>(rootParameters.size());

		//シリアライズしてバイナリにする
		Microsoft::WRL::ComPtr<ID3D10Blob> signatireBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		hr = D3D12SerializeRootSignature(&descriptionRootSignature,
			D3D_ROOT_SIGNATURE_VERSION_1, &signatireBlob, &errorBlob);
		if (FAILED(hr)) {
			Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
			assert(false);
		}
		//バイナリをもとにコンピュートルートシグネチャを生成
		hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatireBlob->GetBufferPointer(),
			signatireBlob->GetBufferSize(), IID_PPV_ARGS(&cRootSignature[0]));
		assert(SUCCEEDED(hr));

		//Shaderをコンパイルする
		Microsoft::WRL::ComPtr<IDxcBlob> computeShaderBlob;
		computeShaderBlob = DirectXCommon::GetInstance()->CompileShader(L"Resources/shaders/particle/InitParticle.CS.hlsl",
			L"cs_6_0");
		assert(computeShaderBlob != nullptr);

		//コンピュートシェーダー用のPSOの設定
		D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
		computePipelineStateDesc.CS = {
			.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
			.BytecodeLength = computeShaderBlob->GetBufferSize()
		};
		computePipelineStateDesc.pRootSignature = cRootSignature[0].Get();
		hr = DirectXCommon::GetInstance()->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&computePipelineState[0]));
	}

	void ParticleManager::EmitCPSOOption() {
		HRESULT hr;
		//RootSignature作成
		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
		descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		//レジスタカウント
		int registerCountT = 0;		//structuredBuffer専用
		int registerCountU = 0;		//RWStructuredBuffer専用
		//使用するデスクリプタの数
		int numDescriptors = 0;

		//DescriptorRangeを作成
		D3D12_DESCRIPTOR_RANGE descriptorRanges[6] = {};
		// 粒配列用の設定
		numDescriptors = 1;
		descriptorRanges[0].BaseShaderRegister = registerCountU;
		descriptorRanges[0].NumDescriptors = numDescriptors;
		descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountU += numDescriptors;
		// フリーリストインデックス用の設定
		numDescriptors = 1;
		descriptorRanges[1].BaseShaderRegister = registerCountU;
		descriptorRanges[1].NumDescriptors = numDescriptors;
		descriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		descriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountU += numDescriptors;
		// フリーリスト用の設定
		numDescriptors = 1;
		descriptorRanges[2].BaseShaderRegister = registerCountU;
		descriptorRanges[2].NumDescriptors = numDescriptors;
		descriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		descriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountU += numDescriptors;
		// エミッターレンジ用の設定
		numDescriptors = 1;
		descriptorRanges[3].BaseShaderRegister = registerCountU;
		descriptorRanges[3].NumDescriptors = numDescriptors;
		descriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		descriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountU += numDescriptors;
		// エミッター情報用の設定
		numDescriptors = 1;
		descriptorRanges[4].BaseShaderRegister = registerCountT;
		descriptorRanges[4].NumDescriptors = numDescriptors;
		descriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountT += numDescriptors;
		// JSON情報用の設定
		numDescriptors = 1;
		descriptorRanges[5].BaseShaderRegister = registerCountT;
		descriptorRanges[5].NumDescriptors = numDescriptors;
		descriptorRanges[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRanges[5].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountT += numDescriptors;

		std::vector<D3D12_ROOT_PARAMETER> rootParameters;
		//粒配列用の設定(0)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[0];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//フリーリストインデックス用の設定(1)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[1];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//フリーリスト用の設定(2)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[2];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//エミッターレンジ用の設定(3)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[3];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//エミッター情報用の設定(4)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[4];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//JSON情報用の設定(5)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[5];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//総合情報用の設定(5)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.Descriptor.ShaderRegister = 0;
			rootParameters.push_back(param);
		}
		//Signatureに反映
		descriptionRootSignature.pParameters = rootParameters.data();
		descriptionRootSignature.NumParameters = static_cast<UINT>(rootParameters.size());

		//シリアライズしてバイナリにする
		Microsoft::WRL::ComPtr<ID3D10Blob> signatireBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		hr = D3D12SerializeRootSignature(&descriptionRootSignature,
			D3D_ROOT_SIGNATURE_VERSION_1, &signatireBlob, &errorBlob);
		if (FAILED(hr)) {
			Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
			assert(false);
		}
		//バイナリをもとにコンピュートルートシグネチャを生成
		hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatireBlob->GetBufferPointer(),
			signatireBlob->GetBufferSize(), IID_PPV_ARGS(&cRootSignature[1]));
		assert(SUCCEEDED(hr));

		//Shaderをコンパイルする
		Microsoft::WRL::ComPtr<IDxcBlob> computeShaderBlob;
		computeShaderBlob = DirectXCommon::GetInstance()->CompileShader(L"Resources/shaders/particle/EmitParticle.CS.hlsl",
			L"cs_6_0");
		assert(computeShaderBlob != nullptr);

		//コンピュートシェーダー用のPSOの設定
		D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
		computePipelineStateDesc.CS = {
			.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
			.BytecodeLength = computeShaderBlob->GetBufferSize()
		};
		computePipelineStateDesc.pRootSignature = cRootSignature[1].Get();
		hr = DirectXCommon::GetInstance()->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&computePipelineState[1]));
	}

	void ParticleManager::UpdateCPSOOption() {
		HRESULT hr;
		//RootSignature作成
		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
		descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		//レジスタカウント
		int registerCountT = 0;		//structuredBuffer専用
		int registerCountU = 0;		//RWStructuredBuffer専用
		//使用するデスクリプタの数
		int numDescriptors = 0;

		//DescriptorRangeを作成
		D3D12_DESCRIPTOR_RANGE descriptorRanges[7] = {};
		// 粒配列用の設定
		numDescriptors = 1;
		descriptorRanges[0].BaseShaderRegister = registerCountU;
		descriptorRanges[0].NumDescriptors = numDescriptors;
		descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountU += numDescriptors;
		// フリーリストインデックス用の設定
		numDescriptors = 1;
		descriptorRanges[1].BaseShaderRegister = registerCountU;
		descriptorRanges[1].NumDescriptors = numDescriptors;
		descriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		descriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountU += numDescriptors;
		// フリーリスト用の設定
		numDescriptors = 1;
		descriptorRanges[2].BaseShaderRegister = registerCountU;
		descriptorRanges[2].NumDescriptors = numDescriptors;
		descriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		descriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountU += numDescriptors;
		// 粒のIndex配列情報用の設定
		numDescriptors = 1;
		descriptorRanges[3].BaseShaderRegister = registerCountU;
		descriptorRanges[3].NumDescriptors = numDescriptors;
		descriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		descriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountU += numDescriptors;
		// エミッターの範囲情報用の設定
		numDescriptors = 1;
		descriptorRanges[4].BaseShaderRegister = registerCountU;
		descriptorRanges[4].NumDescriptors = numDescriptors;
		descriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		descriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountU += numDescriptors;
		// エミッター情報用の設定
		numDescriptors = 1;
		descriptorRanges[5].BaseShaderRegister = registerCountT;
		descriptorRanges[5].NumDescriptors = numDescriptors;
		descriptorRanges[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRanges[5].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountT += numDescriptors;
		// JSON情報用の設定
		numDescriptors = 1;
		descriptorRanges[6].BaseShaderRegister = registerCountT;
		descriptorRanges[6].NumDescriptors = numDescriptors;
		descriptorRanges[6].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRanges[6].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountT += numDescriptors;

		std::vector<D3D12_ROOT_PARAMETER> rootParameters;
		//粒配列用の設定(0)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[0];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//フリーリストインデックス用の設定(1)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[1];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//フリーリスト用の設定(2)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[2];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//粒のIndex配列情報用の設定(3)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[3];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//エミッターの範囲情報用の設定(4)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[4];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//エミッター情報用の設定(5)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[5];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//JSON情報用の設定(6)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[6];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//総合情報用の設定(7)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.Descriptor.ShaderRegister = 0;
			rootParameters.push_back(param);
		}
		//Signatureに反映
		descriptionRootSignature.pParameters = rootParameters.data();
		descriptionRootSignature.NumParameters = static_cast<UINT>(rootParameters.size());

		//シリアライズしてバイナリにする
		Microsoft::WRL::ComPtr<ID3D10Blob> signatireBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		hr = D3D12SerializeRootSignature(&descriptionRootSignature,
			D3D_ROOT_SIGNATURE_VERSION_1, &signatireBlob, &errorBlob);
		if (FAILED(hr)) {
			Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
			assert(false);
		}
		//バイナリをもとにコンピュートルートシグネチャを生成
		hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatireBlob->GetBufferPointer(),
			signatireBlob->GetBufferSize(), IID_PPV_ARGS(&cRootSignature[2]));
		assert(SUCCEEDED(hr));

		//Shaderをコンパイルする
		Microsoft::WRL::ComPtr<IDxcBlob> computeShaderBlob;
		computeShaderBlob = DirectXCommon::GetInstance()->CompileShader(L"Resources/shaders/particle/UpdateParticle.CS.hlsl",
			L"cs_6_0");
		assert(computeShaderBlob != nullptr);

		//コンピュートシェーダー用のPSOの設定
		D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
		computePipelineStateDesc.CS = {
			.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
			.BytecodeLength = computeShaderBlob->GetBufferSize()
		};
		computePipelineStateDesc.pRootSignature = cRootSignature[2].Get();
		hr = DirectXCommon::GetInstance()->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&computePipelineState[2]));
	}

	void ParticleManager::LocksmithCPSOOption() {
		HRESULT hr;
		//RootSignature作成
		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
		descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		//レジスタカウント
		int registerCountT = 0;		//structuredBuffer専用
		int registerCountU = 0;		//RWStructuredBuffer専用
		//使用するデスクリプタの数
		int numDescriptors = 0;

		//DescriptorRangeを作成
		D3D12_DESCRIPTOR_RANGE descriptorRanges[5] = {};
		// 粒配列用の設定
		numDescriptors = 1;
		descriptorRanges[0].BaseShaderRegister = registerCountU;
		descriptorRanges[0].NumDescriptors = numDescriptors;
		descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountU += numDescriptors;
		// 粒Index配列用の設定
		numDescriptors = 1;
		descriptorRanges[1].BaseShaderRegister = registerCountU;
		descriptorRanges[1].NumDescriptors = numDescriptors;
		descriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		descriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountU += numDescriptors;
		// エミッターの範囲配列用の設定
		numDescriptors = 1;
		descriptorRanges[2].BaseShaderRegister = registerCountU;
		descriptorRanges[2].NumDescriptors = numDescriptors;
		descriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		descriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountU += numDescriptors;
		// エミッター情報用の設定
		numDescriptors = 1;
		descriptorRanges[3].BaseShaderRegister = registerCountT;
		descriptorRanges[3].NumDescriptors = numDescriptors;
		descriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountT += numDescriptors;
		// JSON情報用の設定
		numDescriptors = 1;
		descriptorRanges[4].BaseShaderRegister = registerCountT;
		descriptorRanges[4].NumDescriptors = numDescriptors;
		descriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountT += numDescriptors;

		std::vector<D3D12_ROOT_PARAMETER> rootParameters;
		//粒配列用の設定(0)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[0];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//粒Index配列用の設定(1)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[1];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//エミッターの範囲配列用の設定(2)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[2];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//エミッター情報用の設定(3)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[3];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//JSON情報用の設定(4)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[4];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//総合情報用の設定(5)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.Descriptor.ShaderRegister = 0;
			rootParameters.push_back(param);
		}

		//Signatureに反映
		descriptionRootSignature.pParameters = rootParameters.data();
		descriptionRootSignature.NumParameters = static_cast<UINT>(rootParameters.size());

		//シリアライズしてバイナリにする
		Microsoft::WRL::ComPtr<ID3D10Blob> signatireBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		hr = D3D12SerializeRootSignature(&descriptionRootSignature,
			D3D_ROOT_SIGNATURE_VERSION_1, &signatireBlob, &errorBlob);
		if (FAILED(hr)) {
			Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
			assert(false);
		}
		//バイナリをもとにコンピュートルートシグネチャを生成
		hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatireBlob->GetBufferPointer(),
			signatireBlob->GetBufferSize(), IID_PPV_ARGS(&cRootSignature[3]));
		assert(SUCCEEDED(hr));

		//Shaderをコンパイルする
		Microsoft::WRL::ComPtr<IDxcBlob> computeShaderBlob;
		computeShaderBlob = DirectXCommon::GetInstance()->CompileShader(L"Resources/shaders/particle/LocksmithParticle.CS.hlsl",
			L"cs_6_0");
		assert(computeShaderBlob != nullptr);

		//コンピュートシェーダー用のPSOの設定
		D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
		computePipelineStateDesc.CS = {
			.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
			.BytecodeLength = computeShaderBlob->GetBufferSize()
		};
		computePipelineStateDesc.pRootSignature = cRootSignature[3].Get();
		hr = DirectXCommon::GetInstance()->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&computePipelineState[3]));
	}

	ParticleManager::CommonResourceForCS ParticleManager::CreateCommonResourceForCS() {
		CommonResourceForCS result;
		DirectXCommon* dxCommon = DirectXCommon::GetInstance();
		GPUDescriptorManager* gpuDescriptorManager = GPUDescriptorManager::GetInstance();

		//粒
		{
			//粒の情報用のResorceを確保
			result.grainsResource = dxCommon->CreateUAVBufferResource(sizeof(GrainForCS) * kMaxNumGrains);
			//粒情報用のuavを作成。RBStructuredBufferでアクセスできるようにする
			result.grainsUavIndex = gpuDescriptorManager->Allocate();
			gpuDescriptorManager->CreateUAVforRWStructuredBuffer(result.grainsUavIndex, result.grainsResource.Get(), UINT(kMaxNumGrains), sizeof(GrainForCS));
			//粒情報用のsrvを作成。StructuredBufferでアクセスできるようにする
			result.grainsSrvIndex = gpuDescriptorManager->Allocate();
			gpuDescriptorManager->CreateSRVforStructuredBuffer(result.grainsSrvIndex, result.grainsResource.Get(), UINT(kMaxNumGrains), sizeof(GrainForCS));
		}
		//粒のIndex
		{
			//粒のIndexの情報用のResorceを確保
			result.grainIndicesResource = dxCommon->CreateUAVBufferResource(sizeof(int32_t) * kMaxNumGrains);
			//粒のIndex情報用のuavを作成。RBStructuredBufferでアクセスできるようにする
			result.grainIndicesUavIndex = gpuDescriptorManager->Allocate();
			gpuDescriptorManager->CreateUAVforRWStructuredBuffer(result.grainIndicesUavIndex, result.grainIndicesResource.Get(), UINT(kMaxNumGrains), sizeof(int32_t));
			//粒のIndex情報用のsrvを作成。StructuredBufferでアクセスできるようにする
			result.grainIndicesSrvIndex = gpuDescriptorManager->Allocate();
			gpuDescriptorManager->CreateSRVforStructuredBuffer(result.grainIndicesSrvIndex, result.grainIndicesResource.Get(), UINT(kMaxNumGrains), sizeof(int32_t));
		}
		//エミッターの範囲
		{
			//エミッターの範囲情報用のResorceを確保
			result.emitterRangeResource = dxCommon->CreateUAVBufferResource(sizeof(EmitterRangeForCS) * kMaxNumEmitters);
			//エミッターの範囲情報用のuavを作成。RBStructuredBufferでアクセスできるようにする
			result.emitterRangeUavIndex = gpuDescriptorManager->Allocate();
			gpuDescriptorManager->CreateUAVforRWStructuredBuffer(result.emitterRangeUavIndex, result.emitterRangeResource.Get(), UINT(kMaxNumEmitters), sizeof(EmitterRangeForCS));
			//エミッターの範囲情報用のsrvを作成。StructuredBufferでアクセスできるようにする
			result.emitterRangeSrvIndex = gpuDescriptorManager->Allocate();
			gpuDescriptorManager->CreateSRVforStructuredBuffer(result.emitterRangeSrvIndex, result.emitterRangeResource.Get(), UINT(kMaxNumEmitters), sizeof(EmitterRangeForCS));
		}
		//フリーリストインデックス
		{
			//フリーリストインデックス用のResourceを確保
			result.freeListIndexResource = dxCommon->CreateUAVBufferResource(sizeof(int32_t));
			//フリーリストインデックス用のuavを作成。RWStructuredBufferでアクセスできるようにする
			result.freeListIndexUavIndex = gpuDescriptorManager->Allocate();
			gpuDescriptorManager->CreateUAVforRWStructuredBuffer(result.freeListIndexUavIndex, result.freeListIndexResource.Get(), 1, sizeof(int32_t));
		}
		//フリーリスト
		{
			//フリーリスト用のResourceを確保
			result.freeListResource = dxCommon->CreateUAVBufferResource(sizeof(uint32_t) * kMaxNumGrains);
			//フリーリスト用のuavを作成。RWStructuredBufferでアクセスできるようにする
			result.freeListUavIndex = gpuDescriptorManager->Allocate();
			gpuDescriptorManager->CreateUAVforRWStructuredBuffer(result.freeListUavIndex, result.freeListResource.Get(),
				UINT(kMaxNumGrains), sizeof(uint32_t));
		}
		//エミッター情報
		{
			//エミッター情報用のResorceを確保
			result.emitterResource = dxCommon->CreateBufferResource(sizeof(EmitterForCS) * kMaxNumEmitters);
			//マッピング
			result.emitterResource->Map(0, nullptr, reinterpret_cast<void**>(&result.mappedEmitter));
			//エミッター情報用のsrvを作成。StructuredBufferでアクセスできるようにする
			result.emitterSrvIndex = gpuDescriptorManager->Allocate();
			gpuDescriptorManager->CreateSRVforStructuredBuffer(result.emitterSrvIndex, result.emitterResource.Get(), UINT(kMaxNumEmitters), sizeof(EmitterForCS));
		}
		//JSON情報
		{
			//JSON情報用のResorceを確保
			result.jsonInfoResource = dxCommon->CreateBufferResource(sizeof(JsonInfoForCS) * kMaxNumEmitters);
			//マッピング
			result.jsonInfoResource->Map(0, nullptr, reinterpret_cast<void**>(&result.mappedJsonInfo));
			//JSON情報用のsrvを作成。StructuredBufferでアクセスできるようにする
			result.jsonInfoSrvIndex = gpuDescriptorManager->Allocate();
			gpuDescriptorManager->CreateSRVforStructuredBuffer(result.jsonInfoSrvIndex, result.jsonInfoResource.Get(), UINT(kMaxNumEmitters), sizeof(JsonInfoForCS));
		}
		//総合情報
		{
			//総合情報用のResorceを確保
			result.generalInfoResource = dxCommon->CreateBufferResource(sizeof(GeneralInfoForCS));
			//マッピング
			result.generalInfoResource->Map(0, nullptr, reinterpret_cast<void**>(&result.mappedGeneralInfo));
			//データ入力
			result.mappedGeneralInfo[0].time = 0.0f;
			result.mappedGeneralInfo[0].deltaTime = kDeltaTime;
			result.mappedGeneralInfo[0].maxGrains = kMaxNumGrains;
			result.mappedGeneralInfo[0].maxEmitters = kMaxNumEmitters;
		}

		return result;
	}

	uint32_t ParticleManager::AllocateEmitterID() {
		// まずは未使用IDを優先して割り当て
		if (emitterIDCounter_ < kMaxNumEmitters) {
			return emitterIDCounter_++;
		}

		// 未使用IDが尽きたらフリーリストから再利用
		if (!freeEmitterIDList_.empty()) {
			uint32_t result = freeEmitterIDList_.front();
			freeEmitterIDList_.pop_front();
			return result;
		}

		// どちらもダメならエラー
		assert(0 && "エミッターが最大数に達しました");
		return UINT32_MAX;
	}

	void ParticleManager::DiscardEmitterID(uint32_t id) {
		//範囲チェック
		if (id >= kMaxNumEmitters) {
			assert(0 && "無効なエミッターIDです");
			return;
		}

		//二重解放防止
		auto it = std::find(freeEmitterIDList_.begin(), freeEmitterIDList_.end(), id);
		if (it != freeEmitterIDList_.end()) {
			assert(0 && "エミッターIDの二重解放です");
			return;
		}

		//フリーリストに戻す
		freeEmitterIDList_.push_back(id);
	}

}