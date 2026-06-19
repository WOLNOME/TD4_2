#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <unordered_map>
#include <string>
#include <memory>

namespace Norm {

	class Sprite;

	/// <summary>
	/// 全てのスプライトを管理するクラス
	/// シングルトンパターンで実装
	/// </summary>
	class SpriteManager {
	private://コンストラクタ等の隠蔽
		static std::unique_ptr<SpriteManager> instance_;

		SpriteManager() = default;//コンストラクタ隠蔽
		~SpriteManager() = default;//デストラクタ隠蔽
		SpriteManager(SpriteManager&) = delete;//コピーコンストラクタ封印
		SpriteManager& operator=(SpriteManager&) = delete;//コピー代入演算子封印

		friend struct std::default_delete<SpriteManager>;

	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// シングルトンインスタンスの取得
		/// </summary>
		/// <returns>シングルトンインスタンス</returns>
		static SpriteManager* GetInstance();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// バック描画
		/// </summary>
		void BackDraw();
		/// <summary>
		/// フロント描画
		/// </summary>
		void FrontDraw();
		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();

		/// <summary>
		/// ImGuiデバッグ表示
		/// </summary>
		void DebugWithImGui();

		/// <summary>
		/// スプライトをコンテナに登録
		/// </summary>
		/// <param name="name">名前</param>
		/// <param name="sprite">スプライトのポインタ</param>
		void RegisterSprite(const std::string& name, Sprite* sprite);
		/// <summary>
		/// 登録されたスプライトを削除
		/// </summary>
		/// <param name="name">名前</param>
		void DeleteSprite(const std::string& name);

		/// <summary>
		/// 名前を決める関数
		/// </summary>
		/// <param name="name">名前</param>
		/// <returns>重複しない名前</returns>
		std::string GenerateName(const std::string& name);

	private:
		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// グラフィックスパイプラインの生成
		/// </summary>
		void GenerateGraphicsPipeline();

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//ルートシグネチャ
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
		//グラフィックスパイプライン
		Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;

		//スプライトのコンテナ
		std::unordered_map<std::string, Sprite*> sprites_;
	};

}
