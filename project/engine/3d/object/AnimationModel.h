#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <optional>
#include <map>
#include <span>
#include <array>
#include <string>
#include "WorldTransform.h"
#include "BaseCamera.h"
#include "MyMath.h"
#include "ModelFormat.h"
//assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Norm {

	/// <summary>
	/// アニメーションモデルの処理を行うクラス
	/// </summary>
	class AnimationModel {
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
		template <typename tValue>
		/// <summary>
		/// キーフレーム
		/// </summary>
		struct Keyframe {
			float time;
			tValue value;
		};
		using keyframeVector3 = Keyframe<Vector3>;
		using keyframeQuaternion = Keyframe<Quaternion>;
		//ノードアニメーション
		template <typename tValue>
		/// <summary>
		/// アニメーションカーブ
		/// </summary>
		struct AnimationCurve {
			std::vector<Keyframe<tValue>> keyframes;
		};
		/// <summary>
		/// ノードアニメーション
		/// </summary>
		struct NodeAnimation {
			AnimationCurve<Vector3> translate;
			AnimationCurve<Quaternion> rotate;
			AnimationCurve<Vector3> scale;
		};
		/// <summary>
		/// アニメーション
		/// </summary>
		struct Animation {
			float duration;//アニメーション全体の尺(秒)
			//NodeAnimationの集合、Node名で開けるようにしておく
			std::map<std::string, NodeAnimation> nodeAnimations;
		};
		/// <summary>
		/// ジョイント(骨)
		/// </summary>
		struct Joint {
			TransformQuaternion transform;
			Matrix4x4 localMatrix;
			Matrix4x4 skeletonSpaceMatrix;
			std::string name;
			std::vector<int32_t> children;
			int32_t index;
			std::optional<int32_t> parent;
		};
		/// <summary>
		/// スケルトン
		/// </summary>
		struct Skeleton {
			int32_t root;
			std::map<std::string, int32_t> jointMap;
			std::vector<Joint> joints;
		};
		/// <summary>
		/// 頂点ウェイト
		/// </summary>
		struct VertexWeightData {
			float weight;
			uint32_t vertexIndex;
		};
		/// <summary>
		/// ジョイントウェイトデータ
		/// </summary>
		struct JointWeightData {
			Matrix4x4 inverseBindPoseMatrix;
			std::vector<VertexWeightData> vertexWeights;
		};
		/// <summary>
		/// 頂点インフルエンスの最大数
		/// </summary>
		static const uint32_t kNumMaxInfluence = 4;
		struct VertexInfluence {
			std::array<float, kNumMaxInfluence> weights;
			std::array<int32_t, kNumMaxInfluence> jointIndices;
		};
		/// <summary>
		/// GPU用ウェル行列
		/// </summary>
		struct WellForGPU {
			Matrix4x4 skeletonSpaceMatrix;			//位置用
			Matrix4x4 skeletonSpaceInverseMatrix;	//法線用
		};
		/// <summary>
		/// GPU用スキニング情報
		/// </summary>
		struct SkinningInformationForGPU {
			uint32_t numVertices;
		};

		/// <summary>
		/// スキンクラスター
		/// </summary>
		struct SkinCluster {
			std::vector<Matrix4x4> inverseBindPoseMatrices;
			//MatrixPalette
			Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;
			std::span<WellForGPU> mappedPalette;
			uint32_t paletteSrvIndex;
			//入力頂点
			Microsoft::WRL::ComPtr<ID3D12Resource> inputVertexResource;
			std::span<VertexData> mappedInputVertex;
			uint32_t inputVertexSrvIndex;
			//インフルエンス
			Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
			D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
			std::span<VertexInfluence> mappedInfluence;
			uint32_t influenceSrvIndex;
			//出力頂点
			Microsoft::WRL::ComPtr<ID3D12Resource> outputVertexResource;
			uint32_t outputVertexUavIndex;
			//スキニング情報
			Microsoft::WRL::ComPtr<ID3D12Resource> skinningInfoResource;
			std::span<SkinningInformationForGPU> mappedSkinningInfo;
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
		/// ノード構造体
		/// </summary>
		struct Node {
			TransformQuaternion transform;
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
			std::map<std::string, JointWeightData> skinClusterData;
			std::vector<VertexData> vertices;
			std::vector<uint32_t> indices;
			MaterialData material;
			Node rootNode;
		};
		/// <summary>
		/// モデルリソース
		/// </summary>
		struct ModelResource {
			std::vector<ModelData> modelData;
			std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> indexResource;
			std::vector<D3D12_INDEX_BUFFER_VIEW> indexBufferView;
			std::vector<uint32_t*> indexData;
			std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> materialResource;
			std::vector<Material*> materialData;
			std::vector<TransformEuler> uvTransform;
			std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> textureResource;
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
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// モデル描画
		/// </summary>
		/// <param name="materialRootParameterIndex">マテリアル設定用ルートパラメータの番号</param>
		/// <param name="textureRootParameterIndex">テクスチャ設定用ルートパラメータの番号</param>
		/// <param name="instancingNum">インスタンス数</param>
		void Draw(uint32_t materialRootParameterIndex, uint32_t textureRootParameterIndex, uint32_t instancingNum = 1, int32_t textureHandle = EOF);
		/// <summary>
		/// 描画前処理
		/// </summary>
		void SettingCSPreDraw();
		/// <summary>
		/// 描画後処理
		/// </summary>
		void SettingCSPostDraw();

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// モデルリソースの取得
		/// </summary>
		/// <returns>モデルリソース</returns>
		const ModelResource& GetModelResource() { return modelResource_; }

		/// ============================== ///
		///		setter
		/// ============================== ///

		/// <summary>
		/// 色のセット
		/// </summary>
		/// <param name="color">色</param>
		void SetColor(Vector4& color) { color_ = &color; }

		/// <summary>
		/// 新しいアニメーションのセット
		/// </summary>
		/// <param name="_name">名前</param>
		/// <param name="_fileName">ファイル名</param>
		void SetNewAnimation(const std::string& _name, const std::string& _fileName);
		/// <summary>
		/// 現在のアニメーションのセット
		/// </summary>
		/// <param name="_name">名前</param>
		void SetCurrentAnimation(const std::string& _name);

	private:
		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// モデルファイルを読み込む
		/// </summary>
		/// <returns>モデルデータの配列</returns>
		std::vector<ModelData> LoadModelFile();
		/// <summary>
		/// マテリアル解析（LoadModelFileで使用）
		/// </summary>
		/// <param name="scene"></param>
		/// <param name="mesh"></param>
		/// <param name="model"></param>
		void LoadMaterial(const aiScene* scene, const aiMesh* mesh, ModelData& model);
		/// <summary>
		/// 頂点解析（LoadModelFileで使用）
		/// </summary>
		/// <param name="mesh"></param>
		/// <param name="model"></param>
		void LoadVertices(const aiMesh* mesh, ModelData& model);
		/// <summary>
		/// インデックス解析（LoadModelFileで使用）
		/// </summary>
		/// <param name="mesh"></param>
		/// <param name="model"></param>
		void LoadIndices(const aiMesh* mesh, ModelData& model);
		/// <summary>
		/// スキニング情報解析（LoadModelFileで使用）
		/// </summary>
		/// <param name="mesh"></param>
		/// <param name="model"></param>
		void LoadSkinningData(const aiMesh* mesh, ModelData& model);
		/// <summary>
		/// スケルトン解析（LoadModelFileで使用）
		/// </summary>
		/// <param name="scene"></param>
		/// <param name="model"></param>
		void LoadSkeletonIfNeeded(const aiScene* scene, ModelData& model);

		/// <summary>
		/// アニメーションファイルを読み込む
		/// </summary>
		/// <param name="fileName">アニメーションファイル名</param>
		/// <returns>アニメーションデータ</returns>
		Animation LoadAnimationFile(const std::string& fileName);
		/// <summary>
		/// Assimpのノードを構造体ノードに変換する
		/// </summary>
		/// <param name="node">Assimpノード</param>
		/// <returns>変換後のノード</returns>
		Node ReadNode(aiNode* node);
		/// <summary>
		/// モデルリソースを作成する
		/// </summary>
		/// <returns>作成したモデルリソース</returns>
		ModelResource MakeModelResource();
		/// <summary>
		/// スキンクラスターを生成する
		/// </summary>
		/// <returns>スキンクラスター</returns>
		SkinCluster CreateSkinCluster();
		/// <summary>
		/// テクスチャを読み込む
		/// </summary>
		void SettingTexture();
		/// <summary>
		/// 任意の時刻におけるベクトル値を計算する
		/// </summary>
		/// <param name="keyframes">ベクトルキーの配列</param>
		/// <param name="time">再生時間</param>
		/// <returns>補間後のベクトル値</returns>
		Vector3 CalculateValue(const std::vector<Keyframe<Vector3>>& keyframes, float time);
		/// <summary>
		/// 任意の時刻におけるクォータニオン値を計算する
		/// </summary>
		/// <param name="keyframes">クォータニオンキーの配列</param>
		/// <param name="time">再生時間</param>
		/// <returns>補間後のクォータニオン値</returns>
		Quaternion CalculateValue(const std::vector<Keyframe<Quaternion>>& keyframes, float time);
		/// <summary>
		/// NodeからJointを作成する
		/// </summary>
		/// <param name="node">ノード</param>
		/// <param name="parent">親ジョイントのインデックス</param>
		/// <param name="joints">ジョイント配列</param>
		/// <returns>作成したジョイントのインデックス</returns>
		int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
		/// <summary>
		/// NodeからSkeletonを作成する
		/// </summary>
		/// <param name="rootNode">ルートノード</param>
		/// <returns>スケルトン</returns>
		Skeleton CreateSkeleton(const Node& rootNode);
		/// <summary>
		/// ジョイントを更新する
		/// </summary>
		/// <param name="skeleton">スケルトン</param>
		void UpdateJoints(Skeleton& skeleton);
		/// <summary>
		/// アニメーションをスケルトンに適用する
		/// </summary>
		/// <param name="skeleton">スケルトン</param>
		/// <param name="animation">アニメーション</param>
		/// <param name="animationTime">再生時間</param>
		void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);
		/// <summary>
		/// スキンクラスターを更新する
		/// </summary>
		/// <param name="skinCluster">スキンクラスター</param>
		/// <param name="skeleton">スケルトン</param>
		void UpdateSkinCluster(SkinCluster& skinCluster, const Skeleton& skeleton);

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//モデル用リソース
		ModelResource modelResource_;
		//モデル数
		size_t modelNum_;
		//色
		Vector4* color_ = nullptr;

		//ディレクトリパス
		std::string directoryPath_;
		//モデルデータファイル名
		std::string fileName_;
		//形式名
		ModelFormat mf_;
		std::string format_;

		//アニメーション用変数
		std::map<std::string, Animation> animations_;
		std::string currentAnimation_;
		float animationTime_ = 0.0f;

		//スケルトン
		Skeleton skeleton_;
		bool isSkeleton_ = false;

		//スキンクラスター
		SkinCluster skinCluster_;

	};

}