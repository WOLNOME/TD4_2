#include "BulletTrail.h"
#include "BulletTrailManager.h"
#include "DirectXCommon.h"
#include "MainRender.h"
#include "GPUDescriptorManager.h"
#include "BaseCamera.h"
#include "SceneManager.h"

namespace Norm {

	BulletTrail::BulletTrail() {
		//シーンタグの初期化
		sceneTag_ = SceneManager::GetInstance()->GetCurrentScene()->GetSceneName();
	}

	BulletTrail::~BulletTrail() {
		//マネージャーから削除
		BulletTrailManager::GetInstance()->DeleteBulletTrail(name_);
	}

	void BulletTrail::Initialize(const std::string& name, float maxVerLength, float lengthDecayValue) {
		//名前
		name_ = name;
		//パラメーター
		verLength_ = maxVerLength;
		lengthDecayValue_ = lengthDecayValue;

		//リソース作成
		resource_ = CreateBulletTrailResource();
		//マネージャーに登録
		BulletTrailManager::GetInstance()->RegisterBulletTrail(name_, this);
	}

	void BulletTrail::ClearPositions() {
		positions_.clear();
	}

	void BulletTrail::Update() {
		// 頂点・インデックスバッファをクリア
		memset(resource_.vertexData, 0, sizeof(VertexData) * kMaxVertexNum_);
		memset(resource_.indexData, 0, sizeof(uint32_t) * (((kMaxVertexNum_ / 4) - 1) * 24 + 12));
		indexCount_ = 0;

		// トレール寿命処理
		for (auto it = positions_.begin(); it != positions_.end(); ) {
			it->second++; // カウント進行
			if (verLength_ - it->second * lengthDecayValue_ <= 0.0f) {
				it = positions_.erase(it); // 寿命超えたら削除
			}
			else {
				++it;
			}
		}

		//描画対象が2つ以上なら描画する
		isDisplay_ = (positions_.size() >= 2);

		// 描画しないのであればreturn
		if (!isDisplay_) return;

		// 頂点生成
		std::list<Vector3> vertices;
		Vector3 prevPos;

		bool isFirst = true;
		for (const auto& [pos, count] : positions_) {
			// 向き計算
			Vector3 direction;
			if (isFirst) {
				prevPos = pos;
				isFirst = false;
				continue; // 初回は次の座標が必要なのでスキップ
			}
			direction = (pos - prevPos).Normalized();
			prevPos = pos;

			// 幅計算
			float verLength = verLength_ - count * lengthDecayValue_;
			if (verLength <= 0.0f) continue;

			// 軸計算
			Vector3 worldUp = { 0.0f, 1.0f, 0.0f };
			Vector3 right = MyMath::Cross(worldUp, direction).Normalized();
			Vector3 up = MyMath::Cross(direction, right).Normalized();

			// 頂点追加
			vertices.push_back(pos + right * verLength + up * verLength);  // RT
			vertices.push_back(pos - right * verLength + up * verLength);  // LT
			vertices.push_back(pos + right * verLength - up * verLength);  // RB
			vertices.push_back(pos - right * verLength - up * verLength);  // LB
		}

		// 頂点データの転送
		int vtxIdx = 0;
		for (const auto& vtx : vertices) {
			resource_.vertexData[vtxIdx].position = Vector4(vtx.x, vtx.y, vtx.z, 1.0f);
			resource_.vertexData[vtxIdx].texCoord = Vector2(0.0f, 0.0f); // 必要なら後で調整
			++vtxIdx;
		}

		// インデックス構築
		const int sectionCount = static_cast<int>(vertices.size() / 4 - 1);
		if (sectionCount <= 0) return;

		indexCount_ = sectionCount * 6 * 4 + 6 * 2;
		int idx = 0;
		auto AddTriangle = [&](int a, int b, int c) {
			resource_.indexData[idx++] = a;
			resource_.indexData[idx++] = b;
			resource_.indexData[idx++] = c;
			};

		for (int i = 0; i < sectionCount; ++i) {
			int vi = i * 4;
			int ni = vi + 4;

			// 上面
			AddTriangle(vi, ni, vi + 1);
			AddTriangle(vi + 1, ni, ni + 1);

			// 右面
			AddTriangle(vi, vi + 2, ni);
			AddTriangle(ni, vi + 2, ni + 2);

			// 下面
			AddTriangle(vi + 2, vi + 3, ni + 2);
			AddTriangle(vi + 3, ni + 3, ni + 2);

			// 左面
			AddTriangle(vi + 1, ni + 1, vi + 3);
			AddTriangle(vi + 3, ni + 1, ni + 3);

			// 前キャップ
			if (i == 0) {
				AddTriangle(vi, vi + 1, vi + 2);
				AddTriangle(vi + 2, vi + 1, vi + 3);
			}
			// 後キャップ
			else if (i == sectionCount - 1) {
				AddTriangle(ni, ni + 2, ni + 1);
				AddTriangle(ni + 1, ni + 2, ni + 3);
			}
		}
	}

	void BulletTrail::Draw(BaseCamera* _camera) {
		//頂点バッファビューを送信
		MainRender::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &resource_.vertexBufferView);
		//インデックスバッファビューを送信
		MainRender::GetInstance()->GetCommandList()->IASetIndexBuffer(&resource_.indexBufferView);
		//Cameraの情報をCBufferへ送信
		MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, _camera->GetViewProjectionConstBuffer()->GetGPUVirtualAddress());
		//テクスチャ情報をCBufferへ送信
		MainRender::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(1, GPUDescriptorManager::GetInstance()->GetGPUDescriptorHandle(textureHandle_));

		//ドローコール
		MainRender::GetInstance()->GetCommandList()->DrawIndexedInstanced(indexCount_, 1, 0, 0, 0);

	}

	void BulletTrail::SetPosition(const Vector3& _position) {
		//座標をセット
		positions_.push_back(std::make_pair(_position, 0));
	}

	BulletTrail::BulletTrailResource BulletTrail::CreateBulletTrailResource() {
		BulletTrailResource result;
		//リソースの作成
		result.vertexResource = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(VertexData) * kMaxVertexNum_);
		result.indexResource = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(uint32_t) * (((kMaxVertexNum_ / 4) - 1) * 24 + 12));
		//VBVの作成
		result.vertexBufferView.BufferLocation = result.vertexResource->GetGPUVirtualAddress();
		result.vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * kMaxVertexNum_);
		result.vertexBufferView.StrideInBytes = sizeof(VertexData);
		//IBVの作成
		result.indexBufferView.BufferLocation = result.indexResource->GetGPUVirtualAddress();
		result.indexBufferView.SizeInBytes = UINT(sizeof(uint32_t) * (((kMaxVertexNum_ / 4) - 1) * 24 + 12));
		result.indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		//リソースとデータを同期させる
		result.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&result.vertexData));
		result.indexResource->Map(0, nullptr, reinterpret_cast<void**>(&result.indexData));

		return result;
	}

}