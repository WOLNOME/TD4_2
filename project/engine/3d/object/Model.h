#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <string>
#include "MyMath.h"
#include "ModelFormat.h"
//assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Norm {

	/// <summary>
	/// 外部リソース(3Dモデルデータ)単体の情報をまとめたクラス
	/// </summary>
	class Model {
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
		/// ノード
		/// </summary>
		struct Node {
			Matrix4x4 localMatrix;
			std::string name;
			std::vector<Node> children;
		};
		/// <summary>
		/// マテリアルデータ
		/// </summary>
		struct MaterialData {
			std::string materialName;
			std::string textureFilePath;
			Vector4 colorData;
			uint32_t textureHandle;
		};
		/// <summary>
		/// モデルデータ
		/// </summary>
		struct ModelData {
			std::vector<VertexData> vertices;
			std::vector<uint32_t> indices;
			MaterialData material;
			Node rootNode;
		};
		/// <summary>
		/// モデルリソース作成用データ型
		/// </summary>
		struct ModelResource {
			std::vector<ModelData> modelData;
			std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> vertexResource;
			std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferView;
			std::vector<VertexData*> vertexData;
			std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> indexResource;
			std::vector<D3D12_INDEX_BUFFER_VIEW> indexBufferView;
			std::vector<uint32_t*> indexData;
			std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> materialResource;
			std::vector<Material*> materialData;
			std::vector<TransformEuler> uvTransform;
			std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> textureResorce;
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> textureSrvHandleCPU;
			std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> textureSrvHandleGPU;
		};

	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="filename">ファイル名</param>
		/// <param name="format">フォーマット指定子</param>
		/// <param name="directorypath">ディレクトリパス</param>
		void Initialize(const std::string& filename, ModelFormat format = OBJ, std::string directorypath = "Resources/models/");
		/// <summary>
		/// 描画
		/// </summary>
		/// <param name="materialRootParameterIndex">マテリアルのルートパラメータ番号</param>
		/// <param name="textureRootParameterIndex"テクスチャのルートパラメータ番号></param>
		/// <param name="instancingNum">インスタンシング描画数</param>
		/// <param name="textureHandle">テクスチャハンドル</param>
		void Draw(Vector4 color, uint32_t materialRootParameterIndex, uint32_t textureRootParameterIndex, uint32_t instancingNum = 1, int32_t textureHandle = EOF);

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// モデルリソースの取得
		/// </summary>
		/// <returns>モデルリソース</returns>
		const ModelResource& GetModelResource() { return modelResource_; }

	private:
		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// モデルデータファイル読み込み
		/// </summary>
		/// <returns>モデルデータファイル</returns>
		std::vector<ModelData> LoadModelFile();
		/// <summary>
		/// ノード読み取り
		/// </summary>
		/// <param name="node">aiノード</param>
		/// <returns>ノード</returns>
		Node ReadNode(aiNode* node);
		/// <summary>
		/// モデルリソース作成
		/// </summary>
		/// <returns>モデルリソース</returns>
		ModelResource MakeModelResource();
		/// <summary>
		/// テクスチャ設定
		/// </summary>
		void SettingTexture();

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//モデル用リソース
		ModelResource modelResource_;
		//モデル数
		size_t modelNum_;

		//ディレクトリパス
		std::string directoryPath_;
		//モデルデータファイル名
		std::string fileName_;
		//形式名
		ModelFormat mf_;
		std::string format_;
	};

}