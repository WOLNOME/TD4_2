#pragma once
#include <vector>
#include <memory>

#include "Vector3.h"
#include "GimmickBase.h"
#include "engine/3d/camera/BaseCamera.h"


using namespace Norm;


	enum class GimmickType
	{
		None,
		Explosion,
		HomingLauncher,
		StunTrap,

	};
class GimmickManager
{


public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Norm::BaseCamera* _camera);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 全ギミックのデバッグ表示
	/// </summary>
	void Debug();


	/// <summary>
	/// ギミックの生成
	/// </summary>
	void CreateGimmick(
		GimmickType type,
		const Vector3& position
	);

	/// <summary>
	/// ライト情報を更新
	/// </summary>
	void SetLightInfo(const LightInfo& lightInfo)
	{
		lightInfo_ = lightInfo;
	}

private:
	std::vector<std::unique_ptr<GimmickBase>> gimmicks_;// ギミックのリスト
	LightInfo lightInfo_{}; // ライト情報
	GimmickType gimmickType_ = GimmickType::None; // ギミックの状態
	Norm::BaseCamera* camera_ = nullptr; // カメラのポインタ

};

