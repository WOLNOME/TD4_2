#pragma once
/// ===Include=== ///
// Object
#include "BaseCamera.h"
#include "Object3d.h"
#include "WorldTransform.h"
// EnemyState
#include "State/EnemyState.h"
// C++標準ライブラリ
#include <memory>

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
	void Initialize(Norm::Vector3 position);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// ImGuiデバッグ処理
	/// </summary>
	void DebugWithImGui();

	/// <summary>
	/// 攻撃処理
	/// </summary>
	void Attack(const Norm::Vector3 playerPos);

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
	Norm::WorldTransform& GetWorldTransform() { return worldTransform_; }
	 
#ifdef _DEBUG
	Norm::Vector3 GetDebugPlayerPos() const { return debugPlayerPos_; }
#endif // DEBUG


private:
	/// ============================== ///
	///		メンバ変数
	/// ============================== ///
	int32_t textureHandle_ = EOF;
	std::unique_ptr<Norm::Object3d> object3d_ = nullptr;
	Norm::WorldTransform worldTransform_;

	// 現在の状態
	std::unique_ptr<EnemyState> currentState_ = nullptr;

	// 攻撃関連の変数
	struct BaseEnemyAttackData {
		float chargeSpeed = 0.1f; // 突進速度
		float rotateSpeed = 0.05f; // 回転速度
	};
	BaseEnemyAttackData attackData_{};

	// 目標のY軸回転角
	float targetFacingRotationY_ = 0.0f; 

#ifdef _DEBUG
	Norm::Vector3 debugPlayerPos_ = { 0.0f, 0.0f, 0.0f };
	bool isAttack_ = false;
	bool preIsAttack_ = false;
	bool isEscape_ = false;
	bool preIsEscape_ = false;
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

