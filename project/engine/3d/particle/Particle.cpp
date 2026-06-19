#include "Particle.h"
#include "DirectXCommon.h"
#include "GPUDescriptorManager.h"
#include "ModelManager.h"
#include "ParticleManager.h"
#include "TextureManager.h"
#include "JsonUtil.h"
#include "SceneManager.h"

namespace Norm {

	Particle::Particle() {
		//シーンタグの初期化
		sceneTag_ = SceneManager::GetInstance()->GetCurrentScene()->GetSceneName();
	}

	Particle::~Particle() {
		//マネージャーから削除
		ParticleManager::GetInstance()->Delete(name_);
	}

	void Particle::Initialize(const std::string& name, const std::string& fileName) {
		//名前を登録
		name_ = name;
		//パラメータをセット
		auto data = JsonUtil::GetJsonData("Resources/particles/" + fileName);
		//JSONファイルの読み込み
		if (data) param_ = data;
		else assert(0 && "JSONファイルが存在しません");
		//エミッターの初期化
		emitter_.worldTransform.translate = { 0,0,0 };
		emitter_.worldTransform.rotate = { 0,0,0 };
		emitter_.worldTransform.scale = { 1,1,1 };
		emitter_.isAffectedField = false;
		emitter_.isPlay = false;
		//テクスチャハンドルの取得
		textureHandle_ = TextureManager::GetInstance()->LoadTexture(param_["Texture"]);

		//形状を生成
		shape_ = std::make_unique<Shape>();
		//形状の初期化
		Shape::ShapeKind shapeKind = (Shape::ShapeKind)param_["Primitive"];
		shape_->Initialize(shapeKind);

		//エミッターの情報を写す
		TraceEmitterForCS();
		//JSONの情報を写す
		TraceJsonInfoForCS();

		//個別のCS用リソースの作成
		eachResourceForCS_ = CreateEachResourceForCS();

		//最後にマネージャーに登録
		ParticleManager::GetInstance()->Regist(name_, this);
	}

	void Particle::ShapeChange() {
		//JSONをもとにShapeを作り直す
		shape_ = std::make_unique<Shape>();
		//形状の初期化
		Shape::ShapeKind shapeKind = (Shape::ShapeKind)param_["Primitive"];
		shape_->Initialize(shapeKind);
	}

	void Particle::TextureChange() {
		//テクスチャハンドルの取得
		textureHandle_ = TextureManager::GetInstance()->LoadTexture(param_["Texture"]);
	}

	void Particle::TraceEmitterForCS() {
		auto Vec3ToVec4 = [](const Vector3& j) -> Vector4 {
			return { j.x,j.y,j.z,0.0f };
			};
		emitterForCS_.worldTransform.scale = Vec3ToVec4(emitter_.worldTransform.scale);
		emitterForCS_.worldTransform.rotate = Vec3ToVec4(emitter_.worldTransform.rotate);
		emitterForCS_.worldTransform.translate = Vec3ToVec4(emitter_.worldTransform.translate);
		emitterForCS_.isAffectedField = emitter_.isAffectedField;
		emitterForCS_.isPlay = emitter_.isPlay;
		emitterForCS_.isAlive = 1u;
	}

	void Particle::TraceJsonInfoForCS() {
		//データ入力
		auto Vec3ToVec4 = [](const auto& j) -> Vector4 {
			return { (float)j["x"], (float)j["y"], (float)j["z"], 0.0f };
			};
		auto Vec4ToVec4 = [](const auto& j) -> Vector4 {
			return { (float)j["x"], (float)j["y"],(float)j["z"],(float)j["w"] };
			};
		jsonInfoForCS_.localTransform.scale = Vec4ToVec4(param_["LocalTransform"]["Scale"]);
		jsonInfoForCS_.localTransform.rotate = Vec4ToVec4(param_["LocalTransform"]["Rotate"]);
		jsonInfoForCS_.localTransform.translate = Vec4ToVec4(param_["LocalTransform"]["Translate"]);
		jsonInfoForCS_.velocityMax = Vec3ToVec4(param_["Velocity"]["Max"]);
		jsonInfoForCS_.velocityMin = Vec3ToVec4(param_["Velocity"]["Min"]);
		jsonInfoForCS_.initRotateMax = Vec3ToVec4(param_["GrainTransform"]["Rotate"]["Max"]);
		jsonInfoForCS_.initRotateMin = Vec3ToVec4(param_["GrainTransform"]["Rotate"]["Min"]);
		jsonInfoForCS_.initScaleMax = Vec3ToVec4(param_["GrainTransform"]["Scale"]["Max"]);
		jsonInfoForCS_.initScaleMin = Vec3ToVec4(param_["GrainTransform"]["Scale"]["Min"]);
		jsonInfoForCS_.startColorMax = Vec4ToVec4(param_["StartColor"]["Max"]);
		jsonInfoForCS_.startColorMin = Vec4ToVec4(param_["StartColor"]["Min"]);
		jsonInfoForCS_.endColorMax = Vec4ToVec4(param_["EndColor"]["Max"]);
		jsonInfoForCS_.endColorMin = Vec4ToVec4(param_["EndColor"]["Min"]);
		jsonInfoForCS_.angularVelocityMax = Vec3ToVec4(param_["AngularVelocity"]["Max"]);
		jsonInfoForCS_.angularVelocityMin = Vec3ToVec4(param_["AngularVelocity"]["Min"]);
		jsonInfoForCS_.sizeVelocityMax = param_["SizeVelocity"]["Max"];
		jsonInfoForCS_.sizeVelocityMin = param_["SizeVelocity"]["Min"];
		jsonInfoForCS_.lifeTimeMax = param_["LifeTime"]["Max"];
		jsonInfoForCS_.lifeTimeMin = param_["LifeTime"]["Min"];
		jsonInfoForCS_.gravity = param_["Gravity"];
		jsonInfoForCS_.repulsion = param_["Repulsion"];
		jsonInfoForCS_.floorHeight = param_["FloorHeight"];
		jsonInfoForCS_.emitRate = param_["EmitRate"];
		jsonInfoForCS_.maxGrains = param_["MaxGrains"];
		jsonInfoForCS_.generateMethod = param_["GenerateMethod"];
		jsonInfoForCS_.clumpNum = param_["ClumpNum"];
		jsonInfoForCS_.effectStyle = param_["EffectStyle"];
		jsonInfoForCS_.isGravity = param_["IsGravity"];
		jsonInfoForCS_.isBound = param_["IsBound"];
		jsonInfoForCS_.isBillboard = param_["IsBillboard"];
	}

	Particle::EachResourceForCS Particle::CreateEachResourceForCS() {
		EachResourceForCS result;
		DirectXCommon* dxCommon = DirectXCommon::GetInstance();

		//エミッターID
		{
			result.emitterIDResource = dxCommon->CreateBufferResource(sizeof(TargetEmitterIDForVS));
			TargetEmitterIDForVS* mappedEmitterID = nullptr;
			result.emitterIDResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedEmitterID));
			std::memset(mappedEmitterID, 0, sizeof(TargetEmitterIDForVS));
			result.mappedEmitterID = { mappedEmitterID,1 };
			//データ入力
			result.mappedEmitterID[0].id = emitterID_;
		}

		return result;
	}

}