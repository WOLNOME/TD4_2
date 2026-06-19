#include "Sprite.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "MainRender.h"
#include "TextureManager.h"
#include "TextTextureManager.h"
#include "ImGuiManager.h"
#include "SpriteManager.h"
#include "SceneManager.h"

namespace Norm {

	Sprite::Sprite(bool _isNotUsingScene) {
		//シーンタグを設定
		if (_isNotUsingScene) {
			sceneTag_ = "All";
		}
		else {
			sceneTag_ = SceneManager::GetInstance()->GetCurrentScene()->GetSceneName();
		}

		//リソースの作成
		resource_ = CreateResourceForGPU();

	}

	Sprite::~Sprite() {
		//マネージャーから削除
		SpriteManager::GetInstance()->DeleteSprite(name_);
	}

	void Sprite::Initialize(SpriteTag, const std::string& _name, const Order& _order, uint32_t _textureHandle) {
		//名前
		name_ = _name;

		//描画順タグ
		order_ = _order;

		//テクスチャ
		textureHandle_ = _textureHandle;
		textureKind_ = TextureKind::Normal;
		AdjustTextureSize();

		//マネージャーに登録
		SpriteManager::GetInstance()->RegisterSprite(name_, this);
	}

	void Sprite::Initialize(UVScrollTag, const std::string& _name, const Order& _order, int _sheetsNum, float _switchTime, bool _isLoop, uint32_t _textureHandle) {
		//名前
		name_ = _name;

		//描画順タグ
		order_ = _order;

		//UVスクロールデータ
		uvScrollData_.sheetsNum = _sheetsNum;
		uvScrollData_.currentSheetNum = 0;
		uvScrollData_.switchTime = _switchTime;
		uvScrollData_.switchTimer = 0.0f;
		uvScrollData_.isLoop = _isLoop;
		uvScrollData_.isFinished = false;
		//任意のタイミングで開始する
		uvScrollData_.isPlay = false;

		//テクスチャ
		textureHandle_ = _textureHandle;
		textureKind_ = TextureKind::UVScroll;
		AdjustTextureSize();

		//マネージャーに登録
		SpriteManager::GetInstance()->RegisterSprite(name_, this);
	}

	void Sprite::Initialize(TextTag, const std::string& _name, const Order& _order) {
		//名前
		name_ = _name;

		//リソースの作成
		resource_ = CreateResourceForGPU();

		//描画順タグ
		order_ = _order;

		//テクスチャ
		textureKind_ = TextureKind::Text;

		//マネージャーに登録
		SpriteManager::GetInstance()->RegisterSprite(name_, this);
	}

	void Sprite::Update() {
		//表示しないなら処理しない
		if (!isDisplay_) {
			return;
		}

		//揺れの処理
		position += shakeOffset_;

		//UVスクロールの切り替え処理
		if (!uvScrollData_.isFinished && uvScrollData_.isPlay) {
			//タイマーカウント
			uvScrollData_.switchTimer += kDeltaTime;

			//規定時間に達したら
			if (uvScrollData_.switchTimer >= uvScrollData_.switchTime) {
				//タイマーリセット
				uvScrollData_.switchTimer = 0.0f;
				//カウントを進める
				uvScrollData_.currentSheetNum++;
				//カウントが合計枚数を超えたら
				if (uvScrollData_.currentSheetNum >= uvScrollData_.sheetsNum) {
					//もしループするなら
					if (uvScrollData_.isLoop) {
						//カウントリセット
						uvScrollData_.currentSheetNum = 0;
					}
					//ループしないのであれば
					else {
						//カウントを維持
						uvScrollData_.currentSheetNum = uvScrollData_.sheetsNum - 1;
						//終了通知
						uvScrollData_.isFinished = true;
					}
				}
			}
		}

		//Text用ならサイズを常にテクスチャに合わせておく
		if (textureKind_ == Sprite::TextureKind::Text) {
			AdjustTextureSize();
		}

		//トランスフォームの情報を作る
		TransformEuler transform;
		transform.translate = { position.x,position.y,0.0f };
		transform.rotate = { 0.0f,0.0f,rotation };
		transform.scale = { size.x,size.y,1.0f };

		float left = 0.0f - anchorPoint.x;
		float right = 1.0f - anchorPoint.x;
		float top = 0.0f - anchorPoint.y;
		float bottom = 1.0f - anchorPoint.y;

		//左右反転
		if (isFlipX_) {
			left = -left;
			right = -right;
		}
		//上下反転
		if (isFlipY_) {
			top = -top;
			bottom = -bottom;
		}

		//テクスチャのサイズからtexcoordを計算
		float tex_left = 0.0f, tex_right = 0.0f, tex_top = 0.0f, tex_bottom = 0.0f;
		switch (textureKind_) {
		case Sprite::TextureKind::Normal: {
			//指定したテクスチャ番号のメタデータを取得
			const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureHandle_);
			tex_left = textureLeftTop.x / metadata.width;
			tex_right = (textureLeftTop.x + textureSize.x) / metadata.width;
			tex_top = textureLeftTop.y / metadata.height;
			tex_bottom = (textureLeftTop.y + textureSize.y) / metadata.height;
			break;
		}
		case Sprite::TextureKind::UVScroll: {
			//指定したテクスチャ番号のメタデータを取得
			const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureHandle_);
			//現在のシート数に応じたオフセット
			float texOffsetX = textureSize.x * uvScrollData_.currentSheetNum;

			tex_left = (textureLeftTop.x + texOffsetX) / metadata.width;
			tex_right = (textureLeftTop.x + texOffsetX + textureSize.x) / metadata.width;
			tex_top = textureLeftTop.y / metadata.height;
			tex_bottom = (textureLeftTop.y + textureSize.y) / metadata.height;
			break;
		}
		case Sprite::TextureKind::Text: {
			tex_left = textureLeftTop.x / TextTextureManager::GetInstance()->GetTextureWidth(textTextureHandle_);
			tex_right = (textureLeftTop.x + textureSize.x) / TextTextureManager::GetInstance()->GetTextureWidth(textTextureHandle_);
			tex_top = textureLeftTop.y / TextTextureManager::GetInstance()->GetTextureHeight(textTextureHandle_);
			tex_bottom = (textureLeftTop.y + textureSize.y) / TextTextureManager::GetInstance()->GetTextureHeight(textTextureHandle_);
			break;
		}
		default:
			break;
		}

		//データ書き換え処理
		resource_.vertexData[0].position = { left,bottom,0.0f,1.0f };
		resource_.vertexData[1].position = { left,top,0.0f,1.0f };
		resource_.vertexData[2].position = { right,bottom,0.0f,1.0f };
		resource_.vertexData[3].position = { right,top,0.0f,1.0f };

		resource_.vertexData[0].texcoord = { tex_left,tex_bottom };
		resource_.vertexData[1].texcoord = { tex_left,tex_top };
		resource_.vertexData[2].texcoord = { tex_right,tex_bottom };
		resource_.vertexData[3].texcoord = { tex_right,tex_top };

		//レンダリングパイプライン
		Matrix4x4 worldMatrix = MyMath::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
		Matrix4x4 viewMatrix = MyMath::MakeIdentity4x4();
		Matrix4x4 projectionMatrix = MyMath::MakeOrthographicMatrix(0.0f, 0.0f, (float)WinApp::kClientWidth, (float)WinApp::kClientHeight, 0.0f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = MyMath::Multiply(worldMatrix, MyMath::Multiply(viewMatrix, projectionMatrix));
		resource_.transformationMatrixData->WVP = worldViewProjectionMatrix;
		resource_.transformationMatrixData->World = worldMatrix;
	}

	void Sprite::Draw() {
		//非表示状態ならreturn
		if (!isDisplay_) {
			return;
		}

		//頂点バッファービューを設定
		MainRender::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &resource_.vertexBufferView);
		//インデックスバッファービューを設定
		MainRender::GetInstance()->GetCommandList()->IASetIndexBuffer(&resource_.indexBufferView);

		//マテリアルCBufferの場所を設定
		MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, resource_.materialResource->GetGPUVirtualAddress());
		//座標変換行列CBufferの場所を設定
		MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, resource_.transformationMatrixResource->GetGPUVirtualAddress());

		//テクスチャを設定
		switch (textureKind_) {
		case Sprite::TextureKind::Normal:
		case Sprite::TextureKind::UVScroll:
			MainRender::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureHandle_));
			break;
		case Sprite::TextureKind::Text:
			MainRender::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextTextureManager::GetInstance()->GetSrvHandleGPU(textTextureHandle_));
			break;
		default:
			break;
		}

		//描画
		MainRender::GetInstance()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

		//揺らした分元に戻す
		position -= shakeOffset_;
		shakeOffset_ = {};
	}

	void Sprite::DebugWithImGui() {
#ifdef _DEBUG
		ImGui::Begin("スプライト");
		if (ImGui::CollapsingHeader(name_.c_str())) {
			//位置
			ImGui::DragFloat2("位置", &position.x, 0.1f);
			//回転
			ImGui::DragFloat("回転", &rotation, 0.01f);
			//サイズ
			ImGui::DragFloat2("サイズ", &size.x, 0.1f);
			//色
			ImGui::ColorEdit4("色", &resource_.materialData->color.x);
			//アンカーポイント
			ImGui::DragFloat2("アンカーポイント", &anchorPoint.x, 0.01f);
			//左右反転
			ImGui::Checkbox("左右反転", &isFlipX_);
			//上下反転
			ImGui::Checkbox("上下反転", &isFlipY_);
			//テクスチャ左上
			ImGui::DragFloat2("テクスチャ左上", &textureLeftTop.x, 0.1f);
			//テクスチャサイズ
			ImGui::DragFloat2("テクスチャサイズ", &textureSize.x, 0.1f);
		}
		ImGui::End();
#endif // _DEBUG

	}

	void Sprite::SetTexture(uint32_t _textureHandle) {
		textureHandle_ = _textureHandle;
		AdjustTextureSize();
	}

	Sprite::ResourceForGPU Sprite::CreateResourceForGPU() {
		ResourceForGPU result;

		//リソースを作る
		result.vertexResource = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(VertexData) * 4);
		result.indexResource = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(uint32_t) * 6);
		result.materialResource = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(Material));
		result.transformationMatrixResource = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(TransformationMatrix));

		//バッファービューを作る
		result.vertexBufferView.BufferLocation = result.vertexResource->GetGPUVirtualAddress();
		result.vertexBufferView.SizeInBytes = sizeof(VertexData) * 4;
		result.vertexBufferView.StrideInBytes = sizeof(VertexData);
		result.indexBufferView.BufferLocation = result.indexResource->GetGPUVirtualAddress();
		result.indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
		result.indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		//リソースにデータをセット
		result.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&result.vertexData));
		result.indexResource->Map(0, nullptr, reinterpret_cast<void**>(&result.indexData));
		result.materialResource->Map(0, nullptr, reinterpret_cast<void**>(&result.materialData));
		result.transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&result.transformationMatrixData));
		///データに書き込む
		//頂点データ
		//左下
		result.vertexData[0].position = { 0.0f,1.0f,0.0f,1.0f };
		result.vertexData[0].texcoord = { 0.0f,1.0f };
		//左上
		result.vertexData[1].position = { 0.0f,0.0f,0.0f,1.0f };
		result.vertexData[1].texcoord = { 0.0f,0.0f };
		//右下
		result.vertexData[2].position = { 1.0f,1.0f,0.0f,1.0f };
		result.vertexData[2].texcoord = { 1.0f,1.0f };
		//右上
		result.vertexData[3].position = { 1.0f,0.0f,0.0f,1.0f };
		result.vertexData[3].texcoord = { 1.0f,0.0f };
		for (UINT i = 0; i < 4; i++) {
			result.vertexData[i].normal = { 0.0f,0.0f,-1.0f };
		}
		//インデックスデータ(当てられてる数字はVertexDataの要素)
		result.indexData[0] = 0; result.indexData[1] = 1; result.indexData[2] = 2;
		result.indexData[3] = 1; result.indexData[4] = 3; result.indexData[5] = 2;
		//マテリアルデータ
		result.materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		result.materialData->uvTransform = MyMath::MakeIdentity4x4();
		result.materialData->isTexture = true;
		//座標変換行列データ
		result.transformationMatrixData->WVP = MyMath::MakeIdentity4x4();
		result.transformationMatrixData->World = MyMath::MakeIdentity4x4();

		return result;
	}

	void Sprite::AdjustTextureSize() {
		switch (textureKind_) {
		case Sprite::TextureKind::Normal: {
			//テクスチャメタデータを取得
			const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureHandle_);

			textureSize.x = static_cast<float>(metadata.width);
			textureSize.y = static_cast<float>(metadata.height);
			//画像サイズをテクスチャサイズに合わせる
			size = textureSize;
			break;
		}
		case Sprite::TextureKind::UVScroll: {
			//テクスチャメタデータを取得
			const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureHandle_);

			textureSize.x = static_cast<float>(metadata.width) / uvScrollData_.sheetsNum;
			textureSize.y = static_cast<float>(metadata.height);

			//画像サイズをテクスチャサイズに合わせる
			size = textureSize;
			break;
		}
		case Sprite::TextureKind::Text: {
			textureSize.x = (float)TextTextureManager::GetInstance()->GetTextureWidth(textTextureHandle_);
			textureSize.y = (float)TextTextureManager::GetInstance()->GetTextureHeight(textTextureHandle_);
			//画像サイズをテクスチャサイズに合わせる
			size = textureSize;
			break;
		}
		default:
			break;
		}
	}

}