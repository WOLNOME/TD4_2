#include "EnemyMoveState.h"
#include "application/object/entity/enemy/BaseEnemy.h"

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void EnemyMoveState::Enter(BaseEnemy* enemy) {
	enemy_ = enemy;
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void EnemyMoveState::Update() {

}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void EnemyMoveState::Exit() {

}
