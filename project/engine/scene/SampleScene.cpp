#include "SampleScene.h"

namespace Norm {

	void SampleScene::Initialize() {
		//シーン共通の初期化
		BaseScene::Initialize();

		//インプット
		input_ = Input::GetInstance();

		//カメラの生成と初期化
		camera_ = std::make_unique<DevelopCamera>();
		camera_->Initialize();
		camera_->SetFarClip(1000.0f);
		camera_->worldTransform.SetTranslate(cameraTranslate);
		camera_->worldTransform.SetRotate(cameraRotate);
		//カメラをセット
		Object3dManager::GetInstance()->SetCamera(camera_.get());
		LineManager::GetInstance()->SetCamera(camera_.get());
		ParticleManager::GetInstance()->SetCamera(camera_.get());

		//ライト
		dirLight = std::make_unique<DirectionalLight>();
		dirLight->SetIntensity(0.05f);
		dirLight->SetColor({ 1,0,0,1 });
		dirLight->SetDirection({ 0.5f,0.5f,-1.0f });

		pointLight = std::make_unique<PointLight>();
		pointLight->SetPosition({ 0.0f,10.0f,40.0f });

		//ライトを登録
		sceneLight_->SetLight(dirLight.get());
		sceneLight_->SetLight(pointLight.get());

		//天球と地面の生成と初期化
		skydome_ = std::make_unique<Skydome>();
		skydome_->Initialize();
		ground_ = std::make_unique<Ground>();
		ground_->Initialize();

		//スプライトの生成と初期化
		sprite_ = std::make_unique<Sprite>();
		textureHandleSprite_ = TextureManager::GetInstance()->LoadTexture("uvChecker.png");
		sprite_->Initialize(SpriteTag{}, Object3dManager::GetInstance()->GenerateName("Sprite"), Order::Front0, textureHandleSprite_);
		sprite_->SetIsDisplay(false);

		//オブジェクトの生成と初期化
		modelBase_ = std::make_unique<Object3d>();
		modelBase_->Initialize(ModelTag{}, Object3dManager::GetInstance()->GenerateName("ModelBase"), "cube");
		modelBaseWT_.Initialize();
		modelBaseWT_.SetTranslate({ -5.0f,5.0f,0.0f });
		modelBase_->RegistWorldTransform(&modelBaseWT_);
		shapeBase_ = std::make_unique<Object3d>();
		shapeBase_->Initialize(ShapeTag{}, Object3dManager::GetInstance()->GenerateName("ShapeBase"), Shape::ShapeKind::kSphere);
		shapeBaseWT_.Initialize();
		shapeBaseWT_.SetTranslate({ 5.0f,5.0f,0.0f });
		shapeBase_->RegistWorldTransform(&shapeBaseWT_);
		sampleMap_ = std::make_unique<Object3d>();
		sampleMap_->Initialize(ModelTag{}, Object3dManager::GetInstance()->GenerateName("SampleMap"), "sampleMap");
		sampleMapWT_.Initialize();
		sampleMapWT_.SetTranslate({ 0.0f,5.0f,40.0f });
		sampleMapWT_.SetRotate({ 0.0f,0.0f,0.0f });
		sampleMap_->RegistWorldTransform(&sampleMapWT_);

		back_ = std::make_unique<Object3d>();
		back_->Initialize(ShapeTag{}, Object3dManager::GetInstance()->GenerateName("Back"), Shape::ShapeKind::kPlane);
		back_->SetTexture(TextureManager::GetInstance()->LoadTexture("black.png"));



		//パーティクルの生成と初期化
		particle_ = std::make_unique<CombinedParticle>();
		particle_->Initialize("SampleParticle", "Basic");
		TransformEuler baseTransform = {};
		baseTransform.translate = { 0.0f,8.0f,0.0f };
		baseTransform.rotate = { 0.0f,0.0f,0.0f };
		baseTransform.scale = { 1.0f,1.0f,1.0f };
		particle_->SetBaseTransform(baseTransform);
		particle_->SetIsPlay(true);
		particle_->SetIsRepeat(true);

	}

	void SampleScene::Finalize() {
	}

	void SampleScene::Update() {
		//シーン共通の更新
		BaseScene::Update();
		//カメラの更新
		camera_->Update();
		//オブジェクトの回転
		modelBaseWT_.SetRotate({ 0.0f,modelBaseWT_.GetRotate().y + 0.01f,0.0f });
		shapeBaseWT_.SetRotate({ shapeBaseWT_.GetRotate().x + 0.01f,shapeBaseWT_.GetRotate().y + 0.01f,shapeBaseWT_.GetRotate().z + 0.01f });
	}

	void SampleScene::DebugWithImGui() {
#ifdef _DEBUG
		back_->Debug(L"背景");

		//平行光源
		dirLight->DebugWithImGui(L"平行光源１");
		//点光源
		pointLight->DebugWithImGui(L"点光源１");
		//カメラ
		camera_->DebugWithImGui();
		//ポストエフェクト
		PostEffectManager::GetInstance()->DebugWithImGui();

		
#endif // _DEBUG
	}
}