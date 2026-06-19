#include "Framework.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "GPUDescriptorManager.h"
#include "RTVManager.h"
#include "DSVManager.h"
#include "TextTextureRender.h"
#include "MainRender.h"
#include "D2DRender.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "TextTextureManager.h"
#include "PostEffectManager.h"
#include "ModelManager.h"
#include "ParticleManager.h"
#include "CombinedParticleManager.h"
#include "BulletTrailManager.h"
#include "Input.h"
#include "AudioCommon.h"
#include "SpriteManager.h"
#include "Object3dManager.h"
#include "LineManager.h"
#include "SceneManager.h"
#include "CollisionManager.h"

namespace Norm {

	void Framework::Initialize() {
		//WindowsAPIの初期化
		WinApp::GetInstance()->Initialize();

		//DirectX12
		DirectXCommon::GetInstance()->Initialize();

		//GPUDescriptorマネージャー
		GPUDescriptorManager::GetInstance()->Initialize();

		//RTVマネージャー
		RTVManager::GetInstance()->Initialize();

		//DSVマネージャー
		DSVManager::GetInstance()->Initialize();

		//テキストテクスチャレンダー
		TextTextureRender::GetInstance()->Initialize();

		//メインレンダー
		MainRender::GetInstance()->Initialize();

		//D2Dレンダー
		D2DRender::GetInstance()->Initialize();

		//ImGuiマネージャー
		ImGuiManager::GetInstance()->Initialize();

		//テクスチャマネージャー
		TextureManager::GetInstance()->Initialize();

		//テキストテクスチャマネージャー
		TextTextureManager::GetInstance()->Initialize();

		//ポストエフェクトマネージャー
		PostEffectManager::GetInstance()->Initialize();

		//モデルマネージャー
		ModelManager::GetInstance()->Initialize();

		//パーティクルマネージャー
		ParticleManager::GetInstance()->Initialize();

		//複合パーティクルマネージャー
		CombinedParticleManager::GetInstance()->Initialize();

		//弾丸トレールマネージャー
		BulletTrailManager::GetInstance()->Initialize();

		//インプット
		Input::GetInstance()->Initialize();

		//オーディオ共通部
		AudioCommon::GetInstance()->Initialize();

		//スプライトマネージャー
		SpriteManager::GetInstance()->Initialize();

		//オブジェクト3Dマネージャー
		Object3dManager::GetInstance()->Initialize();

		//線描画共通部
		LineManager::GetInstance()->Initialize();

		//シーンマネージャーの生成
		SceneManager::GetInstance()->Initialize();

		//当たり判定マネージャーの生成
		CollisionManager::GetInstance()->Initialize();

	}

	void Framework::Finalize() {
		CollisionManager::GetInstance()->Finalize();
		SceneManager::GetInstance()->Finalize();
		LineManager::GetInstance()->Finalize();
		Object3dManager::GetInstance()->Finalize();
		SpriteManager::GetInstance()->Finalize();
		AudioCommon::GetInstance()->Finalize();
		Input::GetInstance()->Finalize();
		BulletTrailManager::GetInstance()->Finalize();
		CombinedParticleManager::GetInstance()->Finalize();
		ParticleManager::GetInstance()->Finalize();
		ModelManager::GetInstance()->Finalize();
		PostEffectManager::GetInstance()->Finalize();
		TextTextureManager::GetInstance()->Finalize();
		TextureManager::GetInstance()->Finalize();
		ImGuiManager::GetInstance()->Finalize();
		D2DRender::GetInstance()->Finalize();
		MainRender::GetInstance()->Finalize();
		TextTextureRender::GetInstance()->Finalize();
		DSVManager::GetInstance()->Finalize();
		RTVManager::GetInstance()->Finalize();
		GPUDescriptorManager::GetInstance()->Finalize();
		DirectXCommon::GetInstance()->Finalize();
		WinApp::GetInstance()->Finalize();
	}

	void Framework::Update() {
		//インプット更新
		Input::GetInstance()->Update();

		//メッセージ処理
		if (WinApp::GetInstance()->ProcessMessage()) {
			isOver = true;
		}
		//シーンマネージャー更新
		SceneManager::GetInstance()->Update();
	}

	void Framework::Run() {
		//ゲームの初期化
		Initialize();
		while (true) {
			//終了リクエストが来たら抜ける
			if (GetOver()) {
				break;
			}
			//毎フレーム更新
			Update();
			//描画
			Draw();
		}
		//ゲームの終了
		Finalize();
	}

}