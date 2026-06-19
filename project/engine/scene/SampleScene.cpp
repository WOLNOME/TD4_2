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

		//天球と地面の生成と初期化
		skydome_ = std::make_unique<Skydome>();
		skydome_->Initialize();
		ground_ = std::make_unique<Ground>();
		ground_->Initialize();

		//スプライトの生成と初期化
		sprite_ = std::make_unique<Sprite>();
		textureHandleSprite_ = TextureManager::GetInstance()->LoadTexture("uvChecker.png");
		sprite_->Initialize(SpriteTag{}, Object3dManager::GetInstance()->GenerateName("Sprite"), Order::Front0, textureHandleSprite_);

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
		
#endif // _DEBUG
	}

}