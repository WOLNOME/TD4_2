#include "EnemyChaseState.h"
// Enemy
#include "application/object/entity/enemy/BaseEnemy.h"
// Player
#include <application/object/Character/Player.h>

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
	// プレイヤーを追跡する処理
	ChasePlayer();
}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void EnemyChaseState::Exit() {
	EnemyState::Exit();
}

///-------------------------------------------/// 
/// プレイヤーを追う処理
///-------------------------------------------///
void EnemyChaseState::ChasePlayer() {
	// PlayerとEnemyの位置を取得
	Norm::Vector3 playerPos = enemy_->GetPlayer()->GetTranslate();
	Norm::Vector3 enemyPos = enemy_->GetWorldTransform().GetTranslate(); 

	// プレイヤーへの方向ベクトルを計算
	Norm::Vector3 toPlayer = {
		playerPos.x - enemyPos.x,
		playerPos.y - enemyPos.y,
		playerPos.z - enemyPos.z
	};

	float distance = toPlayer.Length();

	// 追跡
	if (distance > 0.0001f) {
		Norm::Vector3 dir = { toPlayer.x / distance, toPlayer.y / distance, toPlayer.z / distance }; 
		Norm::Vector3 velocity;
		velocity.x = dir.x * chaseSpeed_;
		velocity.y = dir.y * chaseSpeed_;
		velocity.z = 0.0f;
		enemy_->SetVelocity(velocity);
	}

	// 向きの更新
	enemy_->UpdateFacing(toPlayer.x);
}
