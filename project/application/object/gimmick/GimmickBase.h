#pragma once
#include <memory>
#include "Vector3.h"
#include "WorldTransform.h"
#include "Object3d.h"
#include "application/object/collision/ObjectCollider.h"
#include "GimmickCollider.h"

#include "engine/3d/camera/BaseCamera.h"
#include "application/ui/guide/GuideUI.h"


using namespace Norm;

enum class GimmickState
{
	Hidden,      // 見つかっていない
	Discovered, // ライトで発見済み
	Active,     // 起動中
	Used,        // 使用済み
	Respawning // 再生成演出中
};

class LightManager;

class GimmickBase
{
public:
	virtual ~GimmickBase() = default;

	virtual void Initialize(Norm::BaseCamera* _camera) = 0;
	virtual void Update();

	/// <summary>
	/// ライトの円に当たっているか判定
	/// </summary>
	bool IsHitLightCircle() const;

	/// <summary>
	/// ライトに当たった時の共通処理
	/// </summary>
	void HitLight();

	/// ==============アクセッサ================ ///
	void SetLightManager(LightManager* _lightManager) { lightManager_ = _lightManager; }


	GimmickState GetGimmickState() const { return gimmickState_; }//状態を取得
	const Vector3& GetPosition() const { return position_; }
	float GetRadius() const { return radius_; }

	void CreateCollider(CollisionAttribute attribute, const Vector3& offset, const Vector3& size);


protected:
	virtual void OnLightHit() {}
	virtual void OnFlashHit() {}

	void SetColliderSize(const Vector3& size);
protected:

	LightManager* lightManager_ = nullptr;

	GimmickState gimmickState_ = GimmickState::Hidden;

	std::unique_ptr<Object3d> gimmickObject_ = nullptr;
	WorldTransform worldTransform_{};

	std::unique_ptr<GuideUI> guideUI_ = nullptr;

	Vector3 uiOffset_ = { 0.0f,4.0f,0.0f };

	Vector3 position_{};
	float radius_ = 1.0f;

	std::unique_ptr<ICollider> collider_ = nullptr;
};