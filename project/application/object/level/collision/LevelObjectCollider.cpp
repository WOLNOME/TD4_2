#include "LevelObjectCollider.h"

//アプリケーション
#include "application/object/level/base/IBaseLevelObject.h"


using namespace Norm;

LevelObjectCollider::LevelObjectCollider(IBaseLevelObject* _holder) : OBBColliderBase(), holder_(_holder) {
}

void LevelObjectCollider::Debug() {
#ifdef _DEBUG
	//基底クラスのデバッグ処理
	OBBColliderBase::Debug();

	//debugLineColorを元に戻す
	debugLineColor_ = { 1,1,1,1 };

#endif // _DEBUG
}

void LevelObjectCollider::OnCollision(ICollider* _other, CollisionAttribute _attribute) {
	_other, _attribute;
	//当たり判定の属性によって分岐
	/*switch (_attribute) {
	default:
		break;
	}*/

}
