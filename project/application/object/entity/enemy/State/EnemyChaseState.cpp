#include "EnemyChaseState.h"
//// ===include=== ///
#include "application/object/entity/enemy/BaseEnemy.h"

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void EnemyChaseState::Enter(BaseEnemy* enemy) {
	enemy_ = enemy;
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void EnemyChaseState::Update() {
#ifdef _DEBUG
	enemy_->Chase(enemy_->GetDebugPlayerPos());
#endif // _DEBUG
}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void EnemyChaseState::Exit() {
	EnemyState::Exit();
}
