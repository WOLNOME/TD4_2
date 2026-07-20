#pragma once
#include "GimmickBase.h"
#include <ICollider.h>
#include "Vector3.h"
#include "CombinedParticle.h"

using namespace Norm;


class ExplosionGimmick : public GimmickBase
{
public:
	
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Norm::BaseCamera* _camera) override;

	/// <summary>
	///	更新
	/// </summary>
	void Update() override;


	/// <summary>
	/// 爆発をリセット
	/// </summary>
	void Reset();
	/// <summary>
	/// ImGui表示
	/// </summary>
	void DebugImGui();

	/// <summary>
	/// 座標のセット
	/// </summary>
	/// <param name="position"></param>
	void SetPosition(const Vector3& position) { position_ = position; }
	/// <summary>
	/// 爆発したかどうかの判定
	/// </summary>
	/// <returns></returns>
	bool IsExploded() const { return isExploded_; }



	const char* GetStateName() const
	{
		switch (gimmickState_) {
		case GimmickState::Hidden:
			return "Hidden";
		case GimmickState::Discovered:
			return "Discovered";
		case GimmickState::Active:
			return "Active";
		case GimmickState::Used:
			return "Used";
		default:
			return "Unknown";
		}
	}



protected:
	void OnFlashHit() override;

private:
	bool isExploded_ = false;

	// 爆発演出用
	float explosionTimer_ = 0.0f;
	float explosionDuration_ = 0.5f;
	float explosionScale_ = 1.0f;
	float maxExplosionScale_ = 5.0f;

	// 再出現用
	float respawnTimer_ = 0.0f;
	float respawnDuration_ = 3.0f;

	// 爆発前のコライダーサイズ
	Vector3 baseColliderSize_;


	//爆発パーティクル
	std::unique_ptr<CombinedParticle> explosionParticle_ = nullptr;

};