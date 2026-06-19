#include "SpriteManager.h"
#include "DirectXCommon.h"
#include "MainRender.h"
#include "Logger.h"
#include "Sprite.h"
#include "RandomStringUtil.h"
#include "SceneManager.h"

namespace Norm {

	std::unique_ptr<SpriteManager> SpriteManager::instance_ = nullptr;

	SpriteManager* SpriteManager::GetInstance() {
		if (!instance_) {
			instance_ = std::unique_ptr<SpriteManager>(new SpriteManager());
		}
		return instance_.get();
	}

	void SpriteManager::Initialize() {
		//グラフィックスパイプラインの生成
		GenerateGraphicsPipeline();
	}

	void SpriteManager::Update() {
		//スプライトが一つもセットされていなかったら抜ける
		if (sprites_.empty()) return;

		//スプライトの更新
		for (const auto& sprite : sprites_) {
			sprite.second->Update();
		}
	}

	void SpriteManager::BackDraw() {
		//スプライトが一つもセットされていなかったら抜ける
		if (sprites_.empty()) return;

		auto mainRender = MainRender::GetInstance();

		//ルートシグネチャをセットするコマンド
		mainRender->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
		//グラフィックスパイプラインステートをセットするコマンド
		mainRender->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
		//プリミティブトポロジーをセットするコマンド
		mainRender->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//バックスプライト0の描画
		for (const auto& sprite : sprites_) {
			//スプライトのシーンタグが現在のシーンと違うなら次へ
			if (sprite.second->sceneTag_ != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
				//シーンタグがAllでないなら次へ
				if (sprite.second->sceneTag_ != "All") {
					continue;
				}
			}

			//順序選別
			if (sprite.second->order_ == Order::Back0)
				//表示しないなら次へ
				if (sprite.second->isDisplay_)
					//描画
					sprite.second->Draw();
		}
		//バックスプライト1の描画
		for (const auto& sprite : sprites_) {
			//スプライトのシーンタグが現在のシーンと違うなら次へ
			if (sprite.second->sceneTag_ != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
				//シーンタグがAllでないなら次へ
				if (sprite.second->sceneTag_ != "All") {
					continue;
				}
			}

			//順序選別
			if (sprite.second->order_ == Order::Back1)
				//表示しないなら次へ
				if (sprite.second->isDisplay_)
					//描画
					sprite.second->Draw();
		}
		//バックスプライト2の描画
		for (const auto& sprite : sprites_) {
			//スプライトのシーンタグが現在のシーンと違うなら次へ
			if (sprite.second->sceneTag_ != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
				//シーンタグがAllでないなら次へ
				if (sprite.second->sceneTag_ != "All") {
					continue;
				}
			}

			//順序選別
			if (sprite.second->order_ == Order::Back2)
				//表示しないなら次へ
				if (sprite.second->isDisplay_)
					//描画
					sprite.second->Draw();
		}
		//バックスプライト3の描画
		for (const auto& sprite : sprites_) {
			//スプライトのシーンタグが現在のシーンと違うなら次へ
			if (sprite.second->sceneTag_ != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
				//シーンタグがAllでないなら次へ
				if (sprite.second->sceneTag_ != "All") {
					continue;
				}
			}

			//順序選別
			if (sprite.second->order_ == Order::Back3)
				//表示しないなら次へ
				if (sprite.second->isDisplay_)
					//描画
					sprite.second->Draw();
		}
		//バックスプライト4の描画
		for (const auto& sprite : sprites_) {
			//スプライトのシーンタグが現在のシーンと違うなら次へ
			if (sprite.second->sceneTag_ != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
				//シーンタグがAllでないなら次へ
				if (sprite.second->sceneTag_ != "All") {
					continue;
				}
			}

			//順序選別
			if (sprite.second->order_ == Order::Back4)
				//表示しないなら次へ
				if (sprite.second->isDisplay_)
					//描画
					sprite.second->Draw();
		}
		//バックスプライト5の描画
		for (const auto& sprite : sprites_) {
			//スプライトのシーンタグが現在のシーンと違うなら次へ
			if (sprite.second->sceneTag_ != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
				//シーンタグがAllでないなら次へ
				if (sprite.second->sceneTag_ != "All") {
					continue;
				}
			}

			//順序選別
			if (sprite.second->order_ == Order::Back5)
				//表示しないなら次へ
				if (sprite.second->isDisplay_)
					//描画
					sprite.second->Draw();
		}
	}

	void SpriteManager::FrontDraw() {
		//スプライトが一つもセットされていなかったら抜ける
		if (sprites_.empty()) return;

		auto mainRender = MainRender::GetInstance();

		//ルートシグネチャをセットするコマンド
		mainRender->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
		//グラフィックスパイプラインステートをセットするコマンド
		mainRender->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
		//プリミティブトポロジーをセットするコマンド
		mainRender->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//フロントスプライト0の描画
		for (const auto& sprite : sprites_) {
			//スプライトのシーンタグが現在のシーンと違うなら次へ
			if (sprite.second->sceneTag_ != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
				//シーンタグがAllでないなら次へ
				if (sprite.second->sceneTag_ != "All") {
					continue;
				}
			}

			//順序選別
			if (sprite.second->order_ == Order::Front0)
				//表示しないなら次へ
				if (sprite.second->isDisplay_)
					//描画
					sprite.second->Draw();
		}
		//フロントスプライト1の描画
		for (const auto& sprite : sprites_) {
			//スプライトのシーンタグが現在のシーンと違うなら次へ
			if (sprite.second->sceneTag_ != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
				//シーンタグがAllでないなら次へ
				if (sprite.second->sceneTag_ != "All") {
					continue;
				}
			}

			//順序選別
			if (sprite.second->order_ == Order::Front1)
				//表示しないなら次へ
				if (sprite.second->isDisplay_)
					//描画
					sprite.second->Draw();
		}
		//フロントスプライト2の描画
		for (const auto& sprite : sprites_) {
			//スプライトのシーンタグが現在のシーンと違うなら次へ
			if (sprite.second->sceneTag_ != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
				//シーンタグがAllでないなら次へ
				if (sprite.second->sceneTag_ != "All") {
					continue;
				}
			}

			//順序選別
			if (sprite.second->order_ == Order::Front2)
				//表示しないなら次へ
				if (sprite.second->isDisplay_)
					//描画
					sprite.second->Draw();
		}
		//フロントスプライト3の描画
		for (const auto& sprite : sprites_) {
			//スプライトのシーンタグが現在のシーンと違うなら次へ
			if (sprite.second->sceneTag_ != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
				//シーンタグがAllでないなら次へ
				if (sprite.second->sceneTag_ != "All") {
					continue;
				}
			}

			//順序選別
			if (sprite.second->order_ == Order::Front3)
				//表示しないなら次へ
				if (sprite.second->isDisplay_)
					//描画
					sprite.second->Draw();
		}
		//フロントスプライト4の描画
		for (const auto& sprite : sprites_) {
			//スプライトのシーンタグが現在のシーンと違うなら次へ
			if (sprite.second->sceneTag_ != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
				//シーンタグがAllでないなら次へ
				if (sprite.second->sceneTag_ != "All") {
					continue;
				}
			}

			//順序選別
			if (sprite.second->order_ == Order::Front4)
				//表示しないなら次へ
				if (sprite.second->isDisplay_)
					//描画
					sprite.second->Draw();
		}
		//フロントスプライト5の描画
		for (const auto& sprite : sprites_) {
			//スプライトのシーンタグが現在のシーンと違うなら次へ
			if (sprite.second->sceneTag_ != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
				//シーンタグがAllでないなら次へ
				if (sprite.second->sceneTag_ != "All") {
					continue;
				}
			}

			//順序選別
			if (sprite.second->order_ == Order::Front5)
				//表示しないなら次へ
				if (sprite.second->isDisplay_)
					//描画
					sprite.second->Draw();
		}
		//シーン遷移スプライトの描画
		for (const auto& sprite : sprites_) {
			//スプライトのシーンタグが現在のシーンと違うなら次へ
			if (sprite.second->sceneTag_ != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
				//シーンタグがAllでないなら次へ
				if (sprite.second->sceneTag_ != "All") {
					continue;
				}
			}

			//順序選別
			if (sprite.second->order_ == Order::SceneTransition)
				//表示しないなら次へ
				if (sprite.second->isDisplay_)
					//描画
					sprite.second->Draw();
		}
	}

	void SpriteManager::Finalize() {
		//インスタンスを削除
		instance_.reset();
	}

	void SpriteManager::DebugWithImGui() {
#ifdef _DEBUG
		//全コンテナのデバッグ処理
		for (const auto& sprite : sprites_) {
			sprite.second->DebugWithImGui();
		}
#endif // _DEBUG

	}

	void SpriteManager::RegisterSprite(const std::string& name, Sprite* sprite) {
		//重複チェック
		if (sprites_.find(name) != sprites_.end()) {
			return;
		}
		//登録
		sprites_[name] = sprite;
	}

	void SpriteManager::DeleteSprite(const std::string& name) {
		// 名前がコンテナ内に存在するかチェック
		auto it = sprites_.find(name);
		if (it != sprites_.end()) {
			sprites_.erase(it);  // コンテナから削除
		}
	}

	std::string SpriteManager::GenerateName(const std::string& name) {
		// 出力する名前
		std::string outputName = name + "_" + RandomStringUtil::GenerateRandomString(3);

		// 重複チェック用のラムダ式
		std::function<void(const std::string&)> checkDuplicate = [&](const std::string& name) {
			// 重複しているかチェック
			if (sprites_.find(name) != sprites_.end()) {
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

	void SpriteManager::GenerateGraphicsPipeline() {
		HRESULT hr;

		//RootSignature作成
		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
		descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		//DescriptorRange作成
		D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
		descriptorRange[0].BaseShaderRegister = 0;
		descriptorRange[0].NumDescriptors = 1;
		descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		//RootParameter作成。複数設定できるので配列。今回は結果1つだけなので長さ1の配列
		D3D12_ROOT_PARAMETER rootParameters[3] = {};
		//マテリアルの設定
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
		rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
		//オブジェクト関連の設定
		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
		rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
		//テクスチャの設定
		rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Tableを使う
		rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
		rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
		rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

		//Samplerの設定
		D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
		staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
		staticSamplers[0].ShaderRegister = 0;
		staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		//Signatureに反映
		descriptionRootSignature.pStaticSamplers = staticSamplers;
		descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);
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

		D3D12_BLEND_DESC blendDesc{};
		blendDesc.AlphaToCoverageEnable = FALSE; // アルファ値を考慮する
		blendDesc.IndependentBlendEnable = FALSE;

		D3D12_RENDER_TARGET_BLEND_DESC rtBlendDesc{};
		rtBlendDesc.BlendEnable = TRUE; // ブレンド有効
		rtBlendDesc.LogicOpEnable = FALSE;
		rtBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		rtBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		rtBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		rtBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		rtBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		rtBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		rtBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		blendDesc.RenderTarget[0] = rtBlendDesc;
		//RasterizerStateの設定
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		//裏面を表示する(モデルとは無関係)
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		//三角形の中を塗りつぶす
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

		//Shaderをコンパイルする
		Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = DirectXCommon::GetInstance()->CompileShader(L"Resources/shaders/sprite/Sprite.VS.hlsl",
			L"vs_6_0");
		assert(vertexShaderBlob != nullptr);

		Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = DirectXCommon::GetInstance()->CompileShader(L"Resources/shaders/sprite/Sprite.PS.hlsl",
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
			IID_PPV_ARGS(&graphicsPipelineState_));
		assert(SUCCEEDED(hr));
	}

}