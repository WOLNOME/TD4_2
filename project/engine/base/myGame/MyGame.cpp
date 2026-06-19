#include "MyGame.h"
#include "DirectXCommon.h"
#include "MainRender.h"
#include "D2DRender.h"
#include "PostEffectManager.h"
#include "TextureManager.h"
#include "GPUDescriptorManager.h"
#include "RTVManager.h"
#include "DSVManager.h"
#include "TextTextureManager.h"
#include "ImGuiManager.h"
#include "SpriteManager.h"
#include "Object3dManager.h"
#include "LineManager.h"
#include "ParticleManager.h"
#include "CombinedParticleManager.h"
#include "BulletTrailManager.h"
#include "CollisionManager.h"
#include "SceneManager.h"

using namespace Norm;

void MyGame::Initialize() {
	//ゲーム基盤部の初期化
	Framework::Initialize();

	//シーンマネージャーに最初のシーンをセット
	SceneManager::GetInstance()->SetNextScene("Sample");

	//パーティクルエディター→PARTICLEEDITOR
	//サンプルシーン→Sample
}

void MyGame::Finalize() {
	//ゲーム基盤解放
	Framework::Finalize();
}

void MyGame::Update() {
	//ImGui受付開始
	ImGuiManager::GetInstance()->Begin();

	//ゲーム基盤更新
	Framework::Update();

	//シーンのデバッグ処理
	SceneManager::GetInstance()->DebugWithImGui();

	//スプライトマネージャーの更新
	SpriteManager::GetInstance()->Update();
	//オブジェクトマネージャーの更新
	Object3dManager::GetInstance()->Update();
	//複合パーティクルマネージャーの更新
	CombinedParticleManager::GetInstance()->Update();
	//パーティクルマネージャーの更新
	ParticleManager::GetInstance()->Update();
	//弾丸マネージャーの更新
	BulletTrailManager::GetInstance()->Update();

	//当たり判定のチェック
	CollisionManager::GetInstance()->CheckCollision();

	//ImGuiの内部コマンドを生成する
	ImGuiManager::GetInstance()->End();
}

void MyGame::Draw() {
	///------------------------------///
	///   テキストテクスチャの生成処理
	///------------------------------///

	//テキストテクスチャ描画前処理
	GPUDescriptorManager::GetInstance()->SetDescriptorHeap(TextTextureRender::GetInstance()->GetCommandList());

	//文字をD2D描画でテクスチャに書き込む
	TextTextureManager::GetInstance()->WriteTextOnD2D();
	//文字の装飾をD3D12で行う
	TextTextureManager::GetInstance()->DrawDecorationOnD3D12();

	//テキストテクスチャ描画後処理
	TextTextureRender::GetInstance()->PostDraw();
	DirectXCommon::GetInstance()->PostEachRender();			//GPUの実行を待つ
	TextTextureRender::GetInstance()->ReadyNextCommand();	//TextTextureRenderで使用したコマンドをリセット

	///------------------------------///
	///        D3D12の描画処理
	///------------------------------///

	//オブジェクト描画前処理
	PostEffectManager::GetInstance()->PreObjectDraw();
	MainRender::GetInstance()->PreObjectDraw();
	GPUDescriptorManager::GetInstance()->SetDescriptorHeap(MainRender::GetInstance()->GetCommandList());

	//シーンのバックスプライト描画
	SpriteManager::GetInstance()->BackDraw();
	//シーンのオブジェクト描画
	Object3dManager::GetInstance()->Draw();
	//シーンのライン描画
	LineManager::GetInstance()->Draw();
	//シーンのパーティクル描画
	ParticleManager::GetInstance()->Draw();
	//シーンの弾丸トレール描画
	BulletTrailManager::GetInstance()->Draw();
	//シーンのフロントスプライト描画
	SpriteManager::GetInstance()->FrontDraw();

	//ImGui描画前処理
	MainRender::GetInstance()->PreImGuiDraw();
	PostEffectManager::GetInstance()->CopySceneToRenderTexture();
	//ImGuiの描画
	ImGuiManager::GetInstance()->Draw();

	//描画後処理
	TextTextureManager::GetInstance()->ReadyNextResourceState();		//MainRenderの描画が終了した時点でtextTextureResourceのステートを遷移
	MainRender::GetInstance()->PostDraw();		//GPUにMainRenderの描画処理を投げる
	DirectXCommon::GetInstance()->PostEachRender();		//GPUの実行を待つ
	MainRender::GetInstance()->ReadyNextCommand();		//MainRenderで使用したコマンドをリセット

	///------------------------------///
	///        D2Dの描画処理
	///------------------------------///

	//D2Dの描画前処理
	D2DRender::GetInstance()->PreDraw();

	//D2Dの描画後処理
	D2DRender::GetInstance()->PostDraw();

	///------------------------------///
	///        全ての描画が終了
	///------------------------------///

	//画面切り替え
	MainRender::GetInstance()->ExchangeScreen();
	
	///------------------------------///
	///      レンダーの最終処理
	///------------------------------///

	//全レンダー終了時の共通処理
	DirectXCommon::GetInstance()->PostAllRenders();

	//デスクリプタヒープ解放済みインデックスを使用可能状態にする
	GPUDescriptorManager::GetInstance()->TransferEnable();
	RTVManager::GetInstance()->TransferEnable();
	DSVManager::GetInstance()->TransferEnable();

}

