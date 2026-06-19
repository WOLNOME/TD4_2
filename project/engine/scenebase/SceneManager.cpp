#include "SceneManager.h"
#include "SceneFactory.h"
#include "ImGuiManager.h"
#include <cassert>

namespace Norm {

	std::unique_ptr<SceneManager> SceneManager::instance_ = nullptr;

	SceneManager* SceneManager::GetInstance() {
		if (!instance_) {
			instance_ = std::unique_ptr<SceneManager>(new SceneManager());
		}
		return instance_.get();
	}

	void SceneManager::Initialize() {
		//シーンファクトリーの生成
		sceneFactory_ = std::make_unique<SceneFactory>();
		//シーン遷移アニメーションの生成
		sceneTransitionAnimation_ = std::make_unique<SceneTransitionAnimation>();
		sceneTransitionAnimation_->Initialize();

	}

	void SceneManager::Update() {
		//シーン遷移アニメーションの更新
		sceneTransitionAnimation_->Update();
		//シーン切り替え処理
		ChangeScene();
		//実行中シーンを更新する
		scene_->Update();

	}

	void SceneManager::DebugWithImGui() {
#ifdef _DEBUG
		//FPS表示
		scene_->ShowFPS();
		//デバッグ処理
		scene_->DebugWithImGui();
		//ブレークポイント
		if (Input::GetInstance()->TriggerKey(DIK_F5)) {
			int a = 0;
			a++;
		}

#endif // _DEBUG

	}

	void SceneManager::Finalize() {
		//最後のシーンの終了と解放
		scene_->Finalize();
		scene_.reset();
		//シーンファクトリー解放
		sceneFactory_.reset();
		//インスタンスを削除
		instance_.reset();
	}

	bool SceneManager::SetNextScene(const std::string& nextSceneName, SceneTransitionAnimation::Type inType, SceneTransitionAnimation::Type outType, SceneTransitionAnimation::Option option, float time, uint32_t _textureHandle, TransitionMode _transitionMode) {
		//遷移中なら何もしない
		if (sceneTransitionAnimation_->GetIsTransitioning())
			return false;

		//警告
		assert(sceneFactory_);
		assert(!nextScene_);

		//もし最初のシーンだったらここで生成＆初期化
		if (!scene_) {
			scene_ = sceneFactory_->CreateScene(nextSceneName);
			nextScene_.reset();
			scene_->Initialize();
			return true;
		}

		//遷移モードをセット
		transitionMode_ = _transitionMode;
		//遷移モードによる処理
		switch (transitionMode_) {
		case Norm::TransitionMode::Normal:
		case Norm::TransitionMode::Temporary:
		{
			//次シーンを生成
			nextScene_ = sceneFactory_->CreateScene(nextSceneName);

			break;
		}
		case Norm::TransitionMode::FromKeep:
		{
			//keepシーンがないなら警告
			assert(keepScene_&&"キープシーンが空です");

			//次シーンにキープシーンを写す
			nextScene_ = std::move(keepScene_);
			keepScene_.reset();

			break;
		}
		default:
			break;
		}

		//遷移アニメーションタイプを設定
		sceneTransitionAnimation_->SetType(inType, outType);
		//遷移アニメーションオプションを設定
		sceneTransitionAnimation_->SetOption(option);
		//遷移アニメーションも時間を設定
		sceneTransitionAnimation_->SetTime(time);
		//テクスチャを設定
		sceneTransitionAnimation_->SetTexture(_textureHandle);
		
		return true;
	}

	void SceneManager::ChangeScene() {
		//次のシーン予約があるなら
		if (nextScene_ && !sceneTransitionAnimation_->GetIsTransitioning()) {
			//遷移アニメーション開始
			sceneTransitionAnimation_->StartTransition();
		}
		//遷移アニメーション中なら
		if (sceneTransitionAnimation_->GetState() == SceneTransitionAnimation::State::UPDATE_IN) {
			//フェードイン処理
			sceneTransitionAnimation_->UpdateIn();
		}
		else if (sceneTransitionAnimation_->GetState() == SceneTransitionAnimation::State::END_IN) {
			//フェードイン終了
			sceneTransitionAnimation_->EndIn();

			//遷移モードによる処理
			switch (transitionMode_) {
			case Norm::TransitionMode::Normal:
			{
				//旧シーンの終了
				if (scene_) {
					scene_->Finalize();
					scene_.reset();
				}
				//シーンの切り替え
				scene_ = std::move(nextScene_);
				nextScene_.reset();
				//次のシーンを初期化する
				scene_->Initialize();

				break;
			}
			case Norm::TransitionMode::Temporary:
			{
				//旧シーンはキープシーンに保存
				keepScene_.reset();
				keepScene_ = std::move(scene_);
				scene_.reset();
				//シーンの切り替え
				scene_ = std::move(nextScene_);
				nextScene_.reset();
				//次のシーンを初期化する
				scene_->Initialize();

				break;
			}
			case Norm::TransitionMode::FromKeep:
			{
				//旧シーンの終了
				if (scene_) {
					scene_->Finalize();
					scene_.reset();
				}
				//シーンの切り替え
				scene_ = std::move(nextScene_);
				nextScene_.reset();

				//次のシーンの復帰時処理を行う
				scene_->OnResume();

				break;
			}
			default:
				break;
			}
		}
		else if (sceneTransitionAnimation_->GetState() == SceneTransitionAnimation::State::UPDATE_OUT) {
			//フェードアウト処理
			sceneTransitionAnimation_->UpdateOut();
		}
		else if (sceneTransitionAnimation_->GetState() == SceneTransitionAnimation::State::END_OUT) {
			//フェードアウト終了
			sceneTransitionAnimation_->EndOut();
		}
		else if (sceneTransitionAnimation_->GetState() == SceneTransitionAnimation::State::END_ALL) {
			//フェードアウト終了
			sceneTransitionAnimation_->EndAll();
		}
	}

}