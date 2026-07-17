#pragma once
#include <Vector3.h>

namespace Norm {
	class PointLight;
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



	// =========================================================
	// setter
	// =========================================================

	/// <summary>
	/// ポイントライトの設定
	/// </summary>
	/// <param name="_pointLight">ポイントライト</param>
	void SetPointLight(Norm::PointLight* _pointLight) { pointLight_ = _pointLight; }


private:
	Norm::PointLight* pointLight_ = nullptr;

	bool isLighting_ = false;
	bool isFlush_ = false;

	const float kMaxIntensity = 6.0f;		//フラッシュ時最大の輝度
	const float kNormalIntensity = 3.0f;	//通常時の輝度
	const float kMaxRange = 12.0f;			//フラッシュ時最大の範囲
	const float kNormalRange = 8.0f;		//通常時の範囲

	const float kFlushKeepTime = 2.0f;
	float flushKeepTimer_ = 0.0f;



};

