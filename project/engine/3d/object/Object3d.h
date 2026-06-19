#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <memory>
#include "MyMath.h"
#include "Model.h"
#include "AnimationModel.h"
#include "Shape.h"
#include "ModelFormat.h"
#include "WorldTransform.h"

namespace Norm {

	/// ============================== ///
	///		列挙体
	/// ============================== ///

	/// <summary>
	/// オブジェクトの種類
	/// </summary>
	enum class ObjectKind {
		Model,				//通常モデル
		AnimationModel,		//アニメーションモデル
		Shape,				//単純形状

		kMaxNumObjectKind,
	};

	//初期化用のタグ
	struct ModelTag {};
	struct AnimationModelTag {};
	struct ShapeTag {};

	//前方宣言
	class BaseCamera;
	class SceneLight;

	/// <summary>
	/// 3Dオブジェクト単体の処理を行うクラス
	/// </summary>
	class Object3d {
		//オブジェクト3dマネージャーに公開
		friend class Object3dManager;

	public:

		/// ============================== ///
		///		構造体
		/// ============================== ///

		/// <summary>
		/// インスタンシング用データ
		/// </summary>
		struct InstancingForGPU {
			Matrix4x4 matWorld;
			Matrix4x4 matWorldInverseTranspose;
		};
		/// <summary>
		/// ライト用フラグ(GPU用)
		/// </summary>
		struct FlagForGPU {
			uint32_t isActiveLights;
			uint32_t isActiveEnvironment;
		};
		/// <summary>
		/// Object用リソース
		/// </summary>
		struct ObjectResource {
			Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;
			InstancingForGPU* instancingData;
			uint32_t instancingSrvIndex;
			Microsoft::WRL::ComPtr<ID3D12Resource> lightFlagResource;
			FlagForGPU* lightFlagData;
		};

		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Object3d();
		/// <summary>
		/// デストラクタ
		/// </summary>
		~Object3d();

		/// <summary>
		/// 通常モデル初期化
		/// </summary>
		/// <param name="">ModelTag{}と入力</param>
		/// <param name="name">名前</param>
		/// <param name="filePath">ファイルパス</param>
		void Initialize(ModelTag, const std::string& name, const std::string& filePath);
		/// <summary>
		/// アニメーションモデル初期化
		/// </summary>
		/// <param name="">AnimationModeltag{}と入力</param>
		/// <param name="name">名前</param>
		/// <param name="filePath">ファイルパス</param>
		void Initialize(AnimationModelTag, const std::string& name, const std::string& filePath);
		/// <summary>
		/// 形状初期化
		/// </summary>
		/// <param name="">ShapeTag{}と入力</param>
		/// <param name="name">名前</param>
		/// <param name="kind">形状の種類</param>
		void Initialize(ShapeTag, const std::string& name, Shape::ShapeKind kind);

		/// <summary>
		/// デバッグ
		/// </summary>
		/// <param name="_name">名前</param>
		void Debug(const std::wstring& _name);

		/// <summary>
		/// ワールドトランスフォームの登録
		/// </summary>
		/// <param name="_worldTransform">ワールドトランスフォームのポインタ</param>
		/// <returns>ハンドル</returns>
		uint32_t RegistWorldTransform(WorldTransform* _worldTransform);
		/// <summary>
		/// ワールドトランスフォームの削除
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		void DeleteWorldTransform(uint32_t _handle);

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// カラーの取得
		/// </summary>
		/// <returns>カラー</returns>
		const Vector4& GetColor() { return color_; }

		/// ============================== ///
		///		setter
		/// ============================== ///

		/// <summary>
		/// テクスチャのセット
		/// </summary>
		/// <param name="_textureHandle">テクスチャハンドル</param>
		void SetTexture(int32_t _textureHandle) { textureHandle_ = _textureHandle; }
		/// <summary>
		/// 環境光用テクスチャのセット
		/// </summary>
		/// <param name="_textureHandle">テクスチャハンドル</param>
		void SetEnvironmentLightTextureHandle(int32_t _textureHandle) { environmentLightTextureHandle_ = _textureHandle; }
		/// <summary>
		///	表示するか
		/// </summary>
		/// <param name="_isDisplay">表示するか</param>
		void SetIsDisplay(bool _isDisplay) { isDisplay_ = _isDisplay; };
		/// <summary>
		///	ライトの処理をするか
		/// </summary>
		/// <param name="_isLightProcess">ライトの処理をするか</param>
		void SetIsLightProcess(bool _isLightProcess) { isLightProcess_ = _isLightProcess; }
		/// <summary>
		/// カラーのセット
		/// </summary>
		/// <param name="_color">カラー</param>
		void SetColor(const Vector4& _color);

		/// <summary>
		/// 新しいアニメーションを追加
		/// </summary>
		/// <param name="_name">名前</param>
		/// <param name="_filePath">ファイルパス</param>
		void SetNewAnimation(const std::string& _name, const std::string& _filePath);
		/// <summary>
		/// 現在のアニメーションを設定
		/// </summary>
		/// <param name="_name">名前</param>
		void SetCurrentAnimation(const std::string& _name);

	private:
		/// ============================== ///
		///		マネージャーへの委託処理用
		/// ============================== ///

		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 描画
		/// </summary>
		/// <param name="_camera">カメラ</param>
		/// <param name="_sceneLight">シーンライト</param>
		void Draw(BaseCamera* _camera, SceneLight* _sceneLight);

		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// オブジェクト用リソースの作成
		/// </summary>
		/// <returns>ライト用フラグリソース</returns>
		ObjectResource CreateObjectResource();

		/// ============================== ///
		///		描画に利用するメンバ変数
		/// ============================== ///

		//テクスチャ
		int32_t textureHandle_ = EOF;
		//環境光用のテクスチャ
		int32_t environmentLightTextureHandle_ = EOF;

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//名前
		std::string name_;
		//シーンタグ
		std::string sceneTag_;

		//モデル
		Model* model_ = nullptr;
		//アニメーションモデル
		std::unique_ptr<AnimationModel> animationModel_ = nullptr;
		//形状
		std::unique_ptr<Shape> shape_ = nullptr;

		//トランスフォーム
		static const int kMaxInstancingNum = 1024;
		std::unordered_map<uint32_t, WorldTransform*> worldTransforms_;
		uint32_t nextIndex_ = 0u;
		std::vector<uint32_t> freeIndices_;

		//オブジェクトの種類
		ObjectKind objKind_;

		//オブジェクト用リソース
		ObjectResource objectResource_;

		//描画するか
		bool isDisplay_ = true;
		//ライトの処理をするか
		bool isLightProcess_ = true;

		//色
		Vector4 color_ = { 1,1,1,1 };

	};

}