#include "EnemyEscapeState.h"
//// ===include=== ///
#include "application/object/entity/enemy/BaseEnemy.h"
#include "MyMath.h"
#include "EnemyMoveState.h"
#include <cmath>
#include <random>

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void EnemyEscapeState::Enter(BaseEnemy* enemy) {
	enemy_ = enemy;

	// ランダムな方向を決定する
	std::random_device seedGenerator;
	std::mt19937 engine(seedGenerator());
	std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * Norm::pi);
	float angle = angleDist(engine);

	escapeDirection_ = { std::cos(angle), std::sin(angle), 0.0f };

	// 経過時間をリセット
	escapeTimer_ = 0.0f;
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void EnemyEscapeState::Update() {
	// 経過時間の更新
	escapeTimer_ += 1.0f / 60.0f; // 60FPS想定
	
	Norm::Vector3 newPos = enemy_->GetWorldTransform().GetTranslate();
	newPos.x += escapeDirection_.x * escapeSpeed_;
	newPos.y += escapeDirection_.y * escapeSpeed_;
	enemy_->GetWorldTransform().SetTranslate(newPos);

	// 向きの更新
	float directionX = escapeDirection_.x;
	enemy_->UpdateFacing(directionX);

	// 逃走時間が経過したら移動状態に遷移
	if (escapeTimer_ >= escapeDuration_) {
		enemy_->ChangeState(std::make_unique<EnemyMoveState>());
	}
}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void EnemyEscapeState::Exit() {
	EnemyState::Exit();
}
