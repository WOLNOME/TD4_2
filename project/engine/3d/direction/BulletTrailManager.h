#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <array>
#include <unordered_map>
#include <memory>
#include "BlendMode.h"

namespace Norm {

	class BaseCamera;
	class BulletTrail;

	/// <summary>
	/// 弾道トレール全体の管理を行うクラス
	/// シングルトンパターンで実装
	/// </summary>
	class BulletTrailManager {
	private://シングルトン
		static std::unique_ptr<BulletTrailManager> instance_;

		BulletTrailManager() = default;//コンストラクタ隠蔽
		~BulletTrailManager() = default;//デストラクタ隠蔽
		BulletTrailManager(BulletTrailManager&) = delete;//コピーコンストラクタ封印
		BulletTrailManager& operator=(BulletTrailManager&) = delete;//コピー代入演算子封印

		friend struct std::default_delete<BulletTrailManager>;
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// インスタンスの取得
		/// </summary>
		/// <returnsインスタンスreturns>
		static BulletTrailManager* GetInstance();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 描画
		/// </summary>
		void Draw();
		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();

		/// <summary>
		/// 弾丸トレールの登録
		/// </summary>
		/// <param name="name">名前</param>
		/// <param name="bulletTrail">弾丸トレール</param>
		void RegisterBulletTrail(const std::string& name, BulletTrail* bulletTrail);
		/// <summary>
		/// 弾丸トレールの取得
		/// </summary>
		/// <param name="name">名前</param>
		void DeleteBulletTrail(const std::string& name);

		/// <summary>
		/// 名前の生成
		/// </summary>
		/// <param name="name">名前</param>
		/// <returns>重複しない名前</returns>
		std::string GenerateName(const std::string& name);

		/// ============================== ///
		///		setter
		/// ============================== ///

		/// <summary>
		/// カメラのセット
		/// </summary>
		/// <param name="_camera">カメラ</param>
		void SetCamera(BaseCamera* _camera) { camera_ = _camera; }

	private:
		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// グラフィックスパイプラインの生成
		/// </summary>
		void GenerateGraphicsPipeline();

		/// ============================== ///
		///		インスタンス
		/// ============================== ///

		BaseCamera* camera_ = nullptr;

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//ルートシグネチャ
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
		//グラフィックスパイプライン
		std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, (int)BlendMode::kMaxBlendModeNum> graphicsPipelineState_;

		//弾丸トレールのコンテナ
		std::unordered_map<std::string, BulletTrail*> bulletTrails_;
	};

}