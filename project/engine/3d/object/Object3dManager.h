#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <array>
#include <unordered_map>
#include <memory>
#include "ObjectEnum.h"

namespace Norm {

	class BaseCamera;
	class SceneLight;
	class Object3d;

	/// <summary>
	/// 全ての3Dオブジェクトの管理を行うクラス
	/// シングルトンパターンで実装
	/// </summary>
	class Object3dManager {
	private:
		static std::unique_ptr<Object3dManager> instance_;

		Object3dManager() = default;//コンストラクタ隠蔽
		~Object3dManager() = default;//デストラクタ隠蔽
		Object3dManager(Object3dManager&) = delete;//コピーコンストラクタ封印
		Object3dManager& operator=(Object3dManager&) = delete;//コピー代入演算子封印

		friend struct std::default_delete<Object3dManager>;
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// シングルトンインスタンスの取得
		/// </summary>
		/// <returns>シングルトンインスタンス</returns>
		static Object3dManager* GetInstance();

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
		/// オブジェクトをコンテナに登録
		/// </summary>
		/// <param name="name">名前</param>
		/// <param name="object">オブジェクト</param>
		void RegisterObject(const std::string& name, Object3d* object);
		/// <summary>
		/// 登録されたオブジェクトを削除
		/// </summary>
		/// <param name="name">名前</param>
		void DeleteObject(const std::string& name);

		/// <summary>
		/// 名前を決める関数
		/// </summary>
		/// <param name="name">名前</param>
		/// <returns>重複しない名前</returns>
		std::string GenerateName(const std::string& name);

		/// <summary>
		/// 共通のグラフィックスパイプライン設定
		/// </summary>
		/// <param name="indexNG">NameGPSの番号</param>
		/// <param name="indexOK">StencilRoleの番号</param>
		void SettingCommonDrawing(NameGPS index = NameGPS::Normal, StencilRole indexSR = StencilRole::Normal);
		/// <summary>
		/// アニメーション用のグラフィックスパイプライン設定
		/// </summary>
		void SettingAnimationCS();

		/// ============================== ///
		///		setter
		/// ============================== ///

		/// <summary>
		/// カメラのセット
		/// </summary>
		/// <param name="_camera">カメラ</param>
		void SetCamera(BaseCamera* _camera) { camera_ = _camera; }
		/// <summary>
		/// シーンライトのセット
		/// </summary>
		/// <param name="_light">シーンライト</param>
		void SetSceneLight(SceneLight* _light) { light_ = _light; }

	private:
		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// グラフィックスパイプラインの生成
		/// </summary>
		void GenerateGraphicsPipeline();
		/// <summary>
		/// コンピュートパイプラインの生成
		/// </summary>
		void GenerateComputePipeline();

		/// <summary>
		/// PSO設定
		/// </summary>
		void SettingPSO();

		/// ============================== ///
		///		インスタンス
		/// ============================== ///

		BaseCamera* camera_ = nullptr;
		SceneLight* light_ = nullptr;

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//ルートシグネチャ
		std::array<Microsoft::WRL::ComPtr<ID3D12RootSignature>, (int)NameGPS::kMaxNumNameGPS> rootSignature_;
		//グラフィックスパイプライン
		std::array<
			std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>,
			(int)StencilRole::kMaxNumStencilRole>,
			(int)NameGPS::kMaxNumNameGPS>
			graphicsPipelineState_;

		//コンピュートルートシグネチャ
		Microsoft::WRL::ComPtr<ID3D12RootSignature> computeRootSignature_ = nullptr;
		//コンピュートパイプライン
		Microsoft::WRL::ComPtr<ID3D12PipelineState> computePipelineState_ = nullptr;

		//オブジェクトのコンテナ
		std::unordered_map<std::string, Object3d*> objects_;
	};

}