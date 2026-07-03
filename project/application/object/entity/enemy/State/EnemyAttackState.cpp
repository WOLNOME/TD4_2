#include "EnemyAttackState.h"
//// ===include=== ///
#include "application/object/entity/enemy/BaseEnemy.h"

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void EnemyAttackState::Enter(BaseEnemy* enemy) {
	enemy_ = enemy;
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void EnemyAttackState::Update() {
#ifdef _DEBUG
	enemy_->Attack(enemy_->GetDebugPlayerPos());
#endif // _DEBUG
}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void EnemyAttackState::Exit() {
	EnemyState::Exit();
}
