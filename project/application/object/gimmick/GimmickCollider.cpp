#include "GimmickCollider.h"

GimmickCollider::GimmickCollider(GimmickBase* _holder):OBBColliderBase(), holder_(_holder)
{


}

void GimmickCollider::Debug()
{
	//基底クラスのデバッグ処理
	OBBColliderBase::Debug();

	//debugLineColorを元に戻す
	debugLineColor_ = { 1,1,1,1 };
}

void GimmickCollider::OnCollision(Norm::ICollider* _other, Norm::CollisionAttribute _attribute)
{
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



}
