#pragma once
#include "GimmickBase.h"

class ExplosionGimmick : public GimmickBase
{
public:
	ExplosionGimmick() = default;
	~ExplosionGimmick() override = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;
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
};