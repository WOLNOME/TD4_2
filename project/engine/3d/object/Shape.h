#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "MyMath.h"

namespace Norm {

	/// <summary>
	/// 単純な形状モデルを扱うクラス
	/// </summary>
	class Shape {
	public:
		/// ============================== ///
		///		列挙体
		/// ============================== ///

		/// <summary>
		/// 形状の種類
		/// </summary>
		enum ShapeKind {
			kSphere,		//球体
			kCube,			//立方体
			kSkyBox,		//スカイボックス
			kPlane,			//平面
			kRing,			//リング
			kTube,			//筒

			kMaxShapeKindNum//最大形状数
		};//※ここに新しい形状を追加する場合、パーティクルクリエイターシーンの形状選択部分も変更すること

	private:
		/// ============================== ///
		///		構造体
		/// ============================== ///

		/// <summary>
		/// 頂点データ
		/// </summary>
		struct VertexData {
			Vector4 position;
			Vector2 texcoord;
			Vector3 normal;
		};
		/// <summary>
		/// マテリアル
		/// </summary>
		struct Material {
			Vector4 color;
			Matrix4x4 uvTransform;
			float isTexture;
			float shininess;
		};
		/// <summary>
		/// 形状リソース作成用データ型
		/// </summary>
		struct ShapeResource {
			Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
			D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
			VertexData* vertexData;
			uint32_t vertexNum;
			Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
			Material* materialData;
		};

	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="shapeKind">形状の種類</param>
		void Initialize(ShapeKind shapeKind);
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 描画
		/// </summary>
		/// <param name="materialRootParameterIndex">マテリアルのルートパラメータ番号</param>
		/// <param name="textureRootParameterIndex">テクスチャのルートパラメータ番号</param>
		/// <param name="instancingNum">インスタンシング描画の数</param>
		/// <param name="textureHandle">テクスチャハンドル</param>
		void Draw(uint32_t materialRootParameterIndex, uint32_t textureRootParameterIndex, uint32_t instancingNum = 1, int32_t textureHandle = EOF);

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// 形状の種類を取得
		/// </summary>
		/// <returns>形状の種類</returns>
		ShapeKind GetShapeKind() const { return shapeKind_; }

		/// ============================== ///
		///		setter
		/// ============================== ///

		/// <summary>
		/// 色のセット
		/// </summary>
		/// <param name="_color">色</param>
		void SetColor(Vector4& _color) { color_ = &_color; }

	private:
		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// 形状リソース作成関数
		/// </summary>
		/// <returns>形状リソース</returns>
		ShapeResource MakeShapeResource();

		/// <summary>
		/// 球体リソースの作成関数
		/// </summary>
		/// <returns>球体リソース</returns>
		ShapeResource MakeSphereResource();
		/// <summary>
		/// 立方体リソースの作成関数
		/// </summary>
		/// <returns>立方体リソース</returns>
		ShapeResource MakeCubeResource();
		/// <summary>
		/// スカイボックスリソースの作成関数
		/// </summary>
		/// <returns>スカイボックスリソース</returns>
		ShapeResource MakeSkyBoxResource();
		/// <summary>
		/// 平面リソースの作成関数
		/// </summary>
		/// <returns>平面リソース</returns>
		ShapeResource MakePlaneResource();
		/// <summary>
		/// リングリソースの作成関数
		/// </summary>
		/// <returns>リングリソース</returns>
		ShapeResource MakeRingResource();
		/// <summary>
		/// 筒リソースの作成関数
		/// </summary>
		/// <returns>筒リソース</returns>
		ShapeResource MakeTubeResource();

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//形状の種類
		ShapeKind shapeKind_;
		//形状用リソース
		ShapeResource shapeResource_;
		//球体の分割数
		const uint32_t kSubdivision = 15;

		//色
		Vector4* color_ = nullptr;

	};

}