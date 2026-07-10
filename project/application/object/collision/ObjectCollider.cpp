#include "ObjectCollider.h"

// Application
#include <application/object/Character/Player.h>
#include <application/object/entity/enemy/BaseEnemy.h>

using namespace Norm;

ObjectCollider::ObjectCollider(Norm::Object3d* _holder) : OBBColliderBase(), holder_(_holder) {
}

void ObjectCollider::Debug() {
#ifdef _DEBUG
	//基底クラスのデバッグ処理
	OBBColliderBase::Debug();

	//debugLineColorを元に戻す
	debugLineColor_ = { 1,1,1,1 };

#endif // _DEBUG

}

void ObjectCollider::OnCollision(ICollider* _other, CollisionAttribute _attribute) {
	//当たり判定時の処理
	switch (_attribute) {
	case CollisionAttribute::Player:
	{
		//色を赤にする
		debugLineColor_ = { 1,0,0,1 };
		break;
	}
	case CollisionAttribute::Enemy:
	{
		//色を赤にする
		debugLineColor_ = { 1,0,0,1 };
		break;
	}
	default:
		break;
	}

	void* holder = GetHolder();
	if (holder) {
		Player* player = static_cast<Player*>(holder);
		player->OnCollision(_other, _attribute);

		BaseEnemy* enemy = static_cast<BaseEnemy*>(holder);
		enemy->OnCollision(_other, _attribute);
	}
}
