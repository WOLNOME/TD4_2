#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <string>
#include <list>
#include <memory>
#include <span>
#include "json.hpp"
#include "Shape.h"
#include "MyMath.h"
#include "ParticleTypes.h"

using json = nlohmann::json;

namespace Norm {

	/// <summary>
	/// パーティクル単体の処理全般を管理するクラス
	/// </summary>
	class Particle {
		//パーティクルマネージャーに公開
		friend class ParticleManager;
		//パーティクルクリエイターシーンに公開
		friend class ParticleEditorScene;
		//複合パーティクルに公開
		friend class CombinedParticle;

	private:
		/// ============================== ///
		///		列挙体(private)
		/// ============================== ///

		struct EachResourceForCS {
			//エミッターID
			Microsoft::WRL::ComPtr<ID3D12Resource> emitterIDResource;
			std::span<TargetEmitterIDForVS> mappedEmitterID;
		};

	public:
		/// ============================== ///
		///		列挙体(public)
		/// ============================== ///

		/// <summary>
		/// 生成方法
		/// </summary>
		enum class GenerateMethod {
			Random,		//ランダム
			Clump,		//塊	

			kMaxGenerateMethodNum,
		};
		/// <summary>
		/// エフェクトスタイル
		/// </summary>
		enum class EffectStyle {
			Loop,		//ループ(永続)
			OneShot,	//一度きり

			kMaxEffectStyleNum,
		};

		/// ============================== ///
		///		構造体(public)
		/// ============================== ///

		/// <summary>
		/// エミッター情報
		/// </summary>
		struct Emitter {
			TransformEuler worldTransform;	//エミッターのトランスフォーム
			bool isAffectedField = false;	//フィールドに影響を受けるか
			bool isPlay = false;			//パーティクルを生成するか
			bool isDraw = false;			//描画するか
		};

	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Particle();
		/// <summary>
		/// デストラクタ
		/// </summary>
		~Particle();
		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="name">インスタンスの名前</param>
		/// <param name="fileName">使用するパーティクルの名前(.jsonは省略)</param>
		void Initialize(const std::string& name, const std::string& fileName);

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		///　パラメーターの取得
		/// </summary>
		/// <returns>パラメーター</returns>
		const json& GetParam() { return param_; }

		/// ============================== ///
		///		メンバ変数(public)
		/// ============================== ///

		Emitter emitter_;

	private:
		/// ============================== ///
		///		メンバ関数(private)
		/// ============================== ///

		/// <summary>
		/// 形状の変更
		/// </summary>
		void ShapeChange();
		/// <summary>
		/// テクスチャの変更
		/// </summary>
		void TextureChange();

		/// <summary>
		/// エミッター情報反映(CSに反映)
		/// </summary>
		void TraceEmitterForCS();
		/// <summary>
		/// JSON情報反映(CSに反映)
		/// </summary>
		void TraceJsonInfoForCS();

		/// <summary>
		/// 個別のCS用リソースの作成
		/// </summary>
		/// <returns>個別のCS用リソース</returns>
		EachResourceForCS CreateEachResourceForCS();

	private:
		/// ============================== ///
		///		メンバ変数(private)
		/// ============================== ///

		//形状(見た目)
		std::unique_ptr<Shape> shape_;

		//名前
		std::string name_;
		//シーンタグ
		std::string sceneTag_;

		//エミッターID（識別番号）
		uint32_t emitterID_ = 0u;
		//各粒のパラメーター
		json param_;
		//テクスチャハンドル
		uint32_t textureHandle_ = 0u;
		//マネージャー送信用のリソース
		EmitterForCS emitterForCS_;
		JsonInfoForCS jsonInfoForCS_;
		//個別のCS用リソース
		EachResourceForCS eachResourceForCS_;

	};

}