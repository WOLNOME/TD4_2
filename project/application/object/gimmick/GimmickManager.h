#pragma once
#include <vector>
#include <memory>

#include "Vector3.h"
#include "GimmickBase.h"
#include "engine/3d/camera/BaseCamera.h"


using namespace Norm;


enum class GimmickType {
	None,
	Explosion,
	HomingLauncher,
	StunTrap,

};

class LightManager;

class GimmickManager {
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
	/// ライトマネージャーの設定
	/// </summary>
	/// <param name="_lightManager">ライトマネージャー</param>
	void SetLightManager(LightManager* _lightManager) { lightManager_ = _lightManager; }

private:
	std::vector<std::unique_ptr<GimmickBase>> gimmicks_;// ギミックのリスト
	LightManager* lightManager_ = nullptr;
	GimmickType gimmickType_ = GimmickType::None; // ギミックの状態
	Norm::BaseCamera* camera_ = nullptr; // カメラのポインタ

};

