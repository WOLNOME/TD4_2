#pragma once
#include "BaseScene.h"
#include "DevelopCamera.h"

//アプリケーション
#include "application/object/environment/Skydome.h"
#include "application/object/environment/Ground.h"

namespace Norm {

	/// <summary>
	/// サンプルシーン　描画システムの使い方とか参考にしてもらうためのシーン
	/// </summary>
	class SampleScene : public BaseScene {
	public:
		/// ============================== ///
		///		メンバ関数
		///	============================== ///

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize() override;
		/// <summary>
		/// 終了時
		/// </summary>
		void Finalize() override;
		/// <summary>
		/// 更新
		/// </summary>
		void Update() override;
		/// <summary>
		/// デバッグ処理
		/// </summary>
		void DebugWithImGui() override;

	private:
		/// ============================== ///
		///		メンバ関数
		///	============================== ///

		Input* input_ = nullptr;
		//開発用カメラ
		std::unique_ptr<DevelopCamera> camera_ = nullptr;
		Vector3 cameraTranslate = { 0.0f,1.0f,-40.0f };
		Vector3 cameraRotate = { 0.0f,0.0f,0.0f };

		//平行光源
		std::unique_ptr<DirectionalLight> dirLight;

		//スプライト
		uint32_t textureHandleSprite_ = 0u;
		std::unique_ptr<Sprite> sprite_ = nullptr;

		//3Dオブジェクト
		std::unique_ptr<Object3d> modelBase_ = nullptr;
		WorldTransform modelBaseWT_;
		std::unique_ptr<Object3d> shapeBase_ = nullptr;
		WorldTransform shapeBaseWT_;
		//天球・地面
		std::unique_ptr<Skydome> skydome_ = nullptr;
		std::unique_ptr<Ground> ground_ = nullptr;
		
		//パーティクル(Particleクラスがあるけど使わないで！)
		std::unique_ptr<CombinedParticle> particle_ = nullptr;

		//オーディオ
		bool isDrawSphere_ = false;
		std::unique_ptr<Audio> audio_ = nullptr;
		float volume = 0.5f;

		//テキスト
		
	};

}

