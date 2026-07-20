#include "CollisionManager.h"
#include "AABBColliderBase.h"
#include "CollisionConfig.h"
#include "MyMath.h"
#include "OBBColliderBase.h"
#include "SphereColliderBase.h"
#include <SceneManager.h>

namespace Norm {

using CollisionFunc = bool (*)(ICollider*, ICollider*);

// 衝突関数群
bool SphereXSphere(ICollider* a, ICollider* b);
bool SphereXAABB(ICollider* a, ICollider* b);
bool SphereXOBB(ICollider* a, ICollider* b);
bool AABBXSphere(ICollider* a, ICollider* b);
bool AABBXAABB(ICollider* a, ICollider* b);
bool AABBXOBB(ICollider* a, ICollider* b);
bool OBBXSphere(ICollider* a, ICollider* b);
bool OBBXAABB(ICollider* a, ICollider* b);
bool OBBXOBB(ICollider* a, ICollider* b);

constexpr CollisionFunc kCollisionTable[3][3] = {
    {SphereXSphere, SphereXAABB, SphereXOBB},
    {AABBXSphere,   AABBXAABB,   AABBXOBB  },
    {OBBXSphere,    OBBXAABB,    OBBXOBB   }
};

std::unique_ptr<CollisionManager> CollisionManager::instance_ = nullptr;

CollisionManager* CollisionManager::GetInstance() {
	if (!instance_) {
		instance_ = std::unique_ptr<CollisionManager>(new CollisionManager());
	}
	return instance_.get();
}

void CollisionManager::Initialize() {}

void CollisionManager::Finalize() {
	// インスタンスを削除
	instance_.reset();
}

void CollisionManager::CheckCollision() {
	// 毎フレームグリッドを作る（動くコライダー用に）
	BuildGrid();

	// 衝突フィルタリング + 判定 + コールバック呼び出しをまとめたラムダ式
	auto TryCheckPair = [this](ICollider* colliderA, ICollider* colliderB) {
		CollisionAttribute attrA = colliderA->GetCollisionAttribute();
		CollisionAttribute attrB = colliderB->GetCollisionAttribute();

		// Nothingnessはスキップ
		if (attrA == CollisionAttribute::Nothingness || attrB == CollisionAttribute::Nothingness) {
			return;
		}

		// 同じ属性同士はスキップ
		if (attrA == attrB) {
			return;
		}

		CheckCollisionPair(colliderA, colliderB);
		};

	// グリッドの各セルについて処理する
	for (auto& [cell, listInCell] : grid_) {

		// 同一説内のコライダー同士のペア
		for (size_t i = 0; i < listInCell.size(); ++i) {
			for (size_t j = i + 1; j < listInCell.size(); ++j) {
				TryCheckPair(listInCell[i], listInCell[j]);
			}
		}

		// 隣接26セルとのペア
		// 同じセルペアを2回処理しないよう、cell < neighbor の場合のみ判定する
		for (int32_t dz = -1; dz <= 1; ++dz) {
			for (int32_t dy = -1; dy <= 1; ++dy) {
				for (int32_t dx = -1; dx <= 1; ++dx) {
					if (dx == 0 && dy == 0 && dz == 0) {
						continue; // 自セルは処理済み
					}

					GridCell neighbor{cell.x + dx, cell.y + dy, cell.z + dz};

					// 順序が逆側のペアは、相手セル側の走査に任せてスキップする
					if (!(cell < neighbor)) {
						continue;
					}

					auto it = grid_.find(neighbor);
					if (it == grid_.end()) {
						continue; // 隣にコライダーがいなければ何もしない
					}

					for (ICollider* colliderA : listInCell) {
						for (ICollider* colliderB : it->second) {
							TryCheckPair(colliderA, colliderB);
						}
					}
				}
			}
		}
	}

	////衝突フィルタリングラムダ式
	// auto ShouldIgnore = [](CollisionAttribute a, CollisionAttribute b) {
	//	// Nothingness は誰とも当たらない
	//	if (a == CollisionAttribute::Nothingness ||
	//		b == CollisionAttribute::Nothingness) {
	//		return true;
	//	}

	//	// 同じ属性同士
	//	if (a == b) {
	//		return true;
	//	}

	//	// それ以外は当たる
	//	return false;
	//	};

	//// リスト内のペアを総当たり（重複しない）
	// const size_t size = colliders_.size();
	// for (size_t i = 0; i < size; ++i) {
	//	ICollider* colliderA = colliders_[i];
	//	CollisionAttribute attrA = colliderA->GetCollisionAttribute();

	//	//シーンタグが現在シーンの物ではない場合弾く
	//	if (colliderA->GetSceneTag() != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
	//		continue;
	//	}

	//	for (size_t j = i + 1; j < size; ++j) {
	//		ICollider* colliderB = colliders_[j];
	//		CollisionAttribute attrB = colliderB->GetCollisionAttribute();

	//		//シーンタグが現在シーンの物ではない場合弾く
	//		if (colliderA->GetSceneTag() != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
	//			continue;
	//		}

	//		//当たり判定を行わないペアは弾く
	//		if (ShouldIgnore(attrA, attrB)) {
	//			continue;
	//		}

	//		CheckCollisionPair(colliderA, colliderB);
	//	}
	//}
}

void CollisionManager::RegistCollider(ICollider* _collider) {
	// 登録
	colliders_.push_back(_collider);
}

void CollisionManager::DeleteCollider(ICollider* _collider) {
	// 削除
	for (size_t i = 0; i < colliders_.size(); ++i) {
		if (colliders_[i] == _collider) {
			colliders_[i] = colliders_.back();
			colliders_.pop_back();
			return;
		}
	}
}

void CollisionManager::CheckCollisionPair(ICollider* colliderA, ICollider* colliderB) {
	// 衝突コールバック
	auto HandleCollisionIf = [&](bool isHit) {
		if (isHit) {
			// あらかじめ衝突属性を取得しておく(OnCollision内で変化したときのため)
			CollisionAttribute attrA = colliderA->GetCollisionAttribute();
			CollisionAttribute attrB = colliderB->GetCollisionAttribute();

			// 衝突時コールバックを呼び出す
			colliderA->OnCollision(colliderB, attrB);
			colliderB->OnCollision(colliderA, attrA);
		}
	};

	// 形状種別取得
	const auto shapeA = colliderA->GetColliderShape();
	const auto shapeB = colliderB->GetColliderShape();

	// 関数テーブルから衝突関数取得
	CollisionFunc func = kCollisionTable[static_cast<int>(shapeA)][static_cast<int>(shapeB)];

	// 衝突していないのでreturn
	if (!func) {
		return;
	}

	// 衝突判定
	HandleCollisionIf(func(colliderA, colliderB));
}

CollisionManager::GridCell CollisionManager::WorldToCell(const Vector3& _position) const {
	GridCell cell;
	cell.x = static_cast<int32_t>(std::floor(_position.x / cellSize_));
	cell.y = static_cast<int32_t>(std::floor(_position.y / cellSize_));
	cell.z = static_cast<int32_t>(std::floor(_position.z / cellSize_));
	return cell;
}

void CollisionManager::BuildGrid() {
	grid_.clear();

	for (ICollider* collider : colliders_) {
		// シーンタグが現在シーンのものではない場合はグリッドに入れない
		if (collider->GetSceneTag() != SceneManager::GetInstance()->GetCurrentScene()->GetSceneName()) {
			continue;
		}

		GridCell cell = WorldToCell(collider->GetCenter());
		grid_[cell].push_back(collider);
	}
}

bool SphereXSphere(ICollider* a, ICollider* b) {
	auto sa = static_cast<SphereColliderBase*>(a);
	auto sb = static_cast<SphereColliderBase*>(b);

	// 判定
	return MyMath::IsCollision(sa->GetCapsule(), sb->GetCapsule());
}

bool SphereXAABB(ICollider* a, ICollider* b) {
	auto sa = static_cast<SphereColliderBase*>(a);
	auto sb = static_cast<AABBColliderBase*>(b);

	// 判定
	return MyMath::IsCollision(sa->GetCapsule(), sb->GetAABB());
}

bool SphereXOBB(ICollider* a, ICollider* b) {
	auto sa = static_cast<SphereColliderBase*>(a);
	auto ob = static_cast<OBBColliderBase*>(b);

	// 判定
	return MyMath::IsCollision(sa->GetCapsule(), ob->GetOBB());
}

bool AABBXSphere(ICollider* a, ICollider* b) {
	// 順序入れ替え
	return SphereXAABB(b, a);
}

bool AABBXAABB(ICollider* a, ICollider* b) {
	auto aa = static_cast<AABBColliderBase*>(a);
	auto ab = static_cast<AABBColliderBase*>(b);

	// 判定
	return MyMath::IsCollision(aa->GetAABB(), ab->GetAABB());
}

bool AABBXOBB(ICollider* a, ICollider* b) {
	auto aa = static_cast<AABBColliderBase*>(a);
	auto ob = static_cast<OBBColliderBase*>(b);

	// 判定
	return MyMath::IsCollision(aa->GetAABB(), ob->GetOBB());
}

bool OBBXSphere(ICollider* a, ICollider* b) {
	// 順序入れ替え
	return SphereXOBB(b, a);
}

bool OBBXAABB(ICollider* a, ICollider* b) {
	// 順序入れ替え
	return AABBXOBB(b, a);
}

bool OBBXOBB(ICollider* a, ICollider* b) {
	auto oa = static_cast<OBBColliderBase*>(a);
	auto ob = static_cast<OBBColliderBase*>(b);

	// 判定
	return MyMath::IsCollision(oa->GetOBB(), ob->GetOBB());
}

} // namespace Norm