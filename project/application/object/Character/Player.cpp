#include "Player.h"

// Engine
#include <CollisionManager.h>
#include <Object3dManager.h>
#include <TextureManager.h>
#include <imgui.h>
#include <CombinedParticleManager.h>
#include <numbers>

// Application
#include <application/object/collision/ObjectCollider.h>

void Norm::Player::Initialize() {
	// インプットのインスタンス取得
	input_ = Input::GetInstance();

	// 3Dモデルの生成 + 初期化
	object_ = std::make_unique<Object3d>();
	object_->Initialize(ModelTag{}, Object3dManager::GetInstance()->GenerateName("player"), "player");
	object_->SetIsOutline(true);
	object_->SetOutlineParam(TextureManager::GetInstance()->LoadTexture("green.png"), 1.1f);
	wt_.Initialize();
	/*wt_.SetTranslate({0.0f, -52.0f, 0.0f});*/

	// 最初は右向き
	currentRotationY_ = 0.0f;
	targetRotationY_ = 0.0f;
	wt_.SetRotate({ 0.0f, currentRotationY_, 0.0f });

	object_->RegistWorldTransform(&wt_);

	// コライダーの生成 + 登録
	collider_ = std::make_unique<ObjectCollider>(object_.get());
	auto* playerCollider = dynamic_cast<ObjectCollider*>(collider_.get());
	if (playerCollider) {
		playerCollider->SetCollisionAttribute(CollisionAttribute::Player);
		playerCollider->SetWorldTransform(&wt_);
		playerCollider->SetOffset({0.0f, 0.0f, 0.0f});
		playerCollider->SetOBBSize({1.0f, 2.0f, 1.0f}); // プレイヤーのコライダーサイズ
		playerCollider->SetHolder(this);                // 自身のポインタをセット
	}

	//移動時パーティクル
	moveParticle_ = std::make_unique<CombinedParticle>();
	moveParticle_->Initialize(CombinedParticleManager::GetInstance()->GenerateName("move"), "grain");
	moveParticle_->SetIsPlay(true);
	moveParticle_->SetIsRepeat(true);


	// SE読み込み
	seJump_ = std::make_unique<Norm::Audio>();
	seJump_->Initialize("jump.wav");

	seLand_ = std::make_unique<Norm::Audio>();
	seLand_->Initialize("land.wav");

	seGoal_ = std::make_unique<Norm::Audio>();
	seGoal_->Initialize("goal.wav");
}

void Norm::Player::Update() {
	// 移動入力処理
	Move();

	// 振り向き更新
	UpdateFacing();

	// 無敵時間の更新
	InvincibleUpdate();

	// 重力の計算（自由落下）
	yVelocity_ += kGravity;
	// 最大落下速度の制限（貫通防止）
	if (yVelocity_ < -1.0f) {
		yVelocity_ = -1.0f;
	}

	// Y軸の速度を反映
	velocity_.y = yVelocity_;
	velocity_.z = 0.0f;

	// 座標の更新
	Vector3 currentPos = wt_.GetTranslate();
	wt_.SetTranslate({currentPos.x + velocity_.x, currentPos.y + velocity_.y, currentPos.z + velocity_.z});

	// 行列の更新
	wt_.UpdateMatrix();

	// Colorの更新
	object_->SetColor(invincibleColor_);

	// 空中フラグをオン
	if (!isGrounded_) {
		isAirborne_ = true;
	}

	//移動時パーティクルを付ける
	TransformEuler transform = {
		{1,1,1},
		{0,0,0},
		wt_.GetTranslate()
	};
	moveParticle_->SetBaseTransform(transform);

	// 接地フラグを毎フレーム最後にリセット
	isGrounded_ = false;
}

void Norm::Player::Debug() {
#ifdef _DEBUG
	if (ImGui::Begin("Player")) {
		// 現在の値を取得
		Vector3 playerTranslate = wt_.GetTranslate();

		// ImGuiで扱えるようfloatの配列で管理
		float translate[3] = {playerTranslate.x, playerTranslate.y, playerTranslate.z};

		// DragFloat3で編集
		bool isChanged = false;
		if (ImGui::DragFloat3("Translate", translate, 0.1f)) {
			isChanged = true;
		}

		// 値に変更があった場合のみ、Setterでカメラに書き戻す
		if (isChanged) {
			wt_.SetTranslate({translate[0], translate[1], translate[2]});
		}

		ImGui::Text("HP: %d", hp_);

		// 接地中フラグ
		ImGui::Checkbox("IsGrounded", &isGrounded_);

		// ゴール済みフラグ
		ImGui::Checkbox("IsGoaled", &isGoaled_);

		// 無敵フラグ
		ImGui::Checkbox("IsInvincible", &isInvincible_);
		ImGui::DragFloat("InvincibleColor.w", &invincibleColor_.w, 0.01f, 0.0f, 1.0f);

		// コライダーデバッグ
		if (collider_) {
			auto* playerCollider = dynamic_cast<ObjectCollider*>(collider_.get());
			if (playerCollider) {
				playerCollider->Debug();
			}
		}
	}
	ImGui::End();
#endif
}

void Norm::Player::OnCollision(ICollider* other, CollisionAttribute otherAttr) {
	// 相手がマップ（ブロック）の場合のみ押し戻し処理を行う（エリアブロックも含めて）
	if (otherAttr == CollisionAttribute::Block || otherAttr == CollisionAttribute::Area) {
		// お互いのコライダーを取得
		auto* playerCollider = dynamic_cast<ObjectCollider*>(this->collider_.get());
		auto* blockCollider = dynamic_cast<ObjectCollider*>(other);

		if (playerCollider && blockCollider) {
			OBB playerOBB = playerCollider->GetOBB();
			OBB blockOBB = blockCollider->GetOBB();

			Vector3 pushVector = {0.0f, 0.0f, 0.0f};

			if (MyMath::CalculatePushVector(playerOBB, blockOBB, &pushVector)) {
				// ブロック吸い付き対策
				// 上方向の押し戻しであり、かつ落下中の場合のみ天面への接地とみなす
				if (pushVector.y > 0.0f && pushVector.y >= std::abs(pushVector.x) && yVelocity_ < 0.0f) {
					pushVector.x = 0.0f;
				}
				// 天井へ頭突きした際
				else if (pushVector.y < 0.0f && std::abs(pushVector.y) >= std::abs(pushVector.x)) {
					pushVector.x = 0.0f;
				}
				// それ以外は側面衝突として処理する
				else {
					pushVector.y = 0.0f;

					if ((pushVector.x > 0.0f && velocity_.x < 0.0f) || (pushVector.x < 0.0f && velocity_.x > 0.0f)) {
						velocity_.x = 0.0f;
					}
				}
				// プレイヤーの座標を押し戻しベクトル分だけ戻す
				Vector3 currentPos = wt_.GetTranslate();
				wt_.SetTranslate(MyMath::Add(currentPos, pushVector));
				wt_.UpdateMatrix();
			}

			// 接地中の処理（上方向に押し戻された際）
			if (pushVector.y > 0.0f) {
				// 空中から着地した瞬間だけ再生
				if (isAirborne_) {
					seLand_->Play(false, 0.5f);
					isAirborne_ = false; // 着地したので戻す
				}

				isGrounded_ = true;
				yVelocity_ = 0.0f;
			}
			// 天井に頭をぶつけた際（下方向に押し戻された場合）
			else if (pushVector.y < 0.0f) {
				if (yVelocity_ > 0.0f) {
					yVelocity_ = 0.0f;
				}
			}
		}
	}

	/// ===Enemy=== ///
	if (otherAttr == CollisionAttribute::Enemy) {
		if (!isInvincible_) {
			// HPを減らす
			//hp_--;

			if (hp_ <= 0) {
				// HPが0になったら死亡フラグを立てる
				isDead_ = true;
			} else {
				// HPが残っている場合は無敵時間を開始する
				isInvincible_ = true;
				// 点滅速度を設定
				invincibleTimer_ = kDeltaTime * 4.0f;
			}
		}
	}

	// 相手がゴールブロックならゴール済みフラグを立てる
	if (otherAttr == CollisionAttribute::Goal) {
		if (!isGoaled_) {
			// ゴール音再生
			seGoal_->Play(false, 0.5f);

			isGoaled_ = true;
		}
	}
}

void Norm::Player::Move() {
	// 目標とするX方向の速度
	float targetVelocityX = 0.0f;

	// ゴールしていない場合のみキー入力を受け付ける
	if (!IsGoaled() && !IsDead()) {
		// 左右入力移動
		if (input_->PushKey(DIK_A)) {
			targetVelocityX = -kSpeed;
			// 左向き
			targetRotationY_ = std::numbers::pi_v<float>;
		}
		if (input_->PushKey(DIK_D)) {
			targetVelocityX = kSpeed;

			// 右向き
			targetRotationY_ = 0.0f;
		}

		// ジャンプ入力
		if (input_->TriggerKey(DIK_W) || input_->TriggerKey(DIK_SPACE)) {
			// 接地中のみ可能
			if (isGrounded_) {
				yVelocity_ = kJumpPower;
				isGrounded_ = false;
				isAirborne_ = true;

				// ジャンプ音再生
				seJump_->Play(false, 0.5f);
			}
		}
	}

	// 現在の速度を目標速度に近づける
	velocity_.x = MyMath::Lerp(velocity_.x, targetVelocityX, kAcceleration);
	// 微小な値になったら完全に停止させる
	if (std::abs(velocity_.x) < 0.001f) {
		velocity_.x = 0.0f;
	}

}

///-------------------------------------------/// 
/// 無敵時間の更新処理
///-------------------------------------------///
void Norm::Player::InvincibleUpdate() {
	// 無敵時間中でなければ処理しない
	if (!isInvincible_) return;

	// 透明度を減少させる
	invincibleColor_.w -= invincibleTimer_;

	// 無敵時間中のカラー変化（点滅）
	if (invincibleColor_.w <= 0.3f) {
		invincibleTimer_ *= -1.0f;
	} else if (invincibleColor_.w > 1.0f){
		invincibleTimer_ *= -1.0f;
		invincibleCounter_++;
	}

	// 無敵時間が終了したかどうかをチェック
	if (invincibleCounter_ >= 5) {
		// 無敵時間終了dw
		invincibleColor_.w = 1.0f;
		invincibleCounter_ = 0;
		isInvincible_ = false;
	}
}

void Norm::Player::UpdateFacing()
{
	// 現在角度を目標角度へ近づける
	currentRotationY_ =
		MyMath::Lerp(currentRotationY_, targetRotationY_, kTurnSpeed);

	// ほぼ目標角度なら完全に一致させる
	if (std::abs(targetRotationY_ - currentRotationY_) < 0.001f) {
		currentRotationY_ = targetRotationY_;
	}

	wt_.SetRotate({
		0.0f,
		currentRotationY_,
		0.0f
		});
}
