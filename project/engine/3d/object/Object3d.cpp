#include "Object3d.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "MainRender.h"
#include "Object3dManager.h"
#include "TextureManager.h"
#include "GPUDescriptorManager.h"
#include "ModelManager.h"
#include "BaseCamera.h"
#include "SceneLight.h"
#include "SceneManager.h"
#include <fstream>
#include <sstream>
#include <cassert>

namespace Norm {

	Object3d::Object3d() {
		//シーンタグ
		sceneTag_ = SceneManager::GetInstance()->GetCurrentScene()->GetSceneName();

		//オブジェクトリソースの作成
		objectResource_ = CreateObjectResource();
	}

	Object3d::~Object3d() {
		//ワールドトランスフォームを破棄
		worldTransforms_.clear();
		//確保したヒープを解放
		GPUDescriptorManager::GetInstance()->Free(objectResource_.instancingSrvIndex);

		//マネージャーから削除
		Object3dManager::GetInstance()->DeleteObject(name_);
	}

	void Object3d::Initialize(ModelTag, const std::string& name, const std::string& filePath) {
		//オブジェクトの種類を決定
		objKind_ = ObjectKind::Model;
		//名前
		name_ = name;
		//モデルマネージャーでモデルを生成
		ModelManager::GetInstance()->LoadModel(filePath, ModelFormat::OBJ);
		//モデルマネージャーから検索してセットする
		model_ = ModelManager::GetInstance()->FindModel(filePath);


		//マネージャーに登録
		Object3dManager::GetInstance()->RegisterObject(name_, this);
	}

	void Object3d::Initialize(AnimationModelTag, const std::string& name, const std::string& filePath) {
		//オブジェクトの種類を決定
		objKind_ = ObjectKind::AnimationModel;
		//名前
		name_ = name;
		//アニメーションモデルの生成と初期化
		animationModel_ = std::make_unique<AnimationModel>();
		animationModel_->Initialize(filePath, ModelFormat::GLTF);

		//マネージャーに登録
		Object3dManager::GetInstance()->RegisterObject(name_, this);
	}

	void Object3d::Initialize(ShapeTag, const std::string& name, Shape::ShapeKind kind) {
		//オブジェクトの種類を決定
		objKind_ = ObjectKind::Shape;
		//名前
		name_ = name;
		//形状の生成と初期化
		shape_ = std::make_unique<Shape>();
		shape_->Initialize(kind);

		//マネージャーに登録
		Object3dManager::GetInstance()->RegisterObject(name_, this);
	}

	uint32_t Object3d::RegistWorldTransform(WorldTransform* _worldTransform) {
		if (_worldTransform == nullptr) {
			return UINT32_MAX; // 無効ハンドル
		}

		uint32_t handle;

		// 再利用可能なインデックスがある場合
		if (!freeIndices_.empty()) {
			handle = freeIndices_.back();
			freeIndices_.pop_back();
		}
		else {
			handle = nextIndex_;
			++nextIndex_;
		}

		worldTransforms_[handle] = _worldTransform;

		return handle;
	}

	void Object3d::DeleteWorldTransform(uint32_t _handle) {
		auto it = worldTransforms_.find(_handle);
		if (it == worldTransforms_.end()) {
			return; // 存在しない
		}

		// マップから削除
		worldTransforms_.erase(it);

		// インデックスを再利用リストへ
		freeIndices_.push_back(_handle);
	}

	void Object3d::Update() {
		//ワールドトランスフォームの更新
		for (auto& [handle, worldTransform] : worldTransforms_) {
			if (worldTransform) {
				worldTransform->UpdateMatrix();
			}
		}

		//オブジェクトの種類ごとの処理
		switch (objKind_) {
		case ObjectKind::Model:
			//何もなし
			break;
		case ObjectKind::AnimationModel:
			//アニメーション反映処理
			animationModel_->Update();
			//CS前処理（スキニング）
			animationModel_->SettingCSPreDraw();
			break;
		case ObjectKind::Shape:
			//形状の更新処理
			shape_->Update();
			break;
		default:
			break;
		}
	}

	void Object3d::Draw(BaseCamera* _camera, SceneLight* _sceneLight) {
		//オブジェクトの種類ごとの処理
		switch (objKind_) {
		case ObjectKind::Model:
		{
			//通常モデル用共通描画の設定
			Object3dManager::GetInstance()->SettingCommonDrawing(Object3dManager::NameGPS::Normal);
			//シーンライト有無設定
			objectResource_.lightFlagData->isActiveLights = (isLightProcess_) ? true : false;

			//lightFlagCbufferの場所を設定
			MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(6, objectResource_.lightFlagResource->GetGPUVirtualAddress());

			//SceneLightCBufferの場所を設定
			if (objectResource_.lightFlagData->isActiveLights) {
				MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(5, _sceneLight->GetSceneLightConstBuffer()->GetGPUVirtualAddress());
			}

			//WorldTransformの設定
			{
				//データに入れる処理
				uint32_t index = 0u;
				for (auto it = worldTransforms_.begin(); it != worldTransforms_.end(); ) {
					WorldTransform* worldTransform = it->second;
					//nullptrなら削除
					if (worldTransform == nullptr) {
						it = worldTransforms_.erase(it);
						continue;
					}
					//ワールド行列
					Matrix4x4 matWorld = worldTransform->GetWorldMatrix();
					objectResource_.instancingData[index].matWorld =
						matWorld;
					objectResource_.instancingData[index].matWorldInverseTranspose =
						MyMath::Transpose(MyMath::Inverse(matWorld));

					++index;
					++it;
				}
				//一つも登録されていなかったらassert
				assert(index && "WorldTransform情報がセットされていません");
				//GPUに送信
				MainRender::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(1,GPUDescriptorManager::GetInstance()->GetGPUDescriptorHandle(objectResource_.instancingSrvIndex));
			}

			//CameraからビュープロジェクションCBufferの場所設定
			MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(2, _camera->GetViewProjectionConstBuffer()->GetGPUVirtualAddress());

			//Cameraからカメラ座標CBufferの場所を設定
			MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(4, _camera->GetCameraPositionConstBuffer()->GetGPUVirtualAddress());

			//環境光テクスチャの設定
			if (environmentLightTextureHandle_ != EOF) {
				objectResource_.lightFlagData->isActiveEnvironment = true;
				//PSにテクスチャ情報を送る
				MainRender::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(7, TextureManager::GetInstance()->GetSrvHandleGPU(environmentLightTextureHandle_));
			}
			else {
				objectResource_.lightFlagData->isActiveEnvironment = false;
			}

			//モデルを描画する
			model_->Draw(color_, 0, 3, (uint32_t)worldTransforms_.size(), textureHandle_);
			break;
		}
		case ObjectKind::AnimationModel:
		{
			//通常モデル用共通描画の設定
			Object3dManager::GetInstance()->SettingCommonDrawing(Object3dManager::NameGPS::Normal);

			//シーンライト有無設定
			objectResource_.lightFlagData->isActiveLights = (isLightProcess_) ? true : false;

			//lightFlagCbufferの場所を設定
			MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(6, objectResource_.lightFlagResource->GetGPUVirtualAddress());

			//SceneLightCBufferの場所を設定
			if (objectResource_.lightFlagData->isActiveLights) {
				MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(5, _sceneLight->GetSceneLightConstBuffer()->GetGPUVirtualAddress());
			}

			//WorldTransformの設定
			{
				//データに入れる処理
				uint32_t index = 0u;
				for (auto it = worldTransforms_.begin(); it != worldTransforms_.end(); ) {
					WorldTransform* worldTransform = it->second;
					//nullptrなら削除
					if (worldTransform == nullptr) {
						it = worldTransforms_.erase(it);
						continue;
					}
					//ワールド行列
					Matrix4x4 matWorld = worldTransform->GetWorldMatrix();
					objectResource_.instancingData[index].matWorld =
						matWorld;
					objectResource_.instancingData[index].matWorldInverseTranspose =
						MyMath::Transpose(MyMath::Inverse(matWorld));

					++index;
					++it;
				}
				//一つも登録されていなかったらassert
				assert(index && "WorldTransform情報がセットされていません");
				//GPUに送信
				MainRender::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(1, GPUDescriptorManager::GetInstance()->GetGPUDescriptorHandle(objectResource_.instancingSrvIndex));
			}

			//CameraからビュープロジェクションCBufferの場所設定
			MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(2, _camera->GetViewProjectionConstBuffer()->GetGPUVirtualAddress());

			//Cameraからカメラ座標CBufferの場所を設定
			MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(4, _camera->GetCameraPositionConstBuffer()->GetGPUVirtualAddress());

			//環境光テクスチャの設定
			if (environmentLightTextureHandle_ != EOF) {
				objectResource_.lightFlagData->isActiveEnvironment = true;
				//PSにテクスチャ情報を送る
				MainRender::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(7, TextureManager::GetInstance()->GetSrvHandleGPU(environmentLightTextureHandle_));
			}
			else {
				objectResource_.lightFlagData->isActiveEnvironment = false;
			}

			//モデルを描画する
			animationModel_->Draw(0, 3, (uint32_t)worldTransforms_.size(), textureHandle_);

			//CS描画後処理(スキニング)
			animationModel_->SettingCSPostDraw();

			break;
		}
		case ObjectKind::Shape:
		{
			//描画前設定
			if (shape_->GetShapeKind() == Shape::kSkyBox) {
				//SkyBoxの描画設定
				Object3dManager::GetInstance()->SettingCommonDrawing(Object3dManager::NameGPS::SkyBox);
			}
			else {
				//通常の描画設定
				Object3dManager::GetInstance()->SettingCommonDrawing(Object3dManager::NameGPS::Normal);
			}

			//シーンライト有無設定
			objectResource_.lightFlagData->isActiveLights = (isLightProcess_) ? true : false;
			//lightFlagCbufferの場所を設定
			MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(6, objectResource_.lightFlagResource->GetGPUVirtualAddress());
			//SceneLightCBufferの場所を設定
			if (objectResource_.lightFlagData->isActiveLights) {
				MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(5, _sceneLight->GetSceneLightConstBuffer()->GetGPUVirtualAddress());
			}

			//WorldTransformの設定
			{
				//データに入れる処理
				uint32_t index = 0u;
				for (auto it = worldTransforms_.begin(); it != worldTransforms_.end(); ) {
					WorldTransform* worldTransform = it->second;
					//nullptrなら削除
					if (worldTransform == nullptr) {
						it = worldTransforms_.erase(it);
						continue;
					}
					//ワールド行列
					Matrix4x4 matWorld = worldTransform->GetWorldMatrix();
					objectResource_.instancingData[index].matWorld =
						matWorld;
					objectResource_.instancingData[index].matWorldInverseTranspose =
						MyMath::Transpose(MyMath::Inverse(matWorld));

					++index;
					++it;
				}
				//一つも登録されていなかったらassert
				assert(index && "WorldTransform情報がセットされていません");
				//GPUに送信
				MainRender::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(1, GPUDescriptorManager::GetInstance()->GetGPUDescriptorHandle(objectResource_.instancingSrvIndex));
			}

			//CameraからビュープロジェクションCBufferの場所設定
			MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(2, _camera->GetViewProjectionConstBuffer()->GetGPUVirtualAddress());
			//Cameraからカメラ座標CBufferの場所を設定
			MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(4, _camera->GetCameraPositionConstBuffer()->GetGPUVirtualAddress());
			//環境光テクスチャの設定
			if (environmentLightTextureHandle_ != EOF) {
				objectResource_.lightFlagData->isActiveEnvironment = true;
				//PSにテクスチャ情報を送る
				MainRender::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(7, TextureManager::GetInstance()->GetSrvHandleGPU(environmentLightTextureHandle_));
			}
			else {
				objectResource_.lightFlagData->isActiveEnvironment = false;
			}

			//形状を描画する
			shape_->Draw(0, 3, (uint32_t)worldTransforms_.size(), textureHandle_);

			break;
		}
		default:
			break;
		}
	}

	void Object3d::SetNewAnimation(const std::string& _name, const std::string& _filePath) {
		//アニメーションモデル以外のオブジェクトで初期化していた場合警告
		if (objKind_ != ObjectKind::AnimationModel) {
			assert(0 && "アニメーションモデル以外の初期化を確認しました。");
			return;
		}
		//AnimationModelに通達
		animationModel_->SetNewAnimation(_name, _filePath);
	}

	void Object3d::SetCurrentAnimation(const std::string& _name) {
		//アニメーションモデル以外のオブジェクトで初期化していた場合警告
		if (objKind_ != ObjectKind::AnimationModel) {
			assert(0 && "アニメーションモデル以外の初期化を確認しました。");
			return;
		}
		//AnimationModelに通達
		animationModel_->SetCurrentAnimation(_name);
	}

	void Object3d::SetColor(const Vector4& _color) {
		//色をセット
		color_ = _color;

		//オブジェクトの種類ごとに分けてセット
		switch (objKind_) {
		case ObjectKind::Model:
		{
			//モデルは個別で色を持たないためDrawでセットする
			break;
		}
		case ObjectKind::AnimationModel:
		{
			//色をセット
			animationModel_->SetColor(color_);
			break;
		}
		case ObjectKind::Shape:
		{
			//色をセット
			shape_->SetColor(color_);
			break;
		}
		default:
			break;
		}
	}

	Object3d::ObjectResource Object3d::CreateObjectResource() {
		ObjectResource result;
		DirectXCommon* dxCommon = DirectXCommon::GetInstance();
		GPUDescriptorManager* gpuDescriptorManager = GPUDescriptorManager::GetInstance();

		//インスタンシング
		{
			//エミッター情報用のResorceを確保
			result.instancingResource = dxCommon->CreateBufferResource(sizeof(InstancingForGPU) * kMaxInstancingNum);
			//マッピング
			result.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&result.instancingData));
			//インスタンシング情報用のsrvを作成。StructuredBufferでアクセスできるようにする
			result.instancingSrvIndex = gpuDescriptorManager->Allocate();
			gpuDescriptorManager->CreateSRVforStructuredBuffer(result.instancingSrvIndex, result.instancingResource.Get(), UINT(kMaxInstancingNum), sizeof(InstancingForGPU));
		}
		//ライトフラグ
		{
			//ライト情報用のResorceを確保
			result.lightFlagResource = dxCommon->CreateBufferResource(sizeof(FlagForGPU));
			//マッピング
			result.lightFlagResource->Map(0, nullptr, reinterpret_cast<void**>(&result.lightFlagData));
			//データ入力
			result.lightFlagData->isActiveLights = false;
			result.lightFlagData->isActiveEnvironment = false;
		}

		return result;
	}

}