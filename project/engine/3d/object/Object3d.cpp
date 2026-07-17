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
#include "StringUtility.h"
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
		//メッシュの種類を決定
		meshType_ = MeshType::Model;
		//名前
		name_ = name;
		//ファイルパスをセット
		filePath_ = filePath;

		//モデルマネージャーでモデルを生成
		ModelManager::GetInstance()->LoadModel(filePath_, ModelFormat::OBJ);
		//モデルマネージャーから検索してセットする
		model_ = ModelManager::GetInstance()->FindModel(filePath_);


		//マネージャーに登録
		Object3dManager::GetInstance()->RegisterObject(name_, this);
	}

	void Object3d::Initialize(AnimationModelTag, const std::string& name, const std::string& filePath) {
		//メッシュの種類を決定
		meshType_ = MeshType::AnimationModel;
		//名前
		name_ = name;
		//ファイルパスをセット
		filePath_ = filePath;

		//アニメーションモデルの生成と初期化
		animationModel_ = std::make_unique<AnimationModel>();
		animationModel_->Initialize(filePath_, ModelFormat::GLTF);

		//マネージャーに登録
		Object3dManager::GetInstance()->RegisterObject(name_, this);
	}

	void Object3d::Initialize(ShapeTag, const std::string& name, Shape::ShapeKind kind) {
		//メッシュの種類を決定
		meshType_ = MeshType::Shape;
		//名前
		name_ = name;

		//形状の生成と初期化
		shape_ = std::make_unique<Shape>();
		shape_->Initialize(kind);

		//マネージャーに登録
		Object3dManager::GetInstance()->RegisterObject(name_, this);
	}

	void Object3d::Debug(const std::wstring& _name) {
#ifdef _DEBUG
		ImGui::Begin("3Dオブジェクト");
		if (ImGui::CollapsingHeader(StringUtility::ConvertString(_name).c_str())) {
			//トランスフォームの編集
			if (!worldTransforms_.empty()) {
				auto& worldTransform = *worldTransforms_.begin()->second;
				Vector3 translate = worldTransform.GetTranslate();
				Vector3 rotate = worldTransform.GetRotate();
				Vector3 scale = worldTransform.GetScale();

				ImGui::DragFloat3(
					"座標",
					&translate.x,
					0.1f
				);
				ImGui::DragFloat3(
					"回転",
					&rotate.x,
					0.1f
				);
				ImGui::DragFloat3(
					"スケール",
					&scale.x,
					0.1f
				);

				worldTransform.SetTranslate(translate);
				worldTransform.SetRotate(rotate);
				worldTransform.SetScale(scale);
			}


		}

		ImGui::End();
#endif // _DEBUG

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
		//アウトラインオブジェクト更新
		if (outlineObject_) {
			//worldTransforms_の先頭のワールドトランスフォームを取得
			auto it = worldTransforms_.begin();
			//トランスフォームをアウトラインオブジェクトに反映
			if (it != worldTransforms_.end() && it->second) {
				Vector3 translate = it->second->GetTranslate();
				Vector3 rotate = it->second->GetRotate();
				Vector3 scale = it->second->GetScale()*olSize_;
				olWT_.SetTranslate(translate);
				olWT_.SetRotate(rotate);
				olWT_.SetScale(scale);
			}
		}

		//メッシュの種類ごとの処理
		switch (meshType_) {
		case MeshType::Model:
			//何もなし
			break;
		case MeshType::AnimationModel:
			//アニメーション反映処理
			animationModel_->Update();
			//CS前処理（スキニング）
			animationModel_->SettingCSPreDraw();
			break;
		case MeshType::Shape:
			//形状の更新処理
			shape_->Update();
			break;
		default:
			break;
		}
	}

	void Object3d::Draw(BaseCamera* _camera, SceneLight* _sceneLight) {
		//使用するPSOを決定
		NameGPS nameGPS = NameGPS::Normal;
		if(meshType_ == MeshType::Shape && shape_->GetShapeKind()==Shape::kSkyBox) {
			nameGPS = NameGPS::SkyBox;
		}

		//共通描画設定
		Object3dManager::GetInstance()->SettingCommonDrawing(nameGPS, stencilRole_);

		//ステンシルRef
		{
			if(stencilRole_ == StencilRole::OlTarget) {
				MainRender::GetInstance()->GetCommandList()->OMSetStencilRef(2);
			}
			else if (stencilRole_ == StencilRole::Outline) {
				MainRender::GetInstance()->GetCommandList()->OMSetStencilRef(1);
			}
			else {
				MainRender::GetInstance()->GetCommandList()->OMSetStencilRef(0);
			}
		}
		//ライト
		{
			//シーンライト有無設定
			objectResource_.lightFlagData->isActiveLights = (isLightProcess_) ? true : false;
			//lightFlagCbufferの場所を設定
			MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(6, objectResource_.lightFlagResource->GetGPUVirtualAddress());
			//SceneLightCBufferの場所を設定
			if (objectResource_.lightFlagData->isActiveLights) {
				MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(5, _sceneLight->GetSceneLightConstBuffer()->GetGPUVirtualAddress());
			}
		}
		//ワールドトランスフォーム
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
		//カメラ
		{
			//CameraからビュープロジェクションCBufferの場所設定
			MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(2, _camera->GetViewProjectionConstBuffer()->GetGPUVirtualAddress());
			//Cameraからカメラ座標CBufferの場所を設定
			MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(4, _camera->GetCameraPositionConstBuffer()->GetGPUVirtualAddress());
		}
		//環境光テクスチャ
		{
			//環境光テクスチャの設定
			if (environmentLightTextureHandle_ != EOF) {
				objectResource_.lightFlagData->isActiveEnvironment = true;
				//PSにテクスチャ情報を送る
				MainRender::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(7, TextureManager::GetInstance()->GetSrvHandleGPU(environmentLightTextureHandle_));
			}
			else {
				objectResource_.lightFlagData->isActiveEnvironment = false;
			}
		}

		//メッシュの種類ごとの処理
		switch (meshType_) {
		case MeshType::Model:
		{
			//モデルを描画する
			model_->Draw(color_, 0, 3, (uint32_t)worldTransforms_.size(), textureHandle_);

			break;
		}
		case MeshType::AnimationModel:
		{
			//モデルを描画する
			animationModel_->Draw(0, 3, (uint32_t)worldTransforms_.size(), textureHandle_);
			//CS描画後処理(スキニング)
			animationModel_->SettingCSPostDraw();

			break;
		}
		case MeshType::Shape:
		{
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
		if (meshType_ != MeshType::AnimationModel) {
			assert(0 && "アニメーションモデル以外の初期化を確認しました。");
			return;
		}
		//AnimationModelに通達
		animationModel_->SetNewAnimation(_name, _filePath);
	}

	void Object3d::SetCurrentAnimation(const std::string& _name) {
		//アニメーションモデル以外のオブジェクトで初期化していた場合警告
		if (meshType_ != MeshType::AnimationModel) {
			assert(0 && "アニメーションモデル以外の初期化を確認しました。");
			return;
		}
		//AnimationModelに通達
		animationModel_->SetCurrentAnimation(_name);
	}

	void Object3d::SetColor(const Vector4& _color) {
		//色をセット
		color_ = _color;

		//メッシュの種類ごとに分けてセット
		switch (meshType_) {
		case MeshType::Model:
		{
			//モデルは個別で色を持たないためDrawでセットする
			break;
		}
		case MeshType::AnimationModel:
		{
			//色をセット
			animationModel_->SetColor(color_);
			break;
		}
		case MeshType::Shape:
		{
			//色をセット
			shape_->SetColor(color_);
			break;
		}
		default:
			break;
		}
	}

	void Object3d::SetIsOutline(bool _isOutline) {
		isOutline_ = _isOutline;

		if (isOutline_) {
			//アウトラインオブジェクトを生成
			outlineObject_ = std::make_unique<Object3d>();
			//ステンシルの役割をセット
			outlineObject_->SetStencilRole(StencilRole::Outline);
			//WTの登録
			olWT_.Initialize();
			outlineObject_->RegistWorldTransform(&olWT_);
			//meshType_に合わせて初期化
			switch (meshType_) {
			case MeshType::Model:
				outlineObject_->Initialize(ModelTag{}, name_ + "_outline", filePath_);
				break;
			case MeshType::AnimationModel:
				outlineObject_->Initialize(AnimationModelTag{}, name_ + "_outline", filePath_);
				break;
			case MeshType::Shape:
				outlineObject_->Initialize(ShapeTag{}, name_ + "_outline", shape_->GetShapeKind());
				break;
			default:
				break;
			}
			//テクスチャ
			outlineObject_->SetTexture(TextureManager::GetInstance()->LoadTexture("white.png"));
			//ライト処理
			outlineObject_->SetIsLightProcess(false);
			//描画
			outlineObject_->SetIsDisplay(true);

			//ステンシルの役割をアウトライン適用モデルにする
			SetStencilRole(StencilRole::OlTarget);

		}
		else {
			//アウトラインオブジェクトを破棄
			outlineObject_.reset();
			//ステンシルの役割を通常にする
			SetStencilRole(StencilRole::Normal);
		}
	}

	void Object3d::SetOutlineParam(uint32_t textureHandle, float _size) {
		if (outlineObject_) {
			//アウトラインオブジェクトの色をセット
			outlineObject_->SetTexture(textureHandle);
			//アウトラインオブジェクトのスケールをセット
			olSize_ = _size;
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