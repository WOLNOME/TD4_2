#include "BulletTrailManager.h"
#include "DirectXCommon.h"
#include "MainRender.h"
#include "Logger.h"
#include "RandomStringUtil.h"
#include "BulletTrail.h"
#include "SceneManager.h"
#include <cassert>

namespace Norm {

	std::unique_ptr<BulletTrailManager> BulletTrailManager::instance_ = nullptr;

	BulletTrailManager* BulletTrailManager::GetInstance() {
		if (!instance_) {
			instance_ = std::unique_ptr<BulletTrailManager>(new BulletTrailManager());
		}
		return instance_.get();
	}

	void BulletTrailManager::Initialize() {
		//グラフィックスパイプラインの生成
		GenerateGraphicsPipeline();
	}

	void BulletTrailManager::Update() {
		//弾丸トレールが一つもセットされていなかったら抜ける
		if (bulletTrails_.empty()) return;

		//全弾丸トレールの更新
		for (const auto& bulletTrail : bulletTrails_) {
			bulletTrail.second->Update();
		}
	}

	void BulletTrailManager::Draw() {
		//弾丸トレールが一つもセットされていなかったら抜ける
		if (bulletTrails_.empty()) return;

		auto mainRender = MainRender::GetInstance();

		//カメラチェック
		if (!camera_) {
			assert(0 && "カメラがセットされていません");
		}

		//全弾丸共通の処理
		{
			//ルートシグネチャのセット
			MainRender::GetInstance()->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
			//グラフィックスパイプラインステートのセット
			MainRender::GetInstance()->GetCommandList()->SetPipelineState(graphicsPipelineState_[(int)BlendMode::Add].Get());
			//プリミティブトポロジーのセット
			MainRender::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}

		//全弾丸トレールの描画
		for (const auto& bulletTrail : bulletTrails_) {
			//シーンタグが現在のシーンと違うのであれば次へ
			if (bulletTrail.second->sceneTag_ != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
				continue;
			}
			//表示しないのであれば次へ
			if (!bulletTrail.second->isDisplay_) {
				continue;
			}

			bulletTrail.second->Draw(camera_);
		}
	}

	void BulletTrailManager::Finalize() {
		//インスタンスを削除
		instance_.reset();
	}

	void BulletTrailManager::RegisterBulletTrail(const std::string& name, BulletTrail* bulletTrail) {
		//重複チェック
		if (bulletTrails_.find(name) != bulletTrails_.end()) {
			return;
		}
		//登録
		bulletTrails_[name] = bulletTrail;
	}

	void BulletTrailManager::DeleteBulletTrail(const std::string& name) {
		//名前がコンテナ内に存在するかチェック
		auto it = bulletTrails_.find(name);
		if (it != bulletTrails_.end()) {
			bulletTrails_.erase(it);  //コンテナから削除
		}
	}

	std::string BulletTrailManager::GenerateName(const std::string& name) {
		//出力する名前
		std::string outputName = name + "_" + RandomStringUtil::GenerateRandomString(3);

		//重複チェック用のラムダ式
		std::function<void(const std::string&)> checkDuplicate = [&](const std::string& name) {
			//重複しているかチェック
			if (bulletTrails_.find(name) != bulletTrails_.end()) {
				//重複しているので名前を変更
				outputName = name + "_" + RandomStringUtil::GenerateRandomString(3);
				checkDuplicate(outputName);
			}
			};

		//重複チェック
		checkDuplicate(outputName);

		//最終的に出力
		return outputName;
	}

	void BulletTrailManager::GenerateGraphicsPipeline() {
		HRESULT hr;
		auto dxCommon = DirectXCommon::GetInstance();

		//RootSignature作成
		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
		descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		//レジスタカウント
		int registerCountVS = 0;
		int registerCountPS = 0;
		//使用するデスクリプタの数
		int numDescriptors = 0;

		//RootParameter作成
		std::vector<D3D12_ROOT_PARAMETER> rootParameters;

		//カメラ情報用の設定(0)
		{
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			param.Descriptor.ShaderRegister = 0;
			rootParameters.push_back(param);
		}
		//テクスチャ情報用の設定(1)
		{
			//デスクリプタレンジ作成
			D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
			numDescriptors = 1;
			descriptorRange[0].BaseShaderRegister = registerCountPS;
			descriptorRange[0].NumDescriptors = numDescriptors;
			descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			registerCountPS += numDescriptors;
			//ルートパラメータ入力
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			param.DescriptorTable.pDescriptorRanges = descriptorRange;
			param.DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);
			rootParameters.push_back(param);
		}

		//Sampler作成
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
			signatireBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
		assert(SUCCEEDED(hr));

		//InputLayout
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
		inputElementDescs[0].SemanticName = "POSITION";
		inputElementDescs[0].SemanticIndex = 0;
		inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElementDescs[1].SemanticName = "TEXCOORD";
		inputElementDescs[1].SemanticIndex = 0;
		inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
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
		Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = dxCommon->CompileShader(L"Resources/shaders/trail/Trail.VS.hlsl",
			L"vs_6_0");
		assert(vertexShaderBlob != nullptr);

		Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = dxCommon->CompileShader(L"Resources/shaders/trail/Trail.PS.hlsl",
			L"ps_6_0");
		assert(pixelShaderBlob != nullptr);

		//DepthStencilStateの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		std::array<D3D12_GRAPHICS_PIPELINE_STATE_DESC, (int)BlendMode::kMaxBlendModeNum> graphicsPipelineStateDesc{};
		for (int i = 0; i < (int)BlendMode::kMaxBlendModeNum; i++) {
			graphicsPipelineStateDesc[i].pRootSignature = rootSignature_.Get();
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
				IID_PPV_ARGS(&graphicsPipelineState_[i]));
			assert(SUCCEEDED(hr));
		}
	}

}