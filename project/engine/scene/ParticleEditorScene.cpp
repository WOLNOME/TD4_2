#include "ParticleEditorScene.h"
#include "SceneManager.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "JsonUtil.h"
#include <filesystem>
#include <iostream>

namespace Norm {

	void ParticleEditorScene::Initialize() {
		//シーン共通の初期化
		BaseScene::Initialize();

		//インプット
		input_ = Input::GetInstance();

		//カメラの生成と初期化
		camera_ = std::make_unique<DevelopCamera>();
		camera_->Initialize();
		camera_->SetFarClip(1000.0f);
		camera_->worldTransform.SetTranslate({ 0.0f,50.0f,0.0f });
		camera_->worldTransform.SetRotate({ 0.2f,0.0f,0.0f });
		//カメラをセット
		Object3dManager::GetInstance()->SetCamera(camera_.get());
		LineManager::GetInstance()->SetCamera(camera_.get());
		ParticleManager::GetInstance()->SetCamera(camera_.get());
		
		//天球と地面の生成と初期化
		skydome_ = std::make_unique<Object3d>();
		skydome_->Initialize(ModelTag{}, Object3dManager::GetInstance()->GenerateName("Skydome"), "skydome");
		skydomeWT_.Initialize();
		skydome_->RegistWorldTransform(&skydomeWT_);
		textureHandleGround_ = TextureManager::GetInstance()->LoadTexture("grid.png");
		ground_ = std::make_unique<Object3d>();
		ground_->Initialize(ModelTag{}, Object3dManager::GetInstance()->GenerateName("Ground"), "ground");
		ground_->SetTexture(textureHandleGround_);
		groundWT_.Initialize();
		ground_->RegistWorldTransform(&groundWT_);

	}

	void ParticleEditorScene::Finalize() {
	}

	void ParticleEditorScene::Update() {
		//シーン共通の更新
		BaseScene::Update();
		//カメラの更新
		camera_->Update();

		//リセットコマンド
		if (state_.check == Check::kNone) {
			if (input_->TriggerKey(DIK_ESCAPE)) {
				state_.option = Option::kReset;
			}
		}
	}

	void ParticleEditorScene::DebugWithImGui() {
		//ImGui操作
#ifdef _DEBUG
		//最初のメニュー画面時操作
		StartWithImGui();

		//エディター画面時の操作
		EditWithImGui();

		//オプション処理
		OptionWithImGui();
		//確認処理
		CheckWithImGui();

		//デバッグ表示
		if (cParticle_) {
			cParticle_->Debug();
			cParticle_->ReculculateDuration();
		}

#endif //_DEBUG
	}

	void ParticleEditorScene::StartWithImGui() {
#ifdef _DEBUG
		//最初の操作
		if (state_.mode == Mode::kNone && state_.check == Check::kNone) {
			ImGui::SetNextWindowPos(ImVec2(450, 300), ImGuiCond_FirstUseEver);
			ImGui::Begin("メニュー");
			ImGui::Text("パーティクルエディターへようこそ！\n以下の項目から希望のオプションを選択して下さい。\n");

			if (ImGui::Button("パーティクルを新しく作成する")) {
				state_.mode = Mode::kEdit;
				//パーティクルの生成
				cParticle_ = std::make_unique<CombinedParticle>();
				//パーティクルの初期化
				cParticle_->Initialize(ParticleManager::GetInstance()->GenerateName("Sample"), "Basic", true);
				//基準のトランスフォームを調整
				TransformEuler baseTransform = cParticle_->GetBaseTransform();
				baseTransform.translate.y += 3.0f;
				cParticle_->SetBaseTransform(baseTransform);
				//jsonデータをロード
				cEditParam_ = cParticle_->GetParams();
				//selectedParticleHandle_に最初の要素を入れる
				selectedParticleHandle_ = cParticle_->particles_[0].particle->name_;

				//カメラの位置をセット
				camera_->worldTransform.SetTranslate({ 0.0f,4.0f,-20.0f });
				camera_->worldTransform.SetRotate({ 0.03f,0.0f,0.0f });
			}
			if (ImGui::Button("パーティクルを編集する")) {
				state_.check = Check::kEditName;
				//パーティクルの生成
				cParticle_ = std::make_unique<CombinedParticle>();
			}
			ImGui::End();
		}

#endif //_DEBUG
	}

	void ParticleEditorScene::EditWithImGui() {
#ifdef _DEBUG
		//パーティクルがnullならreturn
		if (!cParticle_) {
			return;
		}

		//エディターモード時の処理
		if (state_.mode == Mode::kEdit) {
			Editor();
		}

		//複合パーティクルのデバッグ表示
		cParticle_->Debug();

#endif //_DEBUG
	}

	void ParticleEditorScene::OptionWithImGui() {
#ifdef _DEBUG
		//オプションごとの処理
		switch (state_.option) {
		case ParticleEditorScene::Option::kNone:
		{
			//何もしない
			break;
		}
		case ParticleEditorScene::Option::kReset:
		{
			//リセット(Escapeキーによる)
			ImGui::OpenPopup("確認");
			ImGui::SetNextWindowPos(ImVec2(510, 30));
			if (ImGui::BeginPopupModal("確認", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
				ImGui::Text("リセットしますか？\n[注意] 編集中のパラメーターは反映されません\n ");
				if (ImGui::Button("はい", ImVec2(120, 0))) {
					sceneManager_->SetNextScene("PARTICLEEDITOR");
					state_.option = Option::kNone;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("いいえ", ImVec2(120, 0))) {
					state_.option = Option::kNone;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			break;
		}
		case ParticleEditorScene::Option::kSave:
		{
			//セーブ
			ImGui::SetNextWindowPos(ImVec2(470, 280));
			ImGui::Begin("パーティクルのセーブ");

			ImGui::Text("作成したパーティクルの名前を入力してください");
			char buffer[256];
			strncpy_s(buffer, sizeof(buffer), particleFileName_.c_str(), _TRUNCATE);
			buffer[sizeof(buffer) - 1] = '\0';
			if (ImGui::InputText("入力欄", buffer, sizeof(buffer))) {
				particleFileName_ = buffer;
			}
			if (particleFileName_.size() != 0) {
				if (ImGui::Button("名前を確定する")) {
					//該当ファイルのディレクトリパス
					std::string targetDir = "Resources/particles/" + particleFileName_;
					std::string targetDirInto = targetDir + "/";
					//該当ファイルが存在するか確認
					bool isFind = std::filesystem::exists(targetDir) &&
						std::filesystem::is_directory(targetDir);
					//該当ファイルが存在する場合中身をすべて消してから上書きする
					if (isFind) {
						//jsonファイルを全て消す
						for (const auto& entry : std::filesystem::directory_iterator(targetDir)) {
							//jsonファイルなら
							if (entry.is_regular_file() && entry.path().extension() == ".json") {
								std::filesystem::remove(entry.path());
							}
						}
						//targetDirIntoにJsonファイル保存
						for (const auto& [key, param] : cEditParam_) {
							JsonUtil::CreateJson(key, targetDirInto, param);
						}
					}
					//該当ファイルが存在しない場合新しくフォルダを作ってそこにデータを入れる
					else {
						//フォルダ作成
						std::filesystem::create_directories(targetDir);

						//targetDirIntoにJsonファイル保存
						for (const auto& [key, param] : cEditParam_) {
							JsonUtil::CreateJson(key, targetDirInto, param);
						}
					}

					//編集をさらに続けるかの確認へ
					state_.check = Check::kContinue;
				}
			}

			//表示を消す
			if (ImGui::Button("Back")) {
				state_.option = Option::kNone;
			}
			ImGui::End();

			break;
		}
		case ParticleEditorScene::Option::kShowParticleFileDialog:
		{
			//検索済みのパーティクル一覧を表示
			ImGui::SetNextWindowFocus();
			ImGui::SetNextWindowPos(ImVec2(860, 280));
			ImGui::Begin("パーティクル一覧");
			for (const auto& file : particleFiles_) {
				if (ImGui::Selectable(file.c_str())) {
					//パーティクルファイル名をクリア
					particleFileName_.clear();

					//パーティクルファイル名を保存
					particleFileName_ = file;

					state_.option = Option::kNone;	  //ウィンドウを閉じる
				}
			}
			ImGui::End();

			break;
		}
		case ParticleEditorScene::Option::kShowTextureFileDialog:
		{
			//検索済みのテクスチャ一覧を表示
			ImGui::SetNextWindowPos(ImVec2(360, 80));
			ImGui::Begin("テクスチャ一覧");
			for (const auto& file : textureFiles_) {
				if (ImGui::Selectable(file.c_str())) {
					cEditParam_[selectedParticleHandle_]["Texture"] = file;	//変更内容を保存
					state_.option = Option::kNone;  //ウィンドウを閉じる
					isChangeTexture_ = true;  //テクスチャが変更された
				}
			}
			ImGui::End();

			break;
		}
		default:
			break;
		}
#endif // _DEBUG
	}

	void ParticleEditorScene::CheckWithImGui() {
#ifdef _DEBUG
		//確認ごとの処理
		switch (state_.check) {
		case ParticleEditorScene::Check::kNone:
			//何もしない
			break;
		case ParticleEditorScene::Check::kContinue:
		{
			//続けるかの確認
			ImGui::OpenPopup("パーティクルの作成に成功しました");
			ImGui::SetNextWindowPos(ImVec2(510, 30));
			if (ImGui::BeginPopupModal("パーティクルの作成に成功しました", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
				ImGui::Text("作業を終了しますか？\n ");
				if (ImGui::Button("はい", ImVec2(120, 0))) {
					sceneManager_->SetNextScene("PARTICLEEDITOR");
					state_.option = Option::kNone;
					state_.check = Check::kNone;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("いいえ", ImVec2(120, 0))) {
					state_.option = Option::kNone;
					state_.check = Check::kNone;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			break;
		}
		case ParticleEditorScene::Check::kEditName:
		{
			// 編集対象名の確認ウィンドウ（常に表示）
			ImGui::SetNextWindowPos(
				ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
				ImGuiCond_Always,
				ImVec2(0.5f, 0.5f)
			);
			ImGui::SetNextWindowSize(ImVec2(400, 200));

			if (ImGui::Begin("パーティクル名の入力", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse)) {

				ImGui::Text("{particles}フォルダ内のパーティクルファイル名を入力してください\n");

				// particlesから参照
				if (ImGui::Button("{particles}フォルダから参照する")) {
					state_.option = Option::kShowParticleFileDialog;
					particleFiles_.clear();
					for (const auto& entry : std::filesystem::directory_iterator("Resources/particles")) {
						//フォルダのみ
						if (entry.is_directory()) {
							particleFiles_.push_back(entry.path().filename().string());
						}
					}
				}

				// 確認欄
				ImGui::Text("選択中のファイル : %s", particleFileName_.c_str());

				// 名前が入力されているなら
				if (!particleFileName_.empty()) {
					if (ImGui::Button("このファイルを編集する")) {
						// 編集モードへ移行
						state_.mode = Mode::kEdit;

						cParticle_->Initialize(
							ParticleManager::GetInstance()->GenerateName("Sample"),
							particleFileName_,
							true
						);
						TransformEuler baseTransform = cParticle_->GetBaseTransform();
						baseTransform.translate.y += 3.0f;
						cParticle_->SetBaseTransform(baseTransform);
						cEditParam_ = cParticle_->GetParams();
						camera_->worldTransform.SetTranslate({ 0.0f,4.0f,-20.0f });
						camera_->worldTransform.SetRotate({ 0.03f,0.0f,0.0f });
						//selectedParticleHandle_に最初の要素を入れる
						selectedParticleHandle_ = cParticle_->particles_[0].particle->name_;

						// ウィンドウを閉じたい場合はフラグ管理
						state_.check = Check::kNone;
					}
				}

				// 戻る
				if (ImGui::Button("Back")) {
					state_.option = Option::kNone;
					state_.check = Check::kNone;
					particleFileName_.clear();
					cParticle_.release();
				}

				ImGui::End();
			}
			break;
		}
		default:
			break;
		}

#endif // _DEBUG
	}

	void ParticleEditorScene::Editor() {
#ifdef _DEBUG
		//selectedParticleHandle_に該当するインデックスを取得する
		auto it = std::find_if(
			cParticle_->particles_.begin(),
			cParticle_->particles_.end(),
			[&](const auto& sParInfo) {
				return selectedParticleHandle_ == sParInfo.particle->name_;
			}
		);
		//selectedParticleHandle_に入っている文字列で処理を決める
		bool isEmpty = false;
		if (selectedParticleHandle_.size() == 0) {
			//空文字(インスタンスに何も含まれていない状態)だったら、生成ボタンだけ表示
			isEmpty = true;
		}
		else if (it == cParticle_->particles_.end()) {
			//インスタンスに含まれていない名前だったらreturn(本来ここは通らない)
			return;
		}
		//インデックス取得
		size_t index = std::distance(cParticle_->particles_.begin(), it);

		//パラメーター編集処理のラムダ式
		auto paramProcess = [this, index]() {
			//エイリアス
			auto& particle = cParticle_->particles_[index].particle;
			auto& param = cEditParam_[selectedParticleHandle_];

			//パラメーター
			ImGui::SetNextWindowPos(ImVec2(10, 80));
			ImGui::SetNextWindowSize(ImVec2(300, 380));
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove;
			ImGui::Begin("パーティクルのパラメーター", nullptr, flags);
			//テクスチャ
			{
				if (ImGui::CollapsingHeader("テクスチャの設定")) {
					std::string nowTexture = param["Texture"];
					ImGui::Text("選択中のテクスチャ : %s", nowTexture.c_str());
					//フォルダ内のテクスチャを検索
					if (ImGui::Button("{textures}フォルダ内のテクスチャを検索")) {
						state_.option = Option::kShowTextureFileDialog;
						textureFiles_.clear();
						for (const auto& entry : std::filesystem::directory_iterator("Resources/textures")) {
							if (entry.is_regular_file() && entry.path().extension() == ".png") {
								textureFiles_.push_back(entry.path().filename().string());
							}
						}
					}
				}
			}
			//カラー
			Vector4 startColorMax = { param["StartColor"]["Max"]["x"],param["StartColor"]["Max"]["y"],param["StartColor"]["Max"]["z"],param["StartColor"]["Max"]["w"] };
			Vector4 startColorMin = { param["StartColor"]["Min"]["x"],param["StartColor"]["Min"]["y"],param["StartColor"]["Min"]["z"],param["StartColor"]["Min"]["w"] };
			Vector4 endColorMax = { param["EndColor"]["Max"]["x"],param["EndColor"]["Max"]["y"],param["EndColor"]["Max"]["z"],param["EndColor"]["Max"]["w"] };
			Vector4 endColorMin = { param["EndColor"]["Min"]["x"],param["EndColor"]["Min"]["y"],param["EndColor"]["Min"]["z"],param["EndColor"]["Min"]["w"] };
			{
				if (ImGui::CollapsingHeader("色の設定")) {
					ImGui::ColorEdit4("StartColorMax", &startColorMax.x);
					ImGui::ColorEdit4("StartColorMin", &startColorMin.x);
					ImGui::ColorEdit4("EndColorMax", &endColorMax.x);
					ImGui::ColorEdit4("EndColorMin", &endColorMin.x);
				}
			}
			//初期トランスフォーム
			Vector3 rotateMax = { param["GrainTransform"]["Rotate"]["Max"]["x"],param["GrainTransform"]["Rotate"]["Max"]["y"] ,param["GrainTransform"]["Rotate"]["Max"]["z"] };
			Vector3 rotateMin = { param["GrainTransform"]["Rotate"]["Min"]["x"],param["GrainTransform"]["Rotate"]["Min"]["y"] ,param["GrainTransform"]["Rotate"]["Min"]["z"] };
			Vector3 scaleMax = { param["GrainTransform"]["Scale"]["Max"]["x"],param["GrainTransform"]["Scale"]["Max"]["y"],param["GrainTransform"]["Scale"]["Max"]["z"] };
			Vector3 scaleMin = { param["GrainTransform"]["Scale"]["Min"]["x"],param["GrainTransform"]["Scale"]["Min"]["y"],param["GrainTransform"]["Scale"]["Min"]["z"] };
			{
				if (ImGui::CollapsingHeader("初期トランスフォームの設定")) {
					if (ImGui::TreeNode("Rotate")) {
						ImGui::DragFloat3("RotateMax", &rotateMax.x, 0.1f);
						ImGui::DragFloat3("RotateMin", &rotateMin.x, 0.1f);
						ImGui::TreePop();
					}
					if (ImGui::TreeNode("Scale")) {
						ImGui::DragFloat3("ScaleMax", &scaleMax.x, 0.1f, 0.0f, FLT_MAX);
						ImGui::DragFloat3("ScaleMin", &scaleMin.x, 0.1f, 0.0f, FLT_MAX);
						ImGui::TreePop();
					}
				}
			}
			//回転速度
			Vector3 angularVelocityMax = { param["AngularVelocity"]["Max"]["x"],param["AngularVelocity"]["Max"]["y"],param["AngularVelocity"]["Max"]["z"] };
			Vector3 angularVelocityMin = { param["AngularVelocity"]["Min"]["x"],param["AngularVelocity"]["Min"]["y"],param["AngularVelocity"]["Min"]["z"] };
			{
				if (ImGui::CollapsingHeader("回転速度")) {
					ImGui::DragFloat3("AngularVelocityMax", &angularVelocityMax.x, 0.1f);
					ImGui::DragFloat3("AngularVelocityMin", &angularVelocityMin.x, 0.1f);
				}
			}
			//サイズ速度
			float sizeVelocityMax = param["SizeVelocity"]["Max"];
			float sizeVelocityMin = param["SizeVelocity"]["Min"];
			{
				if (ImGui::CollapsingHeader("サイズ速度")) {
					ImGui::DragFloat("SizeVelocityMax", &sizeVelocityMax, 0.1f, sizeVelocityMin, FLT_MAX);
					ImGui::DragFloat("SizeVelocityMin", &sizeVelocityMin, 0.1f, -FLT_MAX, sizeVelocityMax);
				}
			}
			//速度
			Vector3 velocityMax = { param["Velocity"]["Max"]["x"],param["Velocity"]["Max"]["y"],param["Velocity"]["Max"]["z"] };
			Vector3 velocityMin = { param["Velocity"]["Min"]["x"],param["Velocity"]["Min"]["y"],param["Velocity"]["Min"]["z"] };
			{
				if (ImGui::CollapsingHeader("速度の設定")) {
					ImGui::DragFloat3("VelocityMax", &velocityMax.x, 0.1f);
					ImGui::DragFloat3("VelocityMin", &velocityMin.x, 0.1f);
				}
			}
			//寿命
			float lifeTimeMax = param["LifeTime"]["Max"];
			float lifeTimeMin = param["LifeTime"]["Min"];
			{
				if (ImGui::CollapsingHeader("寿命の設定")) {
					ImGui::DragFloat("LifeTimeMax", &lifeTimeMax, 0.1f, lifeTimeMin);
					ImGui::DragFloat("LifeTimeMin", &lifeTimeMin, 0.1f, 0.0f, lifeTimeMax);
				}
			}
			//粒の最大数
			int maxGrains = param["MaxGrains"];
			{
				if (ImGui::CollapsingHeader("粒の最大数")) {
					//推奨値の計算
					int RecommendValue;
					int generateMethod = param["GenerateMethod"];
					switch ((Particle::GenerateMethod)generateMethod) {
					case Particle::GenerateMethod::Random:
						RecommendValue = int(lifeTimeMax * param["EmitRate"]);
						break;
					case Particle::GenerateMethod::Clump:
						RecommendValue = int(lifeTimeMax * param["EmitRate"] * param["ClumpNum"]);
						break;
					default:
						break;
					}
					ImGui::Text("推奨値 : %d (最低限の数で回せる値)", RecommendValue);
					if (ImGui::Button("推奨値を適用")) {
						maxGrains = RecommendValue;
					}
					ImGui::DragInt("MaxGrains", &maxGrains, 1, 0);
				}
			}
			//1秒あたりの粒の生成量
			int emitRate = param["EmitRate"];
			{
				if (ImGui::CollapsingHeader("1秒あたりの粒の生成量")) {
					ImGui::DragInt("EmitRate", &emitRate, 1, 0);
				}
			}
			//ブレンドモード
			int blendMode = param["BlendMode"];
			{
				const char* blendModeList[] = { "None","Normal","Add","Subtract","Multiply","Screen","Exclusion" };
				if (ImGui::CollapsingHeader("ブレンドモード")) {
					ImGui::Combo("BlendMode", &blendMode, blendModeList, (int)BlendMode::kMaxBlendModeNum);
				}
			}
			//形状を写す
			int primitive = param["Primitive"];
			{
				const char* primitiveList[] = { "Shere","Cube","SkyBox","Plane","Ring","Tube" };
				if (ImGui::CollapsingHeader("形状")) {
					if (ImGui::Combo("Primitive", &primitive, primitiveList, (int)Shape::ShapeKind::kMaxShapeKindNum)) {
						//形状の変更通知
						isShapeChange_ = true;
					}
				}
			}

			//editParamに変更を反映
			{
				param["StartColor"]["Max"]["x"] = startColorMax.x;
				param["StartColor"]["Max"]["y"] = startColorMax.y;
				param["StartColor"]["Max"]["z"] = startColorMax.z;
				param["StartColor"]["Max"]["w"] = startColorMax.w;
				param["StartColor"]["Min"]["x"] = startColorMin.x;
				param["StartColor"]["Min"]["y"] = startColorMin.y;
				param["StartColor"]["Min"]["z"] = startColorMin.z;
				param["StartColor"]["Min"]["w"] = startColorMin.w;
				param["EndColor"]["Max"]["x"] = endColorMax.x;
				param["EndColor"]["Max"]["y"] = endColorMax.y;
				param["EndColor"]["Max"]["z"] = endColorMax.z;
				param["EndColor"]["Max"]["w"] = endColorMax.w;
				param["EndColor"]["Min"]["x"] = endColorMin.x;
				param["EndColor"]["Min"]["y"] = endColorMin.y;
				param["EndColor"]["Min"]["z"] = endColorMin.z;
				param["EndColor"]["Min"]["w"] = endColorMin.w;
				param["GrainTransform"]["Rotate"]["Max"]["x"] = rotateMax.x;
				param["GrainTransform"]["Rotate"]["Max"]["y"] = rotateMax.y;
				param["GrainTransform"]["Rotate"]["Max"]["z"] = rotateMax.z;
				param["GrainTransform"]["Rotate"]["Min"]["x"] = rotateMin.x;
				param["GrainTransform"]["Rotate"]["Min"]["y"] = rotateMin.y;
				param["GrainTransform"]["Rotate"]["Min"]["z"] = rotateMin.z;
				param["GrainTransform"]["Scale"]["Max"]["x"] = scaleMax.x;
				param["GrainTransform"]["Scale"]["Max"]["y"] = scaleMax.y;
				param["GrainTransform"]["Scale"]["Max"]["z"] = scaleMax.z;
				param["GrainTransform"]["Scale"]["Min"]["x"] = scaleMin.x;
				param["GrainTransform"]["Scale"]["Min"]["y"] = scaleMin.y;
				param["GrainTransform"]["Scale"]["Min"]["z"] = scaleMin.z;
				param["AngularVelocity"]["Max"]["x"] = angularVelocityMax.x;
				param["AngularVelocity"]["Max"]["y"] = angularVelocityMax.y;
				param["AngularVelocity"]["Max"]["z"] = angularVelocityMax.z;
				param["AngularVelocity"]["Min"]["x"] = angularVelocityMin.x;
				param["AngularVelocity"]["Min"]["y"] = angularVelocityMin.y;
				param["AngularVelocity"]["Min"]["z"] = angularVelocityMin.z;
				param["SizeVelocity"]["Max"] = sizeVelocityMax;
				param["SizeVelocity"]["Min"] = sizeVelocityMin;
				param["Velocity"]["Max"]["x"] = velocityMax.x;
				param["Velocity"]["Max"]["y"] = velocityMax.y;
				param["Velocity"]["Max"]["z"] = velocityMax.z;
				param["Velocity"]["Min"]["x"] = velocityMin.x;
				param["Velocity"]["Min"]["y"] = velocityMin.y;
				param["Velocity"]["Min"]["z"] = velocityMin.z;
				param["LifeTime"]["Max"] = lifeTimeMax;
				param["LifeTime"]["Min"] = lifeTimeMin;
				param["MaxGrains"] = maxGrains;
				param["EmitRate"] = emitRate;
				param["BlendMode"] = blendMode;
				param["Primitive"] = primitive;
				//パーティクルに反映
				cParticle_->SetParams(cEditParam_);
				//形状の変更通知を受け取ったら
				if (isShapeChange_) {
					//パーティクルの形状を変更
					particle->ShapeChange();
					//変更通知フラグを下げる
					isShapeChange_ = false;
				}
				//テクスチャの変更通知を受け取ったら
				if (isChangeTexture_) {
					//パーティクルのテクスチャを変更
					particle->TextureChange();
					//変更通知フラグを下げる
					isChangeTexture_ = false;
				}
			}
			ImGui::End();
			};

		//エミッター編集処理のラムダ式
		auto emitterProcess = [this, index]() {
			//エイリアス
			auto& particle = cParticle_->particles_[index].particle;
			auto& param = cEditParam_[selectedParticleHandle_];

			//エミッター
			ImGui::SetNextWindowPos(ImVec2(970, 80));
			ImGui::SetNextWindowSize(ImVec2(300, 380));
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove;
			ImGui::Begin("エミッター", nullptr, flags);
			//エミッター可視化用ライン登録処理
			{
				ImGui::Checkbox("エミッターの枠を表示する", &displayLineEmitter_);
				if (displayLineEmitter_) {
					//AABBを作成
					AABB aabb;
					aabb.max = particle->emitter_.worldTransform.translate + particle->emitter_.worldTransform.scale;
					aabb.min = particle->emitter_.worldTransform.translate - particle->emitter_.worldTransform.scale;

					Vector4 color = { 1,0,0,1 };

					MyMath::CreateLineAABB(aabb, color);
				}
			}
			//ローカルのトランスフォームを写す
			auto local = param["LocalTransform"];
			TransformEuler localTransform = {
				.scale = {local["Scale"]["x"],local["Scale"]["y"],local["Scale"]["z"]},
				.rotate = {local["Rotate"]["x"],local["Rotate"]["y"],local["Rotate"]["z"]},
				.translate = {local["Translate"]["x"],local["Translate"]["y"],local["Translate"]["z"]}
			};
			{
				if (ImGui::CollapsingHeader("ローカルのトランスフォーム")) {
					ImGui::DragFloat3("平行移動(ローカル)", &localTransform.translate.x, 0.1f);
					ImGui::DragFloat3("拡縮(ローカル)", &localTransform.scale.x, 0.1f, 0.1f, FLT_MAX);
				}
			}
			//生成方法を写す
			int generateMethod = param["GenerateMethod"];
			int clumpNum = param["ClumpNum"];
			{
				if (ImGui::CollapsingHeader("生成方法")) {
					const char* methods[] = { "Random", "Clump" };
					ImGui::Combo("変更（生成方法）", &generateMethod, methods, IM_ARRAYSIZE(methods));

					if (generateMethod == (int)Particle::GenerateMethod::Clump) {
						ImGui::DragInt("一塊の粒の数", &clumpNum, 1, 1, 20);
					}
				}
			}
			//スタイルを写す
			int effectStyle = param["EffectStyle"];
			{
				if (ImGui::CollapsingHeader("スタイル")) {
					const char* styles[] = { "Loop","OneShot" };
					const char* currentStyle = "";
					switch ((Particle::EffectStyle)effectStyle) {
					case Particle::EffectStyle::Loop:
						currentStyle = styles[0];
						break;
					case Particle::EffectStyle::OneShot:
						currentStyle = styles[1];
						break;
					default:
						break;
					}
					ImGui::Text("今のスタイル : %s", currentStyle);
					ImGui::Combo("変更（スタイル）", &effectStyle, styles, IM_ARRAYSIZE(styles));
				}
			}
			//重力を写す
			int isGravity = param["IsGravity"];
			float gravity = param["Gravity"];
			{
				if (ImGui::CollapsingHeader("重力")) {
					ImGui::Checkbox("重力の処理をするか", (bool*)&isGravity);
					ImGui::DragFloat("重力値", &gravity, 0.1f);
				}
			}
			//床を写す
			int isBound = param["IsBound"];
			float repulsion = param["Repulsion"];
			float floorHeight = param["FloorHeight"];
			{
				if (ImGui::CollapsingHeader("床")) {
					ImGui::Checkbox("床の処理をするか", (bool*)&isBound);
					ImGui::DragFloat("床の反発値", &repulsion, 0.1f);
					ImGui::DragFloat("床の高さ", &floorHeight, 0.1f);
				}
			}
			//ビルボードを写す
			int isBillboard = param["IsBillboard"];
			{
				if (ImGui::CollapsingHeader("ビルボード")) {
					ImGui::Checkbox("ビルボードの処理をするか", (bool*)&isBillboard);
				}
			}
			//開始時間及び、終了時間の設定
			float startTime = param["StartTime"];
			float endTime = param["EndTime"];
			{
				if (ImGui::CollapsingHeader("開始時間と終了時間")) {
					//開始時間
					ImGui::DragFloat("開始", &startTime, 0.01f, 0.0f);
					//終了時間
					ImGui::DragFloat("終了", &endTime, 0.01f, startTime);
				}
				//反映
				for (auto& particleInfo : cParticle_->particles_) {
					//選択中のパーティクルに反映
					if (particleInfo.particle->name_ == selectedParticleHandle_) {
						particleInfo.startTime = startTime;
						particleInfo.endTime = endTime;
						break;
					}
				}
			}
			//生成アルゴリズム
			{
				if (ImGui::CollapsingHeader("生成アルゴリズム")) {
					ImGui::Checkbox("生成するか(isPlay)", &particle->emitter_.isPlay);
				}
			}
			//基準のトランスフォーム
			{
				//区切り
				ImGui::Separator();

				if (ImGui::CollapsingHeader("基準のトランスフォーム")) {
					TransformEuler baseTransform = cParticle_->GetBaseTransform();
					ImGui::DragFloat3("平行移動(ベース)", &baseTransform.translate.x, 0.1f);
					ImGui::DragFloat3("拡縮(ベース)", &baseTransform.scale.x, 0.1f, 0.1f, 100.0f);
					cParticle_->SetBaseTransform(baseTransform);
				}
			}
			//editParamに変更を反映
			{
				//json形式に変換するラムダ式
				auto convertVector3ToJson = [&](auto& dst, const Vector3& v) {
					dst["x"] = v.x;
					dst["y"] = v.y;
					dst["z"] = v.z;
					dst["w"] = 0.0f;
					};

				convertVector3ToJson(param["LocalTransform"]["Scale"], localTransform.scale);
				convertVector3ToJson(param["LocalTransform"]["Rotate"], localTransform.rotate);
				convertVector3ToJson(param["LocalTransform"]["Translate"], localTransform.translate);
				param["GenerateMethod"] = generateMethod;
				param["ClumpNum"] = clumpNum;
				param["EffectStyle"] = effectStyle;
				param["IsGravity"] = isGravity;
				param["Gravity"] = gravity;
				param["IsBound"] = isBound;
				param["Repulsion"] = repulsion;
				param["FloorHeight"] = floorHeight;
				param["IsBillboard"] = isBillboard;
				//パーティクルに反映
				cParticle_->SetParams(cEditParam_);
			}
			//editParamに変更を反映
			{
				param["LocalTransform"]["Scale"]["x"] = localTransform.scale.x;
				param["LocalTransform"]["Scale"]["y"] = localTransform.scale.y;
				param["LocalTransform"]["Scale"]["z"] = localTransform.scale.z;
				param["LocalTransform"]["Rotate"]["x"] = localTransform.rotate.x;
				param["LocalTransform"]["Rotate"]["y"] = localTransform.rotate.y;
				param["LocalTransform"]["Rotate"]["z"] = localTransform.rotate.z;
				param["LocalTransform"]["Translate"]["x"] = localTransform.translate.x;
				param["LocalTransform"]["Translate"]["y"] = localTransform.translate.y;
				param["LocalTransform"]["Translate"]["z"] = localTransform.translate.z;
				param["GenerateMethod"] = generateMethod;
				param["ClumpNum"] = clumpNum;
				param["EffectStyle"] = effectStyle;
				param["IsGravity"] = isGravity;
				param["Gravity"] = gravity;
				param["IsBound"] = isBound;
				param["Repulsion"] = repulsion;
				param["FloorHeight"] = floorHeight;
				param["IsBillboard"] = isBillboard;
				param["StartTime"] = startTime;
				param["EndTime"] = endTime;

				//パーティクルに反映
				cParticle_->SetParams(cEditParam_);
			}

			ImGui::End();
			};

		//各パーティクルの管理のラムダ式
		auto particleManagement = [this]() {
			//管理
			ImGui::SetNextWindowPos(ImVec2(10, 470));
			ImGui::SetNextWindowSize(ImVec2(300, 240));
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove;
			ImGui::Begin("パーティクルの管理", nullptr, flags);

			//パーティクルの新規作成
			{
				//新規作成ボタン
				if (ImGui::Button("新規パーティクルの作成")) {
					//複合パーティクルインスタンスにのみ保持(JSON出力はセーブ時)
					cParticle_->AddParticle("Basic/basic.json", 0.0f, 10.0f);
					//cEditParam_に新規パーティクルのパラメーターを追加
					cEditParam_[cParticle_->particles_.back().particle->name_] = cParticle_->particles_.back().particle->GetParam();
					//もしパーティクルが今生成したもののみだった場合
					if (cParticle_->particles_.size() == 1) {
						//選択中のパーティクルをそのパーティクルにセットする
						selectedParticleHandle_ = cParticle_->particles_[0].particle->name_;
					}

				}
			}
			//全パーティクルの再生＆ループチェック
			{
				//全パーティクル再生ボタン
				if (ImGui::Button("再生")) {
					//再生状態でないなら
					if (!cParticle_->GetIsPlay()) {
						cParticle_->SetIsPlay(true);
					}
					//再生状態なら
					else {
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "すでに再生中です");
					}
				}

				//連続再生チェックボックス
				ImGui::Checkbox("連続再生", &cParticle_->playInfo_.isRepeat);

				//ループさせるかのチェックボックス
				ImGui::Checkbox("ループ再生", &isLoop_);
				//ループ処理
				if (isLoop_) {
					//全パーティクルが停止していたら
					if (!cParticle_->GetIsPlay()) {
						//再生状態にする
						cParticle_->SetIsPlay(true);
					}
				}
			}
			//シークバー
			{
				float elapsedTime = cParticle_->GetElapsedTimer();
				float maxTime = cParticle_->GetDuration();
				ImGui::Text("再生時間 : %.2f / %.2f", elapsedTime, maxTime);
				ImGui::SliderFloat(" ", &elapsedTime, 0.0f, maxTime);
			}
			//セーブボタン
			{
				if (cParticle_ && cParticle_->particles_.size() != 0) {
					if (ImGui::Button("セーブ")) {
						state_.option = Option::kSave;
					}
				}
			}

			ImGui::End();
			};

		//各パーティクルのオプションのラムダ式
		auto particleOption = [this]() {
			auto& wParam = cEditParam_[selectedParticleHandle_];

			//管理
			ImGui::SetNextWindowPos(ImVec2(320, 470));
			ImGui::SetNextWindowSize(ImVec2(950, 240));
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove;
			ImGui::Begin("パーティクルごとのオプション", nullptr, flags);

			//タブ分け
			if (ImGui::BeginTabBar("タブ")) {
				//削除依頼キーコンテナ
				std::vector<std::string> eraseKeys;
				//タブごとの処理
				for (const auto& [key, param] : cEditParam_) {
					if (ImGui::BeginTabItem(key.c_str())) {
						//選択中パーティクルのハンドル名を保持
						selectedParticleHandle_ = key;

						//名前の変更
						{
							ImGui::Text("名前");
							char buffer[256];
							strncpy_s(buffer, sizeof(buffer), particleName_.c_str(), _TRUNCATE);
							buffer[sizeof(buffer) - 1] = '\0';
							if (ImGui::InputText("入力欄", buffer, sizeof(buffer))) {
								particleName_ = buffer;
							}
							ImGui::SameLine();
							//もしキーと名前が違ったら
							if (key != particleName_) {
								//別のキーとかぶっている、もしくは空文字列なら変更不可フラグを立てる
								bool isDisable = false;
								if (particleName_.size() == 0) {
									isDisable = true;
								}
								else {
									for (const auto& [otherKey, otherParam] : cEditParam_) {
										if (otherKey == particleName_) {
											isDisable = true;
											break;
										}
									}
								}
								//変更可能なら
								if (!isDisable) {
									if (ImGui::Button("変更")) {
										//編集用パラメーターのキーを変更
										cEditParam_[particleName_] = cEditParam_[key];
										//選択中のパーティクルハンドル名を変更
										selectedParticleHandle_ = particleName_;
										//編集用複合パーティクル内のパーティクル名を変更
										for (auto& particleInfo : cParticle_->particles_) {
											if (particleInfo.particle->name_ == key) {
												//変更
												particleInfo.particle->name_ = particleName_;
												//パーティクルマネージャーの管理名も変更
												if (!ParticleManager::GetInstance()->Rename(key, particleName_)) {
													//失敗したらエラーメッセージを表示
													assert(0 && "パーティクルマネージャーの名前変更に失敗しました");
												}

												break;
											}
										}
										//編集用パーティクル名に空文字をセット
										particleName_ = "";

										//元のキーの削除を依頼
										eraseKeys.push_back(key);
									}
								}
								//変更不可なら
								else {
									//警告文を表示
									ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "この名前は使用できません");
								}
							}
							ImGui::Separator();
						}
						//パーティクルの削除
						{
							//削除ボタン
							if (ImGui::Button("パーティクルの削除")) {
								//複合パーティクルインスタンスから該当キーオブジェクトの削除
								cParticle_->RemoveParticle(key);
								//削除依頼をだす
								eraseKeys.push_back(key);
								//選択中のパーティクルハンドル名を記入
								bool isSet = false;
								for (const auto& [key2, param] : cEditParam_) {
									if (key2 == key) {
										continue;
									}
									//次の選択タブ
									isSet = true;
									selectedParticleHandle_ = key2;
								}
								//次の選択タブが未設定なら
								if (!isSet) {
									//空文字を入れる
									selectedParticleHandle_ = "";
								}
							}
						}

						ImGui::EndTabItem();
					}
				}
				//依頼された要素の削除
				for (const auto& key : eraseKeys) {
					cEditParam_.erase(key);
				}

				ImGui::EndTabBar();
			}
			ImGui::End();
			};


		//コンテナにパーティクルが一つも含まれていない場合に表示しないもの
		if (!isEmpty) {
			//パラメーター処理の呼び出し
			paramProcess();

			//エミッター処理の呼び出し
			emitterProcess();

			//各パーティクルオプションの呼び出し
			particleOption();
		}

		//各パーティクル管理の呼び出し
		particleManagement();

#endif //_DEBUG
	}


}