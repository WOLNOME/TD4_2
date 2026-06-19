#pragma once
#include <string>
#include <unordered_map>
#include <memory>

namespace Norm {

	class CombinedParticle;

	/// <summary>
	/// 全ての複合パーティクルを管理するクラス
	/// シングルトンパターンで実装
	/// 主に更新の一括処理
	/// </summary>
	class CombinedParticleManager {
	private://コンストラクタ等の隠蔽
		static std::unique_ptr<CombinedParticleManager> instance_;

		CombinedParticleManager() = default;//コンストラクタ隠蔽
		~CombinedParticleManager() = default;//デストラクタ隠蔽
		CombinedParticleManager(CombinedParticleManager&) = delete;//コピーコンストラクタ封印
		CombinedParticleManager& operator=(CombinedParticleManager&) = delete;//コピー代入演算子封印

		friend struct std::default_delete<CombinedParticleManager>;
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// シングルトンインスタンスの取得
		/// </summary>
		/// <returns>シングルトンインスタンス</returns>
		static CombinedParticleManager* GetInstance();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();

		/// <summary>
		/// 複合パーティクルをコンテナに登録
		/// </summary>
		/// <param name="name">名前</param>
		/// <param name="particle">パーティクルのポインタ</param>
		void Regist(const std::string& name, CombinedParticle* particle);
		/// <summary>
		/// 登録されたパーティクルを削除
		/// </summary>
		/// <param name="name">名前</param>
		void Delete(const std::string& name);

		/// <summary>
		/// 名前を決める関数
		/// </summary>
		/// <param name="name">名前</param>
		/// <returns>重複しない名前</returns>
		std::string GenerateName(const std::string& name);

	private:

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//複合パーティクルのコンテナ
		std::unordered_map<std::string, CombinedParticle*> particles;

	};

}