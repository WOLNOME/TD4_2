#include "BaseEnemy.h"
/// ===Include=== ///
// Engine
#include "Object3dManager.h"
#include "CollisionManager.h"
#include "TextureManager.h"
#include "PointLight.h"
// Application
#include <application/system/LightManager.h>
// Player
#include <application/object/Character/Player.h>
// EnemyState
#include "State/EnemyMoveState.h"
#include "State/EnemyStopState.h"
#include "State/EnemyDeadState.h"
#include "State/EnemyKnockbackState.h"
// Math
#include "MyMath.h"
// Debug
#ifdef _DEBUG
#include <imgui.h>
#include "State/EnemyChaseState.h"
#include "State/EnemyEscapeState.h"
#endif // _DEBUG


using namespace Norm;

///-------------------------------------------/// 
/// 初期化処理
///-------------------------------------------///
void BaseEnemy::Initialize(Norm::Vector3 position, Norm::Player* player, EnemyDirection FirstDirection) {
	/// ===オブジェクト=== ///
	// 生成・初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(ModelTag{}, Object3dManager::GetInstance()->GenerateName("enemy"), "ghost");
	object3d_->SetIsOutline(true);
	object3d_->SetOutlineParam(TextureManager::GetInstance()->LoadTexture("red.png"), 1.1f);

	/// ===ワールドトランスフォーム=== ///
	// 初期化
	worldTransform_.Initialize();
	worldTransform_.SetTranslate(position);
	//オブジェクトにセット
	object3d_->RegistWorldTransform(&worldTransform_);

	/// ===Player=== ///
	player_ = player;

	/// ===コライダー=== ///
	// 生成 + 登録
	bodyCollider_ = std::make_unique<EnemyBodyCollider>(this);
	bodyCollider_->SetCollisionAttribute(CollisionAttribute::Enemy);
	bodyCollider_->SetWorldTransform(&worldTransform_);
	bodyCollider_->SetOffset({ 0.0f, 0.0f, 0.0f });
	bodyCollider_->SetOBBSize({ 1.0f, 1.0f, 1.0f }); // Bodyのコライダーサイズ
	bodyCollider_->SetHolder(this);

	// Area
	areaCollider_ = std::make_unique<EnemyAreaCollider>(this);
	areaCollider_->SetCollisionAttribute(CollisionAttribute::EnemyArea);
	areaCollider_->SetWorldTransform(&worldTransform_);
	areaCollider_->SetOffset({ 0.0f, 0.0f, 0.0f });
	areaCollider_->SetOBBSize({ 1.3f, 1.0f, 1.3f });
	areaCollider_->SetHolder(this);

	/// ===初期位置=== ///
	initialPosition_ = position;

	/// ===方向の設定=== ///
	currentDirection_ = FirstDirection;

	/// ===Stateの設定=== ///
	ChangeState(std::make_unique<EnemyMoveState>());
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void BaseEnemy::Update() {
	/// ===死亡フラグの確認=== ///
	if (isBodyColliding_) {
		// Colliderを解放
		bodyCollider_.reset();
		areaCollider_.reset();
		// 状態を死亡状態に変更
		ChangeState(std::make_unique<EnemyDeadState>());
	}

	/// ===Playerと衝突した時=== ///
	if (isPlayerColliding_) {
		// 現在の状態がEnemyKnockbackStateでない場合
		if (!dynamic_cast<EnemyKnockbackState*>(currentState_.get()) && !dynamic_cast<EnemyStopState*>(currentState_.get())) {
			//EnemyKnockbackStateに変更
			ChangeState(std::make_unique<EnemyKnockbackState>(std::move(currentState_)));
		}
	}

	/// ===フラッシュの処理=== ///
	IsFlash();

	/// ===Stateの管理=== ///
	if (currentState_) {
		// 各Stateの更新
		currentState_->Update();
	}

	/// ===回転の処理=== ///
	if (isAreaColliding_) {
		// Y軸を180度回転させる
		SetCurrentDirection(Opposite(currentDirection_));
	}

	/// ===座標の更新=== ///
	Vector3 currentPos = worldTransform_.GetTranslate();
	worldTransform_.SetTranslate({
		currentPos.x + velocity_.x,
		currentPos.y + velocity_.y,
		currentPos.z + velocity_.z
	});

	// 行列の更新
	worldTransform_.UpdateMatrix();

	/// ===フラグの更新=== ///
	// 衝突中かどうかのフラグをリセット
	isAreaColliding_ = false;
	isPlayerColliding_ = false;
	// 回転中のフラグの状態を設定
	if (isRotating_) {
		isAreaColliding_ = false;
	}
}

///-------------------------------------------/// 
/// ImGuiデバッグ処理
///-------------------------------------------///
void BaseEnemy::DebugWithImGui() {
#ifdef _DEBUG

	ImGui::Begin("BaseEnemy");
	ImGui::Checkbox("isAttack_", &isAttack_);
	ImGui::Checkbox("isEscape_", &isEscape_);
	ImGui::Checkbox("isTurning_", &isTurning_);
	ImGui::Checkbox("isAreaColliding_", &isAreaColliding_);

	Vector3 worldPos = worldTransform_.GetWorldTranslate();
	ImGui::DragFloat3("position", &worldPos.x, 0.1f);

	ImGui::End();

	// 攻撃状態が変化した場合の処理
	if (isAttack_) {
		isAttack_ = false;
		ChangeState(std::make_unique<EnemyChaseState>());
	}
	// 逃走状態が変化した場合の処理
	if (isEscape_) {
		isEscape_ = false;
		ChangeState(std::make_unique<EnemyEscapeState>());
	}

	// コライダーデバッグ
	if (bodyCollider_) {
		auto* enemyBodyCollider = dynamic_cast<EnemyBodyCollider*>(bodyCollider_.get());
		if (enemyBodyCollider) {
			enemyBodyCollider->Debug();
		}
	}
	if (areaCollider_) {
		auto* enemyAreaCollider = dynamic_cast<EnemyAreaCollider*>(areaCollider_.get());
		if (enemyAreaCollider) {
			enemyAreaCollider->Debug();
		}
	}

#endif // _DEBUG
}

///-------------------------------------------/// 
/// 状態を変更する
///-------------------------------------------///
void BaseEnemy::ChangeState(std::unique_ptr<EnemyState> newState) {
	if (currentState_) {
		// 古い状態を解放  
		currentState_->Exit();
		currentState_.reset();
	}

	// 新しい状態をセット  
	currentState_ = std::move(newState);
	// 新しい状態の初期化  
	currentState_->Enter(this);
}

///-------------------------------------------/// 
/// 向きを更新する
///-------------------------------------------///
void BaseEnemy::UpdateFacing(float directionX) {
	// directionXの方向を向くための目標Y軸回転角を決定
	constexpr float kFacingThreshold = 0.01f;
	if (std::fabs(directionX) > kFacingThreshold) {
		targetFacingRotationY_ = (directionX > 0.0f) ? 0.0f : pi;
	}

	// 決定した左右の向きへ滑らかに回転させる
	float currentRotationY_ = LerpAngle(worldTransform_.GetRotate().y, targetFacingRotationY_, rotationSpeed_);
	worldTransform_.SetRotate({ 0.0f, currentRotationY_, 0.0f });

	// 回転がほぼ完了したかを判定するための許容誤差
	constexpr float kRotationFinishEpsilon = 0.01f;

	// 目標角度と現在の角度の差が許容誤差以内なら回転完了とする
	if (std::fabs(currentRotationY_ - targetFacingRotationY_) < kRotationFinishEpsilon) {
		isRotating_ = false;
	} else {
		isRotating_ = true; // 回転中
	}
}

///-------------------------------------------/// 
///	ライトに当たった時の処理
///-------------------------------------------///
bool BaseEnemy::IsLightHit() {
	// フラッシュとの距離を計算
	Vector3 diff = worldTransform_.GetTranslate() - lightManager_->GetPointLight()->GetPosition();
	float distance = diff.LengthSq(); // 距離の2乗を計算
	float hitRange = lightManager_->GetPointLight()->GetRadius() + 1.0f; // 1.0fは敵の半径

	return distance <= hitRange;
}

///-------------------------------------------/// 
/// フラッシュを喰らった時の処理
///-------------------------------------------///
void BaseEnemy::IsFlash() {
	// ライトに当たっていなければ処理を終了
	if (!IsLightHit()) return;

	// フラッシュを喰らった場合の処理
	if (lightManager_->GetIsFlush()) {
		// 衝突属性をEnemyGhostに変更して、衝突判定を無効化
		bodyCollider_->SetCollisionAttribute(CollisionAttribute::EnemyGhost);

		// EnemyStopStateに変更
		ChangeState(std::make_unique<EnemyStopState>());
	}
}

///-------------------------------------------/// 
/// 角度補間
///-------------------------------------------///
float BaseEnemy::LerpAngle(float a, float b, float t) {
	float diff = b - a;
	// 差分を-π~πの範囲に収め、遠回りせず最短経路で回転させる
	while (diff > pi) {
		diff -= 2.0f * pi;
	}
	while (diff < -pi) {
		diff += 2.0f * pi;
	}
	return a + diff * t;
}

