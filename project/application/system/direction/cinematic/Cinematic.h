#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

//アプリケーション
#include "application/system/direction/shot/base/IShotBase.h"

//前方宣言（エンジン層）
namespace Norm {
	class BlackBoard;
}

/// <summary>
/// 映像演出管理用クラス
/// </summary>
class Cinematic {
public:
	/// ============================== ///
	///		メンバ関数
	/// ============================== ///

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Cinematic();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Cinematic();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const std::string& _fileName, Norm::BlackBoard* _blackBoard);
	/// <summary>
	/// 初期化
	/// </summary>
	void Update();
	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug();

	/// ============================== ///
	///		getter
	/// ============================== ///

	/// <summary>
	/// 全てのショットが終了したかを取得
	/// </summary>
	/// <returns>全てのショットが終了したか</returns>
	bool GetIsAllShotsFinished() { return isAllShotsFinished_; }

private:
	/// ============================== ///
	///		メンバ変数
	/// ============================== ///

	//ショットのコンテナ
	std::vector<std::unique_ptr<IShotBase>> shots_;

	//現在のショット番号
	uint32_t currentIndex_ = 0u;

	//全てのショットが終わったかどうか
	bool isAllShotsFinished_ = false;



};

