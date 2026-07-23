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
#include "collider/EnemyBodyCollider.h"
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

/// ===前方宣言=== ///
class LightManager;
namespace Norm {
	class Player;
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
	void Initialize(Norm::Vector3 position, Norm::Player* player, EnemyDirection FirstDirection = EnemyDirection::Left);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// ImGuiデバッグ処理
	/// </summary>
	void DebugWithImGui();

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
	/// ============================== ///
	///		getter
	/// ============================== ///
	/// <summary>
	/// ワールド変換を取得する
	/// </summary>
	/// <returns></returns>
	Norm::WorldTransform& GetWorldTransform() { return worldTransform_; }

	/// <summary>
	/// カラーの取得
	/// </summary>
	/// <returns></returns>
	Norm::Vector4 GetColor() const { return object3d_->GetColor(); }

	/// <summary>
	/// プレイヤーのポインタを取得
	/// </summary>
	/// <returns></returns>
	Norm::Player* GetPlayer() const { return player_; }

	/// <summary>
	/// velocityを取得する
	/// </summary>
	/// <returns></returns>
	Norm::Vector3 GetVelocity() const { return velocity_; }

	/// <summary>
	/// 初期位置を取得する
	/// </summary>
	/// <returns></returns>
	Norm::Vector3 GetInitialPosition() const { return initialPosition_; }

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
	/// 体と衝突中かどうかを取得する
	/// </summary>
	/// <returns></returns>
	bool IsBodyColliding() const { return isBodyColliding_; }

	/// <summary>
	/// プレイヤーと衝突中かどうかを取得する
	/// </summary>
	/// <returns></returns>
	bool IsPlayerColliding() const { return isPlayerColliding_; }

	/// <summary>
	/// 回転中かどうかを取得
	/// </summary>
	/// <returns></returns>
	bool IsRotating() const { return isRotating_; }

	/// <summary>
	/// 死亡しているかどうかを取得
	/// </summary>
	/// <returns></returns>
	bool IsDead() const { return isDead_; }

#ifdef _DEBUG
	bool GetIsTurning() const { return isTurning_; }
#endif // DEBUG

public:
	/// ============================== ///
	///		setter
	/// ============================== ///
	
	/// <summary>
	/// LightManagerのポインタを設定する
	/// </summary>
	/// <param name="lightManager"></param>
	void SetLightManager(LightManager* lightManager) { lightManager_ = lightManager; }

	/// <summary>
	/// 色の設定
	/// </summary>
	/// <param name="color"></param>
	void SetColor(const Norm::Vector4& color) { object3d_->SetColor(color); }

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
	/// 死亡したことを設定する
	/// </summary>
	void EnemyDead() { isDead_ = true; }

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

	/// <summary>
	/// 衝突中かどうかを設定する(体)
	/// </summary>
	/// <param name="isColliding"></param>
	void SetBodyColliding(bool isColliding) { isBodyColliding_ = isColliding; }

	/// <summary>
	/// 衝突中かどうかを設定する(プレイヤー)
	/// </summary>
	/// <param name="isColliding"></param>
	void SetPlayerColliding(bool isColliding) { isPlayerColliding_ = isColliding; }

#ifdef _DEBUG
	void SetIsTurning(bool isFlag) { isTurning_ = isFlag; }
#endif // _DEBUG

private:
	/// ============================== ///
	///		メンバ変数
	/// ============================== ///
	std::unique_ptr<Norm::Object3d> object3d_ = nullptr;
	Norm::WorldTransform worldTransform_;
	std::unique_ptr<EnemyAreaCollider> areaCollider_ = nullptr;
	std::unique_ptr<EnemyBodyCollider> bodyCollider_ = nullptr;
	// 現在の状態
	std::unique_ptr<EnemyState> currentState_ = nullptr;
	EnemyDirection currentDirection_ = EnemyDirection::Right;

	// プレイヤーのポインタ
	Norm::Player* player_ = nullptr;

	// LightManagerのポインタ
	LightManager* lightManager_ = nullptr;

	// 速度
	Norm::Vector3 velocity_ = { 0.0f, 0.0f, 0.0f };

	// 初期位置
	Norm::Vector3 initialPosition_ = {0.0f, 0.0f, 0.0f};

	// 回転速度
	float rotationSpeed_ = 0.05f;

	// 目標のY軸回転角
	float targetFacingRotationY_ = 0.0f; 

	// 衝突中かどうかを保持するフラグ
	bool isFootColliding_ = false;
	bool isAreaColliding_ = false;
	bool isBodyColliding_ = false;
	bool isPlayerColliding_ = false;

	// 回転中かどうかのフラグ
	bool isRotating_ = false; 

	// 死亡フラグ
	bool isDead_ = false;

#ifdef _DEBUG
	bool isAttack_ = false;
	bool isEscape_ = false;
	bool isTurning_ = false;
#endif // _DEBUG

private:

	/// <summary>
	/// ライトに当たった時の処理
	/// </summary>
	/// <returns></returns>
	bool IsLightHit();

	/// <summary>
	/// フラッシュを喰らった時の処理
	/// </summary>
	void IsFlash();

	/// <summary>
	/// 角度補間関数
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	float LerpAngle(float a, float b, float t);
};

