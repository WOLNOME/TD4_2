#include "BaseEnemy.h"
/// ===Include=== ///
// Engine
#include "Object3dManager.h"
#include "CollisionManager.h"
#include "TextureManager.h"
// Application
#include <application/object/collision/ObjectCollider.h>
// Player
#include <application/object/Character/Player.h>
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
#include "State/EnemyStopState.h"

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
	auto* enemyBodyCollider = dynamic_cast<EnemyBodyCollider*>(bodyCollider_.get());
	if (enemyBodyCollider) {
		enemyBodyCollider->SetCollisionAttribute(CollisionAttribute::Enemy);
		enemyBodyCollider->SetWorldTransform(&worldTransform_);
		enemyBodyCollider->SetOffset({ 0.0f, 0.0f, 0.0f });
		enemyBodyCollider->SetOBBSize({ 1.0f, 1.0f, 1.0f }); // Bodyのコライダーサイズ
		enemyBodyCollider->SetHolder(this);
	}

	// Area
	areaCollider_ = std::make_unique<EnemyAreaCollider>(this);
	auto* enemyAreaCollider = dynamic_cast<EnemyAreaCollider*>(areaCollider_.get());
	if (enemyAreaCollider) {
		enemyAreaCollider->SetCollisionAttribute(CollisionAttribute::EnemyArea);
		enemyAreaCollider->SetWorldTransform(&worldTransform_);
		enemyAreaCollider->SetOffset({ 0.0f, 0.0f, 0.0f });
		enemyAreaCollider->SetOBBSize({ 1.3f, 1.0f, 1.3f }); // Areaのコライダーサイズ
		enemyAreaCollider->SetHolder(this); // 自身のポインタをセット
	}


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
		isDead_ = true;
	}

	/// ===Stateの管理=== ///
	if (currentState_) {
		// 各Stateの更新
		currentState_->Update();
	}

	/// ===回転の処理=== ///
	if (!isFootColliding_ || isAreaColliding_) {
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
	isFootColliding_ = false;
	isAreaColliding_ = false;
	// 回転中のフラグの状態を設定
	if (isRotating_) {
		isFootColliding_ = true; // 回転中は衝突中とみなす
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
	ImGui::Checkbox("isFlash_", &isFlash_);
	ImGui::Checkbox("isAreaColliding_", &isAreaColliding_);
	ImGui::Checkbox("isFootColliding_", &isFootColliding_);

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
	// フラッシュ状態が変化した場合の処理
	if (isFlash_) {
		isFlash_ = false;	
		Flash();
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
/// フラッシュを喰らった時の処理
///-------------------------------------------///
void BaseEnemy::Flash() {
	// フラッシュの構造体などが有ればそれを受け取り、フラッシュの範囲内にEnemyがいたらStateを移動するようにする。

	ChangeState(std::make_unique<EnemyStopState>(std::move(currentState_)));
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

