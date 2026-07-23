#pragma once

// Engine
#include <Object3d.h>
#include <WorldTransform.h>
#include <Input.h>
#include <ICollider.h>
#include <CombinedParticle.h>
#include <Audio.h>

namespace Norm {
	// =========================================================
	// プレイヤークラス
	// =========================================================
	class Player {
	public:
		// =========================================================
		// Public Methods
		// =========================================================

		/// <summary>
		/// 初期化処理
		/// </summary>
		void Initialize();

		/// <summary>
		/// 更新処理
		/// </summary>
		void Update();

		/// <summary>
		/// デバッグ表示
		/// </summary>
		void Debug();

		/// <summary>
		/// 衝突時コールバック
		/// </summary>
		/// <param name="other"></param>
		/// <param name="otherAttr"></param>
		void OnCollision(ICollider* other, CollisionAttribute otherAttr);

		// =========================================================
		// Accessor
		// =========================================================

		/// <summary>
		/// 位置の設定
		/// </summary>
		/// <param name="translate"></param>
		void SetTranslate(Vector3& translate) { wt_.SetTranslate(translate); }

		/// <summary>
		/// 位置の取得
		/// </summary>
		/// <returns></returns>
		Vector3 GetTranslate() const { return wt_.GetTranslate(); }

		/// <summary>
		/// ゴール済みフラグを取得
		/// </summary>
		/// <returns></returns>
		bool IsGoaled() const { return isGoaled_; }

		/// <summary>
		/// 死亡フラグを取得
		/// </summary>
		/// <returns></returns>
		bool IsDead() const { return isDead_; }

		/// <summary>
		/// HPの取得
		/// </summary>
		/// <returns></returns>
		int GetHP() const { return hp_; }

	private:
		// =========================================================
		// Internal Methods
		// =========================================================

		/// <summary>
		/// 移動入力処理
		/// </summary>
		void Move();

		/// <summary>
		/// 無敵時間の更新処理
		/// </summary>
		void InvincibleUpdate();

		/// <summary>
		/// プレイヤーの振り向き更新
		/// </summary>
		void UpdateFacing();

	private:
		// =========================================================
		// Constants
		// =========================================================

		// 移動速度
		const float kSpeed = 0.3f;
		// 重力
		const float kGravity = -0.03f;
		// ジャンプの初速
		const float kJumpPower = 0.55f;
		// 慣性
		const float kAcceleration = 0.325f;
		// 振り向く速さ
		const float kTurnSpeed = 0.15f;

		// =========================================================
		// Member Variables
		// =========================================================

		// 入力システム
		Input* input_ = nullptr;
		// オブジェクト
		std::unique_ptr<Object3d> object_ = nullptr;
		// ワールドトランスフォーム
		WorldTransform wt_;
		// モデル表示・振り向きアニメーション用
		WorldTransform modelWorldTransform_;
		// コライダー
		std::unique_ptr<ICollider> collider_ = nullptr;

		// 速度
		Vector3 velocity_ = { 0.0f, 0.0f, 0.0f };
		// 垂直方向の速度
		float yVelocity_ = 0.0f;
		// 接地フラグ
		bool isGrounded_ = false;
		// 空中状態フラグ
		bool isAirborne_ = false;

		// ゴール済みフラグ
		bool isGoaled_ = false;

		// 死亡フラグ
		bool isDead_ = false;

		// 現在のY軸回転
		float currentRotationY_ = 0.0f;

		// 目標のY軸回転
		float targetRotationY_ = 0.0f;

		//移動時パーティクル
		std::unique_ptr<CombinedParticle> moveParticle_ = nullptr;

		// ジャンプ音
		std::unique_ptr<Norm::Audio> seJump_;
		// 着地音
		std::unique_ptr<Norm::Audio> seLand_;
		// ゴール音
		std::unique_ptr<Norm::Audio> seGoal_;

		// 無敵時間
		bool isInvincible_ = false;
		float invincibleTimer_ = 0.0f;
		int invincibleCounter_ = 0;
		Vector4 invincibleColor_ = { 1.0f, 1.0f, 1.0f, 1.0f }; // 無敵時の色（半透明）

		// HP
		int hp_ = 3;
	};
}
