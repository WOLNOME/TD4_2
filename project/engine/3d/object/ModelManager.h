#pragma once
#include <map>
#include <string>
#include <memory>
#include "Model.h"
#include "ModelFormat.h"

namespace Norm {

	/// <summary>
	/// 外部リソース(3Dモデルデータ)の管理を行うクラス
	/// シングルトンパターンで実装
	/// </summary>
	class ModelManager {
	private://コンストラクタ等の隠蔽
		static std::unique_ptr<ModelManager> instance_;

		ModelManager() = default;//コンストラクタ隠蔽
		~ModelManager() = default;//デストラクタ隠蔽
		ModelManager(ModelManager&) = delete;//コピーコンストラクタ封印
		ModelManager& operator=(ModelManager&) = delete;//コピー代入演算子封印

		friend struct std::default_delete<ModelManager>;
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		///	シングルトンインスタンスの取得
		/// </summary>
		/// <returns>シングルトンインスタンス</returns>
		static ModelManager* GetInstance();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();

		/// <summary>
		/// モデルデータ読み込み関数
		/// </summary>
		/// <param name="filePath">ファイルパス</param>
		/// <param name="format">読み込むモデルの形式</param>
		void LoadModel(const std::string& filePath, ModelFormat format = OBJ);
		/// <summary>
		/// モデルデータ検索関数
		/// </summary>
		/// <param name="filePath">ファイルパス</param>
		/// <returns>モデルデータ</returns>
		Model* FindModel(const std::string& filePath);

	private:
		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//モデルデータ
		std::map<std::string, std::unique_ptr<Model>> models_;

	};

}