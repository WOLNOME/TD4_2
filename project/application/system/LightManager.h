#pragma once
#include <Vector3.h>

namespace Norm {
	class PointLight;
	class BaseCamera;
}

/// <summary>
/// ライトの情報
/// </summary>
class LightManager {
public:
	// =========================================================
	// メンバ関数
	// =========================================================

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug();

	// =========================================================
	// getter
	// =========================================================

	/// <summary>
	/// ポイントライトの取得
	/// </summary>
	/// <returns></returns>
	const Norm::PointLight* GetPointLight() { return pointLight_; }

	/// <summary>
	/// フラッシュ中フラグの取得
	/// </summary>
	/// <returns></returns>
	bool GetIsFlush() { return isFlush_; }

	// =========================================================
	// setter
	// =========================================================

	/// <summary>
	/// ポイントライトの設定
	/// </summary>
	/// <param name="_pointLight">ポイントライト</param>
	void SetPointLight(Norm::PointLight* _pointLight) {
		pointLight_ = _pointLight;
	}

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="_camera">カメラ</param>
	void SetCamera(Norm::BaseCamera* _camera) {
		camera_ = _camera;
	}


private:
	Norm::PointLight* pointLight_ = nullptr;
	Norm::BaseCamera* camera_ = nullptr;

	bool isFlush_ = false;

	const float kMaxIntensity = 6.0f;		//フラッシュ時最大の輝度
	const float kNormalIntensity = 2.0f;	//通常時の輝度
	const float kMaxRange = 14.0f;			//フラッシュ時最大の範囲
	const float kNormalRange = 6.0f;		//通常時の範囲

	const float kFlushKeepTime = 0.6f;
	float flushKeepTimer_ = 0.0f;

	const float kFlushCoolTime = 2.0f;
	float flushCoolTimer_ = 0.0f;


};

