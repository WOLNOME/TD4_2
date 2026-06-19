#pragma once
#include <MyMath.h>
#include "BlackBoard.h"

/// <summary>
/// ショットの結果
/// </summary>
enum class ShotResult {
	Idle,		//待機中
	Running,	//実行中
	Finish		//終了
};

/// <summary>
/// ショットのインターフェース
/// </summary>
class IShotBase {
public:
	/// ============================== ///
	///		メンバ関数
	/// ============================== ///

	/// <summary>
	/// コンストラクタ
	/// </summary>
	IShotBase(int _index, Norm::TransformEuler _startCameraTransform, Norm::TransformEuler _endCameraTransform, float _duration, Norm::BlackBoard* _blackBoard) :index_(_index), startCameraTransform_(_startCameraTransform), endCameraTransform_(_endCameraTransform), duration_(_duration), blackBoard_(_blackBoard) {};
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IShotBase() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() {};
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() {};
	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize() {};
	/// <summary>
	/// デバッグ
	/// </summary>
	virtual void Debug() {};

	/// ============================== ///
	///		getter
	/// ============================== ///

	/// <summary>
	/// ショットの結果を取得
	/// </summary>
	/// <returns>ショットの結果</returns>
	ShotResult GetShotResult() const { return shotResult_; }

protected:
	/// ============================== ///
	///		メンバ変数
	/// ============================== ///

	//番号
	int index_ = -1;
	//スタートカメラトランスフォーム
	Norm::TransformEuler startCameraTransform_ = {};
	//エンドカメラトランスフォーム
	Norm::TransformEuler endCameraTransform_ = {};
	//ショットの尺
	float duration_ = 0.0f;
	//ショットのタイマー
	float elapsedTimer_ = 0.0f;
	//ブラックボード
	Norm::BlackBoard* blackBoard_ = nullptr;
	//ショットの結果
	ShotResult shotResult_ = ShotResult::Idle;

};

