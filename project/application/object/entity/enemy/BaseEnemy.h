#pragma once
/// ===Include=== ///
// Object
#include "BaseCamera.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include "ICollider.h"
// Collider
#include "collider/EnemyMoveCollider.h"
#include "collider/EnemyAreaCollider.h"
// EnemyState
#include "State/Base/EnemyState.h"
// C++標準ライブラリ
#include <memory>

/// ===Enemyの向き=== ///
enum class EnemyDirection {
	Right = -1,
	Left = 1
};

/// ===変換ロジック=== ///
constexpr float DirectionToSign(EnemyDirection dir) { return static_cast<float>(dir); }

/// ===反転=== ///
constexpr EnemyDirection Opposite(EnemyDirection dir) {
	return (dir == EnemyDirection::Right) ? EnemyDirection::Left : EnemyDirection::Right;
}

///=====================================================/// 
/// BaseEnemy
/// Enemyの基盤クラス
///=====================================================///
class BaseEnemy {
public:
	/// ============================== ///
	///		メンバ関数
	/// ============================== ///

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Norm::Vector3 position, EnemyDirection FirstDirection = EnemyDirection::Left);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// ImGuiデバッグ処理
	/// </summary>
	void DebugWithImGui();

	/// <summary>
	/// 追跡処理
	/// </summary>
	void Chase(const Norm::Vector3 playerPos);

	/// <summary>
	/// 状態を変更する
	/// </summary>
	/// <param name="newState"></param>
	void ChangeState(std::unique_ptr<EnemyState> newState);

	/// <summary>
	/// 向きを更新する
	/// </summary>
	/// <param name="directionX"></param>
	void UpdateFacing(float directionX);

public:
	/// <summary>
	/// 衝突時コールバック
	/// </summary>
	/// <param name="other"></param>
	/// <param name="otherAttr"></param>
	void OnCollision(Norm::ICollider* other, Norm::CollisionAttribute otherAttr);

public:
	/// ============================== ///
	///		getter
	/// ============================== ///
	/// <summary>
	/// ワールド変換を取得する
	/// </summary>
	/// <returns></returns>
	Norm::WorldTransform& GetWorldTransform() { return worldTransform_; }

	/// <summary>
	/// velocityを取得する
	/// </summary>
	/// <returns></returns>
	Norm::Vector3 GetVelocity() const { return velocity_; }

	/// <summary>
	/// EnemyDrectionを取得する
	/// </summary>
	/// <returns></returns>
	EnemyDirection GetCurrentDirection() const { return currentDirection_; }

	/// <summary>
	/// 衝突中かどうかを取得する(足元)
	/// </summary>
	/// <returns></returns>
	bool IsFootColliding() const { return isFootColliding_; }

	/// <summary>
	/// エリアと衝突中かどうかを取得する
	/// </summary>
	/// <returns></returns>
	bool IsAreaColliding() const { return isAreaColliding_; }

	/// <summary>
	/// 回転中かどうかを取得
	/// </summary>
	/// <returns></returns>
	bool IsRotating() const { return isRotating_; }

#ifdef _DEBUG
	Norm::Vector3 GetDebugPlayerPos() const { return debugPlayerPos_; }
	bool GetIsTurning() const { return isTurning_; }
#endif // DEBUG

public:
	/// ============================== ///
	///		setter
	/// ============================== ///
	/// <summary>
	/// 速度を設定する
	/// </summary>
	/// <param name="velocity"></param>
	void SetVelocity(const Norm::Vector3& velocity) { velocity_ = velocity; }
	
	/// <summary>
	/// 現在の向きを設定する
	/// </summary>
	/// <param name="dir"></param>
	void SetCurrentDirection(EnemyDirection dir) { currentDirection_ = dir; }

	/// <summary>
	/// 衝突中かどうかを設定する(足元)
	/// </summary>
	/// <param name="isColliding"></param>
	void SetFootColliding(bool isColliding) { isFootColliding_ = isColliding; }

	/// <summary>
	/// 衝突中かどうかを設定する(エリア)
	/// </summary>
	/// <param name="isColliding"></param>
	void SetAreaColliding(bool isColliding) { isAreaColliding_ = isColliding; }

#ifdef _DEBUG
	void SetIsTurning(bool isFlag) { isTurning_ = isFlag; }
#endif // _DEBUG


private:
	/// ============================== ///
	///		メンバ変数
	/// ============================== ///
	std::unique_ptr<Norm::Object3d> object3d_ = nullptr;
	Norm::WorldTransform worldTransform_;
	std::unique_ptr<Norm::ICollider> collider_ = nullptr;
	std::unique_ptr<EnemyAreaCollider> areaCollider_ = nullptr;
	std::unique_ptr<EnemyMoveCollider> moveCollider_ = nullptr;

	// 現在の状態
	std::unique_ptr<EnemyState> currentState_ = nullptr;
	EnemyDirection currentDirection_ = EnemyDirection::Right;

	// 速度
	Norm::Vector3 velocity_ = { 0.0f, 0.0f, 0.0f };

	// 追跡関連の変数
	struct BaseEnemyChaseData {
		float chargeSpeed = 0.1f; // 突進速度
		float rotateSpeed = 0.05f; // 回転速度
	};
	BaseEnemyChaseData chaseData_{};

	// 目標のY軸回転角
	float targetFacingRotationY_ = 0.0f; 

	// 衝突中かどうかを保持するフラグ
	bool isFootColliding_ = false;
	bool isAreaColliding_ = false;

	// 回転中かどうかのフラグ
	bool isRotating_ = false; 

#ifdef _DEBUG
	Norm::Vector3 debugPlayerPos_ = { 0.0f, 0.0f, 0.0f };
	bool isAttack_ = false;
	bool preIsAttack_ = false;
	bool isEscape_ = false;
	bool preIsEscape_ = false;
	bool isTurning_ = false;
#endif // _DEBUG

private:

	/// <summary>
	/// 角度補間関数
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	float LerpAngle(float a, float b, float t);
};

