#pragma once
#include <JsonUtil.h>
#include <Vector3.h>
#include <list>
#include <string>
#include <memory>

//アプリケーション

/// <summary>
/// レベルデータの読み込みと管理を行うクラス
/// </summary>
class LevelLoader {
private:
	/// ============================== ///
	///		構造体
	/// ============================== ///

	/// <summary>
	/// レベルデータ
	/// </summary>
	struct LevelData {

	};

public:
	/// ============================== ///
	///		メンバ関数
	/// ============================== ///

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_filePath">レベルデータのファイルパス</param>
	void Initialize(const std::string& _filePath);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// ImGui操作
	/// </summary>
	void DebugWithImGui();

	/// ============================== ///
	///		getter
	/// ============================== ///

	

private:
	/// ============================== ///
	///		非公開メンバ関数
	/// ============================== ///

	/// <summary>
	/// レベルデータの読み込み
	/// </summary>
	/// <param name="object">オブジェクトデータの入っているjson型変数</param>
	void ScanObjectData(json& object);

	/// ============================== ///
	///		メンバ変数
	/// ============================== ///

	//レベルデータ
	LevelData levelData_;

};
