#include "LineManager.h"
#include "DirectXCommon.h"
#include "MainRender.h"
#include "GPUDescriptorManager.h"
#include "BaseCamera.h"
#include "Logger.h"
#include <cassert>

namespace Norm {

	//ラインの最大数
	const int kMaxLineNum_ = 8192;

	std::unique_ptr<LineManager> LineManager::instance_ = nullptr;

	LineManager* LineManager::GetInstance() {
		if (!instance_) {
			instance_ = std::unique_ptr<LineManager>(new LineManager());
		}
		return instance_.get();
	}

	void LineManager::Initialize() {
		//グラフィックスパイプラインの生成
		GenerateGraphicsPipeline();

		//リソースの作成
		lineResource_ = MakeLineResource();
		//インスタンシング用SRV設定
		SettingSRV();
	}

	void LineManager::Draw() {
		//ラインが一つもセットされていなかったら抜ける
		if (lines_.empty()) {
			return;
		}

		//カメラの有無チェック
		if (!camera_) {
			assert(0 && "カメラがセットされていません。");
		}

		uint32_t instanceNum = 0;

		for (std::list<Line>::iterator lineIterator = lines_.begin(); lineIterator != lines_.end();) {
			if (instanceNum >= kMaxLineNum_) {
				assert(0 && "ラインの描画上限を超えています。");
				break;	//最大インスタンス数に達したらループを抜ける
			}

			//インスタンシング用データを書きこむ
			lineResource_.instancingData[instanceNum].start = (*lineIterator).start;
			lineResource_.instancingData[instanceNum].end = (*lineIterator).end;
			lineResource_.instancingData[instanceNum].color = (*lineIterator).color;

			//次のインスタンスへ
			++instanceNum;
			//次のイテレータに進む
			++lineIterator;
		}
		//ルートシグネチャをセットするコマンド
		MainRender::GetInstance()->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
		//グラフィックスパイプラインステートをセットするコマンド
		MainRender::GetInstance()->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
		//プリミティブトポロジーをセットするコマンド
		MainRender::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		//座標変換行列の場所を設定
		MainRender::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(0, GPUDescriptorManager::GetInstance()->GetGPUDescriptorHandle(lineResource_.srvIndex));
		//カメラCBuffer場所を設定
		MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, camera_->GetViewProjectionConstBuffer()->GetGPUVirtualAddress());
		//頂点バッファービューを設定
		MainRender::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &lineResource_.vertexBufferView);
		//描画
		if (instanceNum > 0) {
			MainRender::GetInstance()->GetCommandList()->DrawInstanced(2, instanceNum, 0, 0);
		}

		//リストのクリア
		lines_.clear();
	}

	void LineManager::Finalize() {
		//インスタンスを削除
		instance_.reset();
	}

	void LineManager::CreateLine(Vector3 start, Vector3 end, Vector4 color) {
		//Lineを作成
		Line newLine;
		newLine.start = { start.x,start.y,start.z,1.0f };
		newLine.end = { end.x,end.y,end.z,1.0f };
		newLine.color = color;

		//コンテナに登録
		lines_.push_back(newLine);
	}

	void LineManager::GenerateGraphicsPipeline() {
		HRESULT hr;

		//RootSignature作成
		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
		descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		//DescriptorRange作成
		D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
		descriptorRangeForInstancing[0].BaseShaderRegister = 0;
		descriptorRangeForInstancing[0].NumDescriptors = 1;
		descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		//RootParameter作成。複数設定できるので配列。今回は結果1つだけなので長さ1の配列
		D3D12_ROOT_PARAMETER rootParameters[2] = {};
		//座標データの設定
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Tableを使う
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
		rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
		rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);
		//カメラデータの設定
		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
		rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド

		//Signatureに反映
		descriptionRootSignature.pParameters = rootParameters;//ルートパラメータ配列へのポインタ
		descriptionRootSignature.NumParameters = _countof(rootParameters);//配列の長さ

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
			signatireBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
		assert(SUCCEEDED(hr));

		//InputLayout
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
		inputElementDescs[0].SemanticName = "POSITION";
		inputElementDescs[0].SemanticIndex = 0;
		inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElementDescs[1].SemanticName = "VERTEXINDEX";
		inputElementDescs[1].SemanticIndex = 0;
		inputElementDescs[1].Format = DXGI_FORMAT_R32_FLOAT;
		inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = inputElementDescs;
		inputLayoutDesc.NumElements = _countof(inputElementDescs);

		//BlendStateの設定
		D3D12_BLEND_DESC blendDesc{};
		//全ての色要素を書き込む（αブレンドは必要なし）
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[0].BlendEnable = FALSE;

		//RasterizerStateの設定
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		//裏面を表示しない
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		//線の間を塗りつぶす（ワイヤーフレーム）
		rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;

		//Shaderをコンパイルする
		Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = DirectXCommon::GetInstance()->CompileShader(L"Resources/shaders/line/Line.VS.hlsl",
			L"vs_6_0");
		assert(vertexShaderBlob != nullptr);
		Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = DirectXCommon::GetInstance()->CompileShader(L"Resources/shaders/line/Line.PS.hlsl",
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

		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();
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
		//利用するトポロジのタイプ。線
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		//どのように画面に色を打ち込むかの設定
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//DepthStencilの設定
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		//実際に生成
		hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
			IID_PPV_ARGS(&graphicsPipelineState_));
		assert(SUCCEEDED(hr));
	}

	LineManager::LineResource LineManager::MakeLineResource() {
		LineResource result;

		DirectXCommon* dxCommon = DirectXCommon::GetInstance();
		GPUDescriptorManager* gpuDescriptorManager = GPUDescriptorManager::GetInstance();

		//頂点
		{
			//頂点用Resourceを確保
			result.vertexResource = dxCommon->CreateBufferResource(sizeof(VertexForGPU) * 2);
			//頂点バッファービューを作成
			result.vertexBufferView.BufferLocation = result.vertexResource->GetGPUVirtualAddress();
			result.vertexBufferView.SizeInBytes = UINT(sizeof(VertexForGPU) * 2);
			result.vertexBufferView.StrideInBytes = sizeof(VertexForGPU);
			//マッピング
			result.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&result.vertexData));
			//データ入力
			result.vertexData[0].position = { 0.0f,0.0f,0.0f };
			result.vertexData[1].position = { 1.0f,0.0f,0.0f };
			result.vertexData[0].vertexIndex = 0.0f;
			result.vertexData[1].vertexIndex = 1.0f;
		}
		//インスタンシング
		{
			//インスタンシング情報用のResourceを確保
			result.instancingResource = dxCommon->CreateBufferResource(sizeof(LineForGPU) * kMaxLineNum_);
			//マッピング
			result.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&result.instancingData));
			//インスタンシング用のSRVを作成
			result.srvIndex = gpuDescriptorManager->Allocate();
			gpuDescriptorManager->CreateSRVforStructuredBuffer(
				result.srvIndex,
				result.instancingResource.Get(),
				kMaxLineNum_,
				sizeof(LineForGPU)
			);
		}

		return result;
	}

	void LineManager::SettingSRV() {
		//SRVマネージャーからデスクリプタヒープの空き番号を取得
		lineResource_.srvIndex = GPUDescriptorManager::GetInstance()->Allocate();

		//srv設定
		GPUDescriptorManager::GetInstance()->CreateSRVforStructuredBuffer(lineResource_.srvIndex, lineResource_.instancingResource.Get(), kMaxLineNum_, sizeof(LineForGPU));
	}

}