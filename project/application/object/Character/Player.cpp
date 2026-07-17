#include "Player.h"

// Engine
#include <Object3dManager.h>
#include <imgui.h>
#include <CollisionManager.h>
#include <TextureManager.h>

// Application
#include <application/object/collision/ObjectCollider.h>

void Norm::Player::Initialize() {
	// インプットのインスタンス取得
	input_ = Input::GetInstance();

	// 3Dモデルの生成 + 初期化
	object_ = std::make_unique<Object3d>();
	object_->Initialize(ModelTag{}, Object3dManager::GetInstance()->GenerateName("player"), "player");
	object_->SetIsOutline(true);
	object_->SetOutlineParam(TextureManager::GetInstance()->LoadTexture("green.png"), 1.05f);
	wt_.Initialize();
	wt_.SetTranslate({0.0f, -10.0f, 0.0f});
	object_->RegistWorldTransform(&wt_);

	// コライダーの生成 + 登録
	collider_ = std::make_unique<ObjectCollider>(object_.get());
	auto* playerCollider = dynamic_cast<ObjectCollider*>(collider_.get());
	if (playerCollider) {
		playerCollider->SetCollisionAttribute(CollisionAttribute::Player);
		playerCollider->SetWorldTransform(&wt_);
		playerCollider->SetOffset({0.0f, 0.0f, 0.0f});
		playerCollider->SetOBBSize({1.0f, 2.0f, 1.0f}); // プレイヤーのコライダーサイズ
		playerCollider->SetHolder(this); // 自身のポインタをセット
	}
}

void Norm::Player::Update() {
	// 左右入力移動
	velocity_.x = 0.0f;
	if (input_->PushKey(DIK_A)) {
		velocity_.x = -kSpeed;
	}
	if (input_->PushKey(DIK_D)) {
		velocity_.x = kSpeed;
	}
	
	// 重力の計算（自由落下）
	yVelocity_ += kGravity;
	// 最大落下速度の制限（貫通防止）
	if (yVelocity_ < -0.5f) {
		yVelocity_ = -0.5f;
	}

	// ジャンプ入力
	if (input_->PushKey(DIK_W)) {
		// 接地中のみ可能
		if (isGrounded_) {
			yVelocity_ = kJumpPower;
			isGrounded_ = false;
		}
	}

	// Y軸の速度を反映
	velocity_.y = yVelocity_;
	velocity_.z = 0.0f;

	// 座標の更新
	Vector3 currentPos = wt_.GetTranslate();
	wt_.SetTranslate({ 
		currentPos.x + velocity_.x, 
		currentPos.y + velocity_.y, 
		currentPos.z + velocity_.z
	});

	// 行列の更新
	wt_.UpdateMatrix();

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

		// 接地中フラグ
		ImGui::Checkbox("IsGrounded", &isGrounded_);

		// ゴール済みフラグ
		ImGui::Checkbox("IsGoaled", &isGoaled_);

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
				// プレイヤーの座標を押し戻しベクトル分だけ戻す
				Vector3 currentPos = wt_.GetTranslate();
				wt_.SetTranslate(MyMath::Add(currentPos, pushVector));
				wt_.UpdateMatrix();
			}

			// 接地中の処理（上方向に押し戻された際）
			if (pushVector.y > 0.0f) {
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

	// 相手がゴールブロックならゴール済みフラグを立てる
	if (otherAttr == CollisionAttribute::Goal) {
		isGoaled_ = true;
	}
}