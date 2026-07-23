#include "EnemyDeadState.h"
// Enemy
#include "application/object/entity/enemy/BaseEnemy.h"

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void EnemyDeadState::Enter(BaseEnemy* enemy) {
	enemy_ = enemy;

	// 死亡時は横方向の移動を止める
	enemy_->SetVelocity({ 0.0f, 0.0f, 0.0f });

	// 初期ジャンプ速度を設定
	yVelocity_ = parames_.jumpPower; 
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void EnemyDeadState::Update() {

	/// ===死亡演出の更新=== ///
	DeadAnimation();

	/// ===死亡演出が終わったら死亡フラグを立てる=== ///
	if (isFinishedAnimation_) {
		enemy_->EnemyDead();
	}
}

///-------------------------------------------/// 
/// 終了処理
///-------------------------------------------///
void EnemyDeadState::Exit() {
	EnemyState::Exit();
}

///-------------------------------------------/// 
/// 死亡アニメーションの処理
///-------------------------------------------///
void EnemyDeadState::DeadAnimation() {
	// ワールド座標を取得
	Norm::WorldTransform &wt = enemy_->GetWorldTransform();
	Norm::Vector3 velocity = enemy_->GetVelocity();

	// 色のフェード処理
	Norm::Vector4 color = enemy_->GetColor();
	color.w -= parames_.colorFadeSpeed; // 徐々に透明にする
	if (color.w < 0.0f) {
		color.w = 0.0f;
		isFinishedAnimation_ = true; // アニメーション終了フラグを立てる
		return;
	}

	// X軸の回転
	Norm::Vector3 rotate = wt.GetRotate();
	rotate.z += parames_.rotationSpeed; // 回転速度を加算
	wt.SetRotate(rotate);

	// X軸の移動処理
	velocity.x += parames_.moveSpeedX; // 徐々にX軸方向に移動

	// Y軸の処理
	yVelocity_ += parames_.gravity; // 重力の影響
	if (yVelocity_ < -1.0f) {
		yVelocity_ = -1.0f; // 最大落下速度の制限
	}

	// Z軸の方向に徐々に移動
	Norm::Vector3 translate = wt.GetTranslate();
	if (translate.z <= 2.0f) {
		velocity.z += parames_.moveSpeedZ;
	} else {
		velocity.z = 0.0f;
	}

	// 設定した情報を反映
	enemy_->SetVelocity(velocity);
	enemy_->SetColor(color);
}
