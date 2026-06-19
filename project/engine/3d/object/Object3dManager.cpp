#include "Object3dManager.h"
#include "DirectXCommon.h"
#include "MainRender.h"
#include "SceneLight.h"
#include "SceneManager.h"
#include "Logger.h"
#include "RandomStringUtil.h"
#include "Object3d.h"
#include <cassert>

namespace Norm {

	std::unique_ptr<Object3dManager> Object3dManager::instance_ = nullptr;

	Object3dManager* Object3dManager::GetInstance() {
		if (!instance_) {
			instance_ = std::unique_ptr<Object3dManager>(new Object3dManager());
		}
		return instance_.get();
	}

	void Object3dManager::Initialize() {
		//グラフィックスパイプラインの生成
		GenerateGraphicsPipeline();
		//コンピュートパイプラインの生成
		GenerateComputePipeline();
	}

	void Object3dManager::Update() {
		//オブジェクトが一つもセットされていなかったら抜ける
		if (objects_.empty()) return;

		//全オブジェクトの更新
		for (const auto& object : objects_) {
			object.second->Update();
		}
	}

	void Object3dManager::Draw() {
		//オブジェクトが一つもセットされていなかったら抜ける
		if (objects_.empty()) return;

		auto mainRender = MainRender::GetInstance();

		//カメラチェック
		assert(camera_ && "カメラがセットされていません");
		//シーンライトチェック
		assert(light_ && "シーンライトがセットされていません");


		//全オブジェクトの描画（不透明）
		for (const auto& object : objects_) {
			//オブジェクトのシーンタグが現在のシーンタグと違うならスキップ
			if (object.second->sceneTag_ != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
				continue;
			}
			//オブジェクトが非表示ならスキップ
			if (!object.second->isDisplay_) {
				continue;
			}
			//半透明ならスキップ
			if (object.second->GetColor().w < 1.0f) {
				continue;
			}

			object.second->Draw(camera_, light_);
		}

		//全オブジェクトの描画（半透明）
		for (const auto& object : objects_) {
			//オブジェクトのシーンタグが現在のシーンタグと違うならスキップ
			if (object.second->sceneTag_ != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
				continue;
			}
			//オブジェクトが非表示ならスキップ
			if (!object.second->isDisplay_) {
				continue;
			}
			//不透明ならスキップ
			if (object.second->GetColor().w == 1.0f) {
				continue;
			}

			object.second->Draw(camera_, light_);
		}

	}

	void Object3dManager::Finalize() {
		//インスタンスを削除
		instance_.reset();
	}

	void Object3dManager::RegisterObject(const std::string& name, Object3d* object) {
		//重複チェック
		if (objects_.find(name) != objects_.end()) {
			return;
		}
		//登録
		objects_[name] = object;
	}

	void Object3dManager::DeleteObject(const std::string& name) {
		//名前がコンテナ内に存在するかチェック
		auto it = objects_.find(name);
		if (it != objects_.end()) {
			objects_.erase(it);  //コンテナから削除
		}
	}

	std::string Object3dManager::GenerateName(const std::string& name) {
		//出力する名前
		std::string outputName = name + "_" + RandomStringUtil::GenerateRandomString(3);

		//重複チェック用のラムダ式
		std::function<void(const std::string&)> checkDuplicate = [&](const std::string& name) {
			//重複しているかチェック
			if (objects_.find(name) != objects_.end()) {
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

	void Object3dManager::SettingCommonDrawing(NameGPS index) {
		//ルートシグネチャをセットするコマンド
		MainRender::GetInstance()->GetCommandList()->SetGraphicsRootSignature(rootSignature_[(int)index].Get());
		//グラフィックスパイプラインステートをセットするコマンド
		MainRender::GetInstance()->GetCommandList()->SetPipelineState(graphicsPipelineState_[(int)index].Get());
		//プリミティブトポロジーをセットするコマンド
		MainRender::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void Object3dManager::SettingAnimationCS() {
		//コンピュートルートシグネチャ
		MainRender::GetInstance()->GetCommandList()->SetComputeRootSignature(computeRootSignature_.Get());
		//コンピュートパイプライン
		MainRender::GetInstance()->GetCommandList()->SetPipelineState(computePipelineState_.Get());
	}

	void Object3dManager::GenerateGraphicsPipeline() {
		//通常PSOの設定
		NormalPSOOption();
		//スカイボックス用PSOの設定
		SkyBoxPSOOption();
	}

	void Object3dManager::GenerateComputePipeline() {
		HRESULT hr;
		//RootSignature作成
		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
		descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		//レジスタカウント
		int registerCountT = 0;		//structuredBuffer専用
		int registerCountU = 0;		//RWStructuredBuffer専用
		//使用するデスクリプタの数
		int numDescriptors = 0;

		//MatrixPalette用DescriptorRange作成
		D3D12_DESCRIPTOR_RANGE matrixPaletteDescriptorRange[1] = {};
		numDescriptors = 1;
		matrixPaletteDescriptorRange[0].BaseShaderRegister = registerCountT;
		matrixPaletteDescriptorRange[0].NumDescriptors = numDescriptors;
		matrixPaletteDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		matrixPaletteDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountT += numDescriptors;
		//入力頂点用DescriptorRange作成
		D3D12_DESCRIPTOR_RANGE inputVerticesDescriptorRange[1] = {};
		numDescriptors = 1;
		inputVerticesDescriptorRange[0].BaseShaderRegister = registerCountT;
		inputVerticesDescriptorRange[0].NumDescriptors = numDescriptors;
		inputVerticesDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		inputVerticesDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountT += numDescriptors;
		//入力インフルエンス用DescriptorRange作成
		D3D12_DESCRIPTOR_RANGE inputInfluenceDescriptorRange[1] = {};
		numDescriptors = 1;
		inputInfluenceDescriptorRange[0].BaseShaderRegister = registerCountT;
		inputInfluenceDescriptorRange[0].NumDescriptors = numDescriptors;
		inputInfluenceDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		inputInfluenceDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountT += numDescriptors;
		//出力頂点用DescriptorRange作成
		D3D12_DESCRIPTOR_RANGE outputVerticesDescriptorRange[1] = {};
		numDescriptors = 1;
		outputVerticesDescriptorRange[0].BaseShaderRegister = registerCountU;
		outputVerticesDescriptorRange[0].NumDescriptors = numDescriptors;
		outputVerticesDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		outputVerticesDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountU += numDescriptors;

		std::vector<D3D12_ROOT_PARAMETER> rootParameters;
		//MatirixPalette用の設定(0)
		D3D12_ROOT_PARAMETER matrixPaletteParam = {};
		matrixPaletteParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		matrixPaletteParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		matrixPaletteParam.DescriptorTable.pDescriptorRanges = matrixPaletteDescriptorRange;
		matrixPaletteParam.DescriptorTable.NumDescriptorRanges = _countof(matrixPaletteDescriptorRange);
		rootParameters.push_back(matrixPaletteParam);

		//入力頂点用の設定(1)
		D3D12_ROOT_PARAMETER inputVerticesParam = {};
		inputVerticesParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		inputVerticesParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		inputVerticesParam.DescriptorTable.pDescriptorRanges = inputVerticesDescriptorRange;
		inputVerticesParam.DescriptorTable.NumDescriptorRanges = _countof(inputVerticesDescriptorRange);
		rootParameters.push_back(inputVerticesParam);

		//入力インフルエンス関連の設定(2)
		D3D12_ROOT_PARAMETER inputInfluenceParam = {};
		inputInfluenceParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		inputInfluenceParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		inputInfluenceParam.DescriptorTable.pDescriptorRanges = inputInfluenceDescriptorRange;
		inputInfluenceParam.DescriptorTable.NumDescriptorRanges = _countof(inputInfluenceDescriptorRange);
		rootParameters.push_back(inputInfluenceParam);

		//出力頂点用の設定(3)
		D3D12_ROOT_PARAMETER outputVerticesParam = {};
		outputVerticesParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		outputVerticesParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		outputVerticesParam.DescriptorTable.pDescriptorRanges = outputVerticesDescriptorRange;
		outputVerticesParam.DescriptorTable.NumDescriptorRanges = _countof(outputVerticesDescriptorRange);
		rootParameters.push_back(outputVerticesParam);

		//スキニング情報の設定(4)
		D3D12_ROOT_PARAMETER skinningInformationParam = {};
		skinningInformationParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		skinningInformationParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		skinningInformationParam.Descriptor.ShaderRegister = 0;
		rootParameters.push_back(skinningInformationParam);

		//ルートシグネチャの記述
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
			signatireBlob->GetBufferSize(), IID_PPV_ARGS(&computeRootSignature_));
		assert(SUCCEEDED(hr));

		//Shaderをコンパイルする
		Microsoft::WRL::ComPtr<IDxcBlob> computeShaderBlob;
		computeShaderBlob = DirectXCommon::GetInstance()->CompileShader(L"Resources/shaders/object/Skinning.CS.hlsl",
			L"cs_6_0");
		assert(computeShaderBlob != nullptr);

		//コンピュートシェーダー用のPSOの設定
		D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
		computePipelineStateDesc.CS = {
			.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
			.BytecodeLength = computeShaderBlob->GetBufferSize()
		};
		computePipelineStateDesc.pRootSignature = computeRootSignature_.Get();
		hr = DirectXCommon::GetInstance()->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&computePipelineState_));
	}

	void Object3dManager::NormalPSOOption() {
		HRESULT hr;

		//RootSignature作成
		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
		descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		//レジスタカウント
		int registerCountVS = 0;
		int registerCountPS = 0;
		//使用するデスクリプタの数
		int numDescriptors = 0;

		//DescriptorRange設定
		D3D12_DESCRIPTOR_RANGE descriptorRanges[3] = {};
		//トランスフォームの設定
		numDescriptors = 1;
		descriptorRanges[0].BaseShaderRegister = registerCountVS;
		descriptorRanges[0].NumDescriptors = numDescriptors;
		descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountVS += numDescriptors;
		//オブジェクトのテクスチャの設定
		numDescriptors = 1;
		descriptorRanges[1].BaseShaderRegister = registerCountPS;
		descriptorRanges[1].NumDescriptors = numDescriptors;
		descriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountPS += numDescriptors;
		//環境光テクスチャ用
		numDescriptors = 1;
		descriptorRanges[2].BaseShaderRegister = registerCountPS;
		descriptorRanges[2].NumDescriptors = numDescriptors;
		descriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountPS += numDescriptors;

		//RootParameter作成
		std::vector<D3D12_ROOT_PARAMETER> rootParameters;

		//マテリアルの設定(0)
		{
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			param.Descriptor.ShaderRegister = 0;
			rootParameters.push_back(param);
		}
		//ワールドトランスフォーム情報用の設定(1)
		{
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[0];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//カメラ情報用の設定(2)
		{
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			param.Descriptor.ShaderRegister = 0;
			rootParameters.push_back(param);
		}
		//テクスチャ情報用の設定(3)
		{
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[1];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//カメラ座標用定数バッファ情報用の設定(4)
		{
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			param.Descriptor.ShaderRegister = 1;
			rootParameters.push_back(param);
		}
		//シーンライト情報用用の設定(5)
		{
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			param.Descriptor.ShaderRegister = 2;
			rootParameters.push_back(param);
		}
		//光源有無情報用の設定(6)
		{
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			param.Descriptor.ShaderRegister = 3;
			rootParameters.push_back(param);
		}
		//環境マップテクスチャ情報用の設定(7)
		{
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[2];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}

		//Samplerの設定
		std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers(1);
		staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
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
		hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatireBlob->GetBufferPointer(),
			signatireBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_[(int)NameGPS::Normal]));
		assert(SUCCEEDED(hr));

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDesc;

		//宣言
		D3D12_INPUT_ELEMENT_DESC ied0 = {};
		D3D12_INPUT_ELEMENT_DESC ied1 = {};
		D3D12_INPUT_ELEMENT_DESC ied2 = {};
		//定義
		ied0.SemanticName = "POSITION";
		ied0.SemanticIndex = 0;
		ied0.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		ied0.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		ied1.SemanticName = "TEXCOORD";
		ied1.SemanticIndex = 0;
		ied1.Format = DXGI_FORMAT_R32G32_FLOAT;
		ied1.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		ied2.SemanticName = "NORMAL";
		ied2.SemanticIndex = 0;
		ied2.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		ied2.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		//登録
		inputElementDesc.push_back(ied0);
		inputElementDesc.push_back(ied1);
		inputElementDesc.push_back(ied2);

		//インプットレイアウトディスクに登録
		inputLayoutDesc.pInputElementDescs = inputElementDesc.data();
		inputLayoutDesc.NumElements = static_cast<UINT>(inputElementDesc.size());

		//BlendStateの設定
		D3D12_BLEND_DESC blendDesc{};
		//全ての色要素を書き込む
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

		//RasterizerStateの設定
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		//裏面を表示しない
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		//三角形の中を塗りつぶす
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

		//Shaderをコンパイルする
		Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob;
		vertexShaderBlob = DirectXCommon::GetInstance()->CompileShader(L"Resources/shaders/object/Object3d.VS.hlsl",
			L"vs_6_0");
		assert(vertexShaderBlob != nullptr);
		Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = DirectXCommon::GetInstance()->CompileShader(L"Resources/shaders/object/Object3d.PS.hlsl",
			L"ps_6_0");
		assert(pixelShaderBlob != nullptr);

		//DepthStencilStateの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		//Depthの機能を有効化する
		depthStencilDesc.DepthEnable = true;
		//書き込みします
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		//比較関数はLessEqual。つまり、近ければ描画される
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		//PSO情報を書き込む
		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = rootSignature_[(int)NameGPS::Normal].Get();
		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
		graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize() };
		graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize() };
		graphicsPipelineStateDesc.BlendState = blendDesc;
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
		//書き込むRTVの情報
		graphicsPipelineStateDesc.NumRenderTargets = 1;
		graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		//利用するトポロジのタイプ。三角形
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		//どのように画面に色を打ち込むかの設定
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//DepthStencilの設定
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		//実際に生成
		hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
			IID_PPV_ARGS(&graphicsPipelineState_[(int)NameGPS::Normal]));
		assert(SUCCEEDED(hr));
	}

	void Object3dManager::SkyBoxPSOOption() {
		HRESULT hr;
		//RootSignature作成
		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
		descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		//レジスタカウント
		int registerCountVS = 0;
		int registerCountPS = 0;
		//使用するデスクリプタの数
		int numDescriptors = 0;

		//DescriptorRange設定
		D3D12_DESCRIPTOR_RANGE descriptorRanges[2] = {};
		//トランスフォームの設定
		numDescriptors = 1;
		descriptorRanges[0].BaseShaderRegister = registerCountVS;
		descriptorRanges[0].NumDescriptors = numDescriptors;
		descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountVS += numDescriptors;
		//オブジェクトのテクスチャの設定
		numDescriptors = 1;
		descriptorRanges[1].BaseShaderRegister = registerCountPS;
		descriptorRanges[1].NumDescriptors = numDescriptors;
		descriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		registerCountPS += numDescriptors;

		//RootParameter作成
		std::vector<D3D12_ROOT_PARAMETER> rootParameters;

		//マテリアルの設定(0)
		{
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			param.Descriptor.ShaderRegister = 0;
			rootParameters.push_back(param);
		}
		//ワールドトランスフォーム情報用の設定(1)
		{
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[0];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//カメラ情報用の設定(2)
		{
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			param.Descriptor.ShaderRegister = 0;
			rootParameters.push_back(param);
		}
		//テクスチャ情報用の設定(3)
		{
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			param.DescriptorTable.pDescriptorRanges = &descriptorRanges[1];
			param.DescriptorTable.NumDescriptorRanges = 1;
			rootParameters.push_back(param);
		}
		//カメラ座標用定数バッファ情報用の設定(4)
		{
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			param.Descriptor.ShaderRegister = 1;
			rootParameters.push_back(param);
		}
		//シーンライト情報用用の設定(5)
		{
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			param.Descriptor.ShaderRegister = 2;
			rootParameters.push_back(param);
		}
		//光源有無情報用の設定(6)
		{
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			param.Descriptor.ShaderRegister = 3;
			rootParameters.push_back(param);
		}

		//Samplerの設定
		std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers(1);
		staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
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
		hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatireBlob->GetBufferPointer(),
			signatireBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_[(int)NameGPS::SkyBox]));
		assert(SUCCEEDED(hr));

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDesc;

		//宣言
		D3D12_INPUT_ELEMENT_DESC ied0 = {};
		D3D12_INPUT_ELEMENT_DESC ied1 = {};
		D3D12_INPUT_ELEMENT_DESC ied2 = {};
		//定義
		ied0.SemanticName = "POSITION";
		ied0.SemanticIndex = 0;
		ied0.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		ied0.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		ied1.SemanticName = "TEXCOORD";
		ied1.SemanticIndex = 0;
		ied1.Format = DXGI_FORMAT_R32G32_FLOAT;
		ied1.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		ied2.SemanticName = "NORMAL";
		ied2.SemanticIndex = 0;
		ied2.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		ied2.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		//登録
		inputElementDesc.push_back(ied0);
		inputElementDesc.push_back(ied1);
		inputElementDesc.push_back(ied2);

		//インプットレイアウトディスクに登録
		inputLayoutDesc.pInputElementDescs = inputElementDesc.data();
		inputLayoutDesc.NumElements = static_cast<UINT>(inputElementDesc.size());

		//BlendStateの設定
		D3D12_BLEND_DESC blendDesc{};
		//全ての色要素を書き込む
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

		//RasterizerStateの設定
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		//裏面を表示しない
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		//三角形の中を塗りつぶす
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

		//Shaderをコンパイルする
		Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob;
		vertexShaderBlob = DirectXCommon::GetInstance()->CompileShader(L"Resources/shaders/object/Skybox.VS.hlsl",
			L"vs_6_0");
		assert(vertexShaderBlob != nullptr);
		Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = DirectXCommon::GetInstance()->CompileShader(L"Resources/shaders/object/Skybox.PS.hlsl",
			L"ps_6_0");
		assert(pixelShaderBlob != nullptr);

		//DepthStencilStateの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		//Depthの機能を有効化する
		depthStencilDesc.DepthEnable = true;
		//書き込みします
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		//比較関数はLessEqual。つまり、近ければ描画される
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		//PSO情報を書き込む
		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = rootSignature_[(int)NameGPS::SkyBox].Get();
		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
		graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize() };
		graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize() };
		graphicsPipelineStateDesc.BlendState = blendDesc;
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
		//書き込むRTVの情報
		graphicsPipelineStateDesc.NumRenderTargets = 1;
		graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		//利用するトポロジのタイプ。三角形
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		//どのように画面に色を打ち込むかの設定
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//DepthStencilの設定
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		//実際に生成
		hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
			IID_PPV_ARGS(&graphicsPipelineState_[(int)NameGPS::SkyBox]));
		assert(SUCCEEDED(hr));
	}

}