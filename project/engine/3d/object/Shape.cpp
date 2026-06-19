#include "Shape.h"
#include "DirectXCommon.h"
#include "MainRender.h"
#include "TextureManager.h"
#include <cmath>
#include <numbers>
#include <cassert>

namespace Norm {

	void Shape::Initialize(ShapeKind shapeKind) {
		//形状の種類
		shapeKind_ = shapeKind;
		//形状リソースの初期設定
		shapeResource_ = MakeShapeResource();
	}

	void Shape::Update() {
	}

	void Shape::Draw(uint32_t materialRootParameterIndex, uint32_t textureRootParameterIndex, uint32_t instancingNum, int32_t textureHandle) {
		//頂点バッファビューを設定
		MainRender::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &shapeResource_.vertexBufferView);
		//マテリアルCBufferの場所を特定
		if (color_) {
			shapeResource_.materialData->color = *color_;
		}
		MainRender::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(materialRootParameterIndex, shapeResource_.materialResource->GetGPUVirtualAddress());
		//テクスチャがない場合は飛ばす
		shapeResource_.materialData->isTexture = (textureHandle != EOF) ? true : false;
		if (shapeResource_.materialData->isTexture) {
			MainRender::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(textureRootParameterIndex, TextureManager::GetInstance()->GetSrvHandleGPU(textureHandle));
		}
		//描画
		MainRender::GetInstance()->GetCommandList()->DrawInstanced(shapeResource_.vertexNum, instancingNum, 0, 0);
	}

	Shape::ShapeResource Shape::MakeShapeResource() {
		//形状リソース
		ShapeResource resource;

		switch (shapeKind_) {
		case ShapeKind::kSphere:
			resource = MakeSphereResource();
			break;
		case ShapeKind::kCube:
			resource = MakeCubeResource();
			break;
		case ShapeKind::kSkyBox:
			resource = MakeSkyBoxResource();
			break;
		case ShapeKind::kPlane:
			resource = MakePlaneResource();
			break;
		case ShapeKind::kRing:
			resource = MakeRingResource();
			break;
		case ShapeKind::kTube:
			resource = MakeTubeResource();
			break;
		default:
			assert(0 && "未確認の形状が入力されています。");
			break;
		}

		return resource;
	}

	Shape::ShapeResource Shape::MakeSphereResource() {
		ShapeResource resource;
		//ローカル変数
		float pi = std::numbers::pi_v<float>;
		const float kLonEvery = pi * 2.0f / float(kSubdivision);
		const float kLatEvery = pi / float(kSubdivision);
		Sphere sphere;
		sphere.center = { 0.0f,0.0f,0.0f };
		sphere.radius = 1.0f;
		//頂点数を保持
		resource.vertexNum = kSubdivision * kSubdivision * 6;
		//リソースを作る
		resource.vertexResource = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(VertexData) * resource.vertexNum);
		resource.materialResource = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(Material));
		//頂点バッファビューを作成
		resource.vertexBufferView.BufferLocation = resource.vertexResource->GetGPUVirtualAddress();
		resource.vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * resource.vertexNum);
		resource.vertexBufferView.StrideInBytes = sizeof(VertexData);
		//リソースにデータをマッピング
		resource.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&resource.vertexData));
		resource.materialResource->Map(0, nullptr, reinterpret_cast<void**>(&resource.materialData));
		//データに書き込み
		//緯度の方向に分割
		for (UINT latIndex = 0; latIndex < kSubdivision; ++latIndex) {
			float lat = -pi / 2.0f + kLatEvery * latIndex;
			//緯度の方向に分割
			for (UINT lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
				uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
				float lon = lonIndex * kLonEvery;
				///三角形一枚目
				//頂点にデータを入れる
				resource.vertexData[start].position.x = sphere.center.x + sphere.radius * cos(lat) * cos(lon);
				resource.vertexData[start].position.y = sphere.center.y + sphere.radius * sin(lat);
				resource.vertexData[start].position.z = sphere.center.z + sphere.radius * cos(lat) * sin(lon);
				resource.vertexData[start].position.w = 1.0f;
				resource.vertexData[start].texcoord.x = float(lonIndex) / float(kSubdivision);
				resource.vertexData[start].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
				//頂点にデータを入れる
				resource.vertexData[start + 1].position.x = sphere.center.x + sphere.radius * cos(lat + kLatEvery) * cos(lon);
				resource.vertexData[start + 1].position.y = sphere.center.y + sphere.radius * sin(lat + kLatEvery);
				resource.vertexData[start + 1].position.z = sphere.center.z + sphere.radius * cos(lat + kLatEvery) * sin(lon);
				resource.vertexData[start + 1].position.w = 1.0f;
				resource.vertexData[start + 1].texcoord.x = float(lonIndex) / float(kSubdivision);
				resource.vertexData[start + 1].texcoord.y = 1.0f - float(latIndex + 1) / float(kSubdivision);
				//頂点にデータを入れる
				resource.vertexData[start + 2].position.x = sphere.center.x + sphere.radius * cos(lat) * cos(lon + kLonEvery);
				resource.vertexData[start + 2].position.y = sphere.center.y + sphere.radius * sin(lat);
				resource.vertexData[start + 2].position.z = sphere.center.z + sphere.radius * cos(lat) * sin(lon + kLonEvery);
				resource.vertexData[start + 2].position.w = 1.0f;
				resource.vertexData[start + 2].texcoord.x = float(lonIndex + 1) / float(kSubdivision);
				resource.vertexData[start + 2].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
				///三角形二枚目
				//頂点にデータを入れる
				resource.vertexData[start + 3].position.x = sphere.center.x + sphere.radius * cos(lat + kLatEvery) * cos(lon);
				resource.vertexData[start + 3].position.y = sphere.center.y + sphere.radius * sin(lat + kLatEvery);
				resource.vertexData[start + 3].position.z = sphere.center.z + sphere.radius * cos(lat + kLatEvery) * sin(lon);
				resource.vertexData[start + 3].position.w = 1.0f;
				resource.vertexData[start + 3].texcoord.x = float(lonIndex) / float(kSubdivision);
				resource.vertexData[start + 3].texcoord.y = 1.0f - float(latIndex + 1) / float(kSubdivision);
				//頂点にデータを入れる
				resource.vertexData[start + 4].position.x = sphere.center.x + sphere.radius * cos(lat + kLatEvery) * cos(lon + kLonEvery);
				resource.vertexData[start + 4].position.y = sphere.center.y + sphere.radius * sin(lat + kLatEvery);
				resource.vertexData[start + 4].position.z = sphere.center.z + sphere.radius * cos(lat + kLatEvery) * sin(lon + kLonEvery);
				resource.vertexData[start + 4].position.w = 1.0f;
				resource.vertexData[start + 4].texcoord.x = float(lonIndex + 1) / float(kSubdivision);
				resource.vertexData[start + 4].texcoord.y = 1.0f - float(latIndex + 1) / float(kSubdivision);
				//頂点にデータを入れる
				resource.vertexData[start + 5].position.x = sphere.center.x + sphere.radius * cos(lat) * cos(lon + kLonEvery);
				resource.vertexData[start + 5].position.y = sphere.center.y + sphere.radius * sin(lat);
				resource.vertexData[start + 5].position.z = sphere.center.z + sphere.radius * cos(lat) * sin(lon + kLonEvery);
				resource.vertexData[start + 5].position.w = 1.0f;
				resource.vertexData[start + 5].texcoord.x = float(lonIndex + 1) / float(kSubdivision);
				resource.vertexData[start + 5].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);

				//uvフリップ
				for (uint32_t i = 0; i < 6; i++) {
					resource.vertexData[start + i].texcoord.y = 1.0f - resource.vertexData[start + i].texcoord.y;
				}

				//法線情報の入力
				for (uint32_t i = 0; i < 6; i++) {
					resource.vertexData[start + i].normal.x = resource.vertexData[start + i].position.x;
					resource.vertexData[start + i].normal.y = resource.vertexData[start + i].position.y;
					resource.vertexData[start + i].normal.z = resource.vertexData[start + i].position.z;
				}

				//メモ//
				//三角形描画時には頂点を半時計周りの順番で設定する。
				//時計回りにすると表裏が逆になってしまう。
			}
		}
		resource.materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		resource.materialData->isTexture = false;
		resource.materialData->shininess = 20.0f;
		resource.materialData->uvTransform = MyMath::MakeIdentity4x4();
		return resource;
	}

	Shape::ShapeResource Shape::MakeCubeResource() {
		ShapeResource resource;
		auto dxCommon = DirectXCommon::GetInstance();

		// 頂点数（6面 × 2三角形 × 3頂点 = 36）
		resource.vertexNum = 36;

		// バッファ作成
		size_t vbSize = sizeof(VertexData) * resource.vertexNum;
		resource.vertexResource = dxCommon->CreateBufferResource(vbSize);
		resource.materialResource = dxCommon->CreateBufferResource(sizeof(Material));

		// バッファビュー設定
		resource.vertexBufferView = {
			resource.vertexResource->GetGPUVirtualAddress(),
			static_cast<UINT>(vbSize),
			sizeof(VertexData)
		};

		// マッピング
		resource.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&resource.vertexData));
		resource.materialResource->Map(0, nullptr, reinterpret_cast<void**>(&resource.materialData));

		// 頂点定義（各面に法線を設定）
		static const Vector4 kPositions[36] = {
			// +X面（右）→ CCW
			{1,1,-1,1}, {1,1,1,1}, {1,-1,1,1},
			{1,1,-1,1}, {1,-1,1,1}, {1,-1,-1,1},

			// -X面（左）→ CCW
			{-1,1,1,1}, {-1,1,-1,1}, {-1,-1,-1,1},
			{-1,1,1,1}, {-1,-1,-1,1}, {-1,-1,1,1},

			// +Z面（前）→ CCW
			{1,1,1,1}, {-1,1,1,1}, {-1,-1,1,1},
			{1,1,1,1}, {-1,-1,1,1}, {1,-1,1,1},

			// -Z面（後）→ CCW
			{-1,1,-1,1}, {1,1,-1,1}, {1,-1,-1,1},
			{-1,1,-1,1}, {1,-1,-1,1}, {-1,-1,-1,1},

			// +Y面（上）→ CCW
			{-1,1,1,1}, {1,1,-1,1}, {-1,1,-1,1},
			{-1,1,1,1}, {1,1,1,1}, {1,1,-1,1},

			// -Y面（下）→ CCW
			{1,-1,1,1}, {-1,-1,-1,1}, {1,-1,-1,1},
			{1,-1,1,1}, {-1,-1,1,1}, {-1,-1,-1,1},
		};

		static const Vector3 kNormals[6] = {
			{1, 0, 0},   // +X
			{-1, 0, 0},  // -X
			{0, 0, 1},   // +Z
			{0, 0, -1},  // -Z
			{0, 1, 0},   // +Y
			{0, -1, 0},  // -Y
		};

		for (int i = 0; i < 36; ++i) {
			resource.vertexData[i].position = kPositions[i];
			resource.vertexData[i].texcoord = { 0.0f, 0.0f };

			// 法線（各6頂点ずつ同じ面）
			int face = i / 6;
			resource.vertexData[i].normal = kNormals[face];
		}

		// マテリアル設定
		resource.materialData->color = { 1,1,1,1 };
		resource.materialData->isTexture = false;
		resource.materialData->shininess = 32.0f;
		resource.materialData->uvTransform = MyMath::MakeIdentity4x4();
		return resource;
	}

	Shape::ShapeResource Shape::MakeSkyBoxResource() {
		ShapeResource resource;
		auto dxCommon = DirectXCommon::GetInstance();

		// 頂点数
		resource.vertexNum = 36;

		// バッファ作成
		size_t vbSize = sizeof(VertexData) * resource.vertexNum;
		resource.vertexResource = dxCommon->CreateBufferResource(vbSize);
		resource.materialResource = dxCommon->CreateBufferResource(sizeof(Material));

		// 頂点バッファビュー
		resource.vertexBufferView = {
			resource.vertexResource->GetGPUVirtualAddress(),
			static_cast<UINT>(vbSize),
			sizeof(VertexData)
		};

		// マッピング
		resource.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&resource.vertexData));
		resource.materialResource->Map(0, nullptr, reinterpret_cast<void**>(&resource.materialData));

		// 頂点位置定義
		static const Vector4 kPositions[36] = {
			// 右・左・前・後・上・下（各6頂点ずつ）
			{1,1,1,1}, {1,1,-1,1}, {1,-1,1,1}, {1,-1,1,1}, {1,1,-1,1}, {1,-1,-1,1},
			{-1,1,-1,1}, {-1,1,1,1}, {-1,-1,-1,1}, {-1,-1,-1,1}, {-1,1,1,1}, {-1,-1,1,1},
			{-1,1,1,1}, {1,1,1,1}, {-1,-1,1,1}, {-1,-1,1,1}, {1,1,1,1}, {1,-1,1,1},
			{1,1,-1,1}, {-1,1,-1,1}, {1,-1,-1,1}, {1,-1,-1,1}, {-1,1,-1,1}, {-1,-1,-1,1},
			{-1,1,-1,1}, {1,1,-1,1}, {-1,1,1,1}, {-1,1,1,1}, {1,1,-1,1}, {1,1,1,1},
			{-1,-1,1,1}, {1,-1,1,1}, {-1,-1,-1,1}, {-1,-1,-1,1}, {1,-1,1,1}, {1,-1,-1,1},
		};

		for (int i = 0; i < 36; ++i) {
			resource.vertexData[i].position = kPositions[i];
			resource.vertexData[i].texcoord = { 0,0 };
			resource.vertexData[i].normal = { 0,0,0 };
		}

		// マテリアル設定
		resource.materialData->color = { 1,1,1,1 };
		resource.materialData->isTexture = false;
		resource.materialData->shininess = 20.0f;
		resource.materialData->uvTransform = MyMath::MakeIdentity4x4();

		return resource;
	}

	Shape::ShapeResource Shape::MakePlaneResource() {
		ShapeResource resource;
		//頂点数を保持
		resource.vertexNum = 6;
		//リソースを作る
		resource.vertexResource = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(VertexData) * resource.vertexNum);
		resource.materialResource = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(Material));
		//頂点バッファビューを作成
		resource.vertexBufferView.BufferLocation = resource.vertexResource->GetGPUVirtualAddress();
		resource.vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * resource.vertexNum);
		resource.vertexBufferView.StrideInBytes = sizeof(VertexData);
		//リソースにデータをマッピング
		resource.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&resource.vertexData));
		resource.materialResource->Map(0, nullptr, reinterpret_cast<void**>(&resource.materialData));
		//データに書き込み
		// 三角形一枚目
		resource.vertexData[0].position = { -1.0f, 1.0f, 0.0f, 1.0f };
		resource.vertexData[0].texcoord = { 0.0f, 1.0f };
		resource.vertexData[1].position = { 1.0f, -1.0f, 0.0f, 1.0f };
		resource.vertexData[1].texcoord = { 1.0f, 0.0f };
		resource.vertexData[2].position = { -1.0f, -1.0f, 0.0f, 1.0f };
		resource.vertexData[2].texcoord = { 0.0f, 0.0f };

		// 三角形二枚目
		resource.vertexData[3].position = { -1.0f, 1.0f, 0.0f, 1.0f };
		resource.vertexData[3].texcoord = { 0.0f, 1.0f };
		resource.vertexData[4].position = { 1.0f, 1.0f, 0.0f, 1.0f };
		resource.vertexData[4].texcoord = { 1.0f, 1.0f };
		resource.vertexData[5].position = { 1.0f, -1.0f, 0.0f, 1.0f };
		resource.vertexData[5].texcoord = { 1.0f, 0.0f };

		//uvフリップ
		for (uint32_t i = 0; i < 6; i++) {
			resource.vertexData[i].texcoord.y = 1.0f - resource.vertexData[i].texcoord.y;
		}

		// 法線ベクトル
		for (uint32_t i = 0; i < 6; i++) {
			resource.vertexData[i].normal = { 0.0f, 0.0f, 1.0f };
		}
		//マテリアルデータ
		resource.materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		resource.materialData->isTexture = false;
		resource.materialData->shininess = 20.0f;
		resource.materialData->uvTransform = MyMath::MakeIdentity4x4();

		return resource;
	}

	Shape::ShapeResource Shape::MakeRingResource() {
		ShapeResource resource;
		//ローカル変数
		const uint32_t kRingDivide = 32;
		const float kOuterRadius = 1.0f;
		const float kInnerRadius = 0.2f;
		const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / float(kRingDivide);
		//頂点数を保持
		resource.vertexNum = kRingDivide * 6;
		//リソースを作る
		resource.vertexResource = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(VertexData) * resource.vertexNum);
		resource.materialResource = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(Material));
		//頂点バッファビューを作成
		resource.vertexBufferView.BufferLocation = resource.vertexResource->GetGPUVirtualAddress();
		resource.vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * resource.vertexNum);
		resource.vertexBufferView.StrideInBytes = sizeof(VertexData);
		//リソースにデータをマッピング
		resource.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&resource.vertexData));
		resource.materialResource->Map(0, nullptr, reinterpret_cast<void**>(&resource.materialData));
		//データに書き込み
		for (uint32_t index = 0; index < kRingDivide; index++) {
			float sin = std::sin(index * radianPerDivide);
			float cos = std::cos(index * radianPerDivide);
			float sinNext = std::sin((index + 1) * radianPerDivide);
			float cosNext = std::cos((index + 1) * radianPerDivide);
			float u = float(index) / float(kRingDivide);
			float uNext = float(index + 1) / float(kRingDivide);
			//position(時計回り)とuvの書き込み
			resource.vertexData[index * 6 + 0].position = { kOuterRadius * (-sin), kOuterRadius * cos, 0.0f, 1.0f };
			resource.vertexData[index * 6 + 0].texcoord = { u, 0.0f };

			resource.vertexData[index * 6 + 1].position = { kInnerRadius * (-sin), kInnerRadius * cos, 0.0f, 1.0f };
			resource.vertexData[index * 6 + 1].texcoord = { u, 1.0f };

			resource.vertexData[index * 6 + 2].position = { kOuterRadius * (-sinNext), kOuterRadius * cosNext, 0.0f, 1.0f };
			resource.vertexData[index * 6 + 2].texcoord = { uNext, 0.0f };

			resource.vertexData[index * 6 + 3] = resource.vertexData[index * 6 + 2];

			resource.vertexData[index * 6 + 4] = resource.vertexData[index * 6 + 1];

			resource.vertexData[index * 6 + 5].position = { kInnerRadius * (-sinNext), kInnerRadius * cosNext, 0.0f, 1.0f };
			resource.vertexData[index * 6 + 5].texcoord = { uNext, 1.0f };

			//uvフリップ
			for (uint32_t i = 0; i < 6; i++) {
				resource.vertexData[index * 6 + i].texcoord.y = 1.0f - resource.vertexData[index * 6 + i].texcoord.y;
			}

			//法線ベクトルの書き込み
			for (int j = 0; j < 6; j++) {
				resource.vertexData[index * 6 + j].normal = { 0.0f, 0.0f, 1.0f };
			}
		}
		//マテリアルデータ
		resource.materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		resource.materialData->isTexture = false;
		resource.materialData->shininess = 20.0f;
		resource.materialData->uvTransform = MyMath::MakeIdentity4x4();

		return resource;
	}

	Shape::ShapeResource Shape::MakeTubeResource() {
		ShapeResource resource;
		//ローカル変数
		const uint32_t kCylinderDivide = 32;
		const float kTopRadius = 1.0f;
		const float kBottomRadius = 1.0f;
		const float kHeight = 3.0f;
		const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / float(kCylinderDivide);
		//頂点数を保持
		resource.vertexNum = kCylinderDivide * 6;
		//リソースを作る
		resource.vertexResource = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(VertexData) * resource.vertexNum);
		resource.materialResource = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(Material));
		//頂点バッファビューを作成
		resource.vertexBufferView.BufferLocation = resource.vertexResource->GetGPUVirtualAddress();
		resource.vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * resource.vertexNum);
		resource.vertexBufferView.StrideInBytes = sizeof(VertexData);
		//リソースにデータをマッピング
		resource.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&resource.vertexData));
		resource.materialResource->Map(0, nullptr, reinterpret_cast<void**>(&resource.materialData));
		//データに書き込み
		for (uint32_t index = 0; index < kCylinderDivide; index++) {
			float sin = std::sin(index * radianPerDivide);
			float cos = std::cos(index * radianPerDivide);
			float sinNext = std::sin((index + 1) * radianPerDivide);
			float cosNext = std::cos((index + 1) * radianPerDivide);
			float u = float(index) / float(kCylinderDivide);
			float uNext = float(index + 1) / float(kCylinderDivide);

			//position,texcoord,normalの書き込み
			resource.vertexData[index * 6 + 0].position = { kTopRadius * (-sin), kHeight,kTopRadius * cos, 1.0f };
			resource.vertexData[index * 6 + 0].texcoord = { u, 0.0f };
			resource.vertexData[index * 6 + 0].normal = { -sin, 0.0f, cos };

			resource.vertexData[index * 6 + 1].position = { kTopRadius * (-sinNext), kHeight,kTopRadius * cosNext, 1.0f };
			resource.vertexData[index * 6 + 1].texcoord = { uNext, 0.0f };
			resource.vertexData[index * 6 + 1].normal = { -sinNext, 0.0f, cosNext };

			resource.vertexData[index * 6 + 2].position = { kBottomRadius * (-sin), 0.0f,kBottomRadius * cos, 1.0f };
			resource.vertexData[index * 6 + 2].texcoord = { u, 1.0f };
			resource.vertexData[index * 6 + 2].normal = { -sin, 0.0f, cos };

			resource.vertexData[index * 6 + 3] = resource.vertexData[index * 6 + 2];

			resource.vertexData[index * 6 + 4] = resource.vertexData[index * 6 + 1];

			resource.vertexData[index * 6 + 5].position = { kBottomRadius * (-sinNext), 0.0f,kBottomRadius * cosNext, 1.0f };
			resource.vertexData[index * 6 + 5].texcoord = { uNext, 1.0f };
			resource.vertexData[index * 6 + 5].normal = { -sinNext, 0.0f, cosNext };

			//uvをフリップ
			for (int j = 0; j < 6; j++) {
				resource.vertexData[index * 6 + j].texcoord.y = 1.0f - resource.vertexData[index * 6 + j].texcoord.y;
			}
		}
		//マテリアルデータ
		resource.materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		resource.materialData->isTexture = false;
		resource.materialData->shininess = 20.0f;
		resource.materialData->uvTransform = MyMath::MakeIdentity4x4();

		return resource;
	}

}