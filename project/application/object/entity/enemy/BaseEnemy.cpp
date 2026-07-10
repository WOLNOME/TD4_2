#include "BaseEnemy.h"
/// ===Include=== ///
// Engine
#include "Object3dManager.h"
#include "CollisionManager.h"
// Application
#include <application/object/collision/ObjectCollider.h>
// EnemyState
#include "State/EnemyMoveState.h"
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
void BaseEnemy::Initialize(Norm::Vector3 position) {
	/// ===オブジェクト=== ///
	// 生成・初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(ModelTag{}, Object3dManager::GetInstance()->GenerateName("enemy"), "enemy");

	/// ===ワールドトランスフォーム=== ///
	// 初期化
	worldTransform_.Initialize();
	worldTransform_.SetTranslate(position);
	//オブジェクトにセット
	object3d_->RegistWorldTransform(&worldTransform_);

	/// ===コライダー=== ///
	// 生成 + 登録
	collider_ = std::make_unique<ObjectCollider>(object3d_.get());
	auto* enemyCollider = dynamic_cast<ObjectCollider*>(collider_.get());
	if (enemyCollider) {
		enemyCollider->SetCollisionAttribute(CollisionAttribute::Enemy);
		enemyCollider->SetWorldTransform(&worldTransform_);
		enemyCollider->SetOffset({ 0.0f, 0.0f, 0.0f });
		enemyCollider->SetOBBSize({ 1.0f, 1.0f, 1.0f }); // 敵のコライダーサイズ
		enemyCollider->SetHolder(this); // 自身のポインタをセット
	}
	

	/// ===Stateの設定=== ///
	ChangeState(std::make_unique<EnemyMoveState>());
}

///-------------------------------------------/// 
/// 更新処理
///-------------------------------------------///
void BaseEnemy::Update() {
	/// ===Stateの管理=== ///
	if (currentState_) {
		// 各Stateの更新
		currentState_->Update();
	}
}

///-------------------------------------------/// 
/// ImGuiデバッグ処理
///-------------------------------------------///
void BaseEnemy::DebugWithImGui() {
#ifdef _DEBUG

	preIsAttack_ = isAttack_;
	preIsEscape_ = isEscape_;

	ImGui::Begin("BaseEnemy");
	ImGui::DragFloat3("debugPlayerPos_", &debugPlayerPos_.x, 0.1f);
	ImGui::Checkbox("isAttack_", &isAttack_);
	ImGui::Checkbox("isEscape_", &isEscape_);

	Vector3 worldPos = worldTransform_.GetWorldTranslate();
	ImGui::DragFloat3("position", &worldPos.x, 0.1f);

	ImGui::End();

	// 攻撃状態が変化した場合の処理
	if (!preIsAttack_ && isAttack_) {
		isEscape_ = false; // 攻撃状態に入るときは逃走状態を解除
		ChangeState(std::make_unique<EnemyChaseState>());
	}
	// 逃走状態が変化した場合の処理
	if (!preIsEscape_ && isEscape_) {
		isAttack_ = false; // 逃走状態に入るときは攻撃状態を解除
		ChangeState(std::make_unique<EnemyEscapeState>());
	}

	// コライダーデバッグ
	if (collider_) {
		auto* enemyCollider = dynamic_cast<ObjectCollider*>(collider_.get());
		if (enemyCollider) {
			enemyCollider->Debug();
		}
	}

#endif // _DEBUG
}

///-------------------------------------------/// 
/// 追跡処理
///-------------------------------------------///
void BaseEnemy::Chase(const Vector3 playerPos) {
	// Playerの方向を計算
	Vector3 toPlayer = playerPos - worldTransform_.GetWorldTranslate();
	float distance = toPlayer.Length();

	// 突進攻撃
	if (distance > 0.0001f) {
		Norm::Vector3 dir = { toPlayer.x / distance, toPlayer.y / distance, 0.0f };
		Norm::Vector3 newPos = worldTransform_.GetWorldTranslate();
		newPos.x += dir.x * chaseData_.chargeSpeed;
		newPos.y += dir.y * chaseData_.chargeSpeed;
		worldTransform_.SetTranslate(newPos);
	}

	// 向きの更新
	UpdateFacing(toPlayer.x);
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
	float currentRotationY_ = LerpAngle(worldTransform_.GetRotate().y, targetFacingRotationY_, chaseData_.rotateSpeed);
	worldTransform_.SetRotate({ 0.0f, currentRotationY_, 0.0f });
}

///-------------------------------------------/// 
/// 衝突時コールバック
///-------------------------------------------///
void BaseEnemy::OnCollision(ICollider* other, CollisionAttribute otherAttr) {
	other, otherAttr; // 未使用の引数を無視するためのダミー
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
