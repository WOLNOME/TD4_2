#pragma once
#include <d3d12.h>
#include <string>
#include <array>
#include <list>
#include <unordered_map>
#include <wrl.h>
#include <span>
#include <memory>
#include "BaseCamera.h"
#include "MyMath.h"
#include "BlendMode.h"
#include "ParticleTypes.h"

namespace Norm {

	class Particle;

	/// <summary>
	/// 全てのパーティクルを管理するクラス
	/// シングルトンパターンで実装
	/// </summary>
	class ParticleManager {
		//パーティクルクリエイターシーンに公開
		friend class ParticleEditorScene;
	private:
		/// ============================== ///
		///		構造体(private)
		/// ============================== ///

		/// <summary>
		/// 共通のCS用リソース
		/// </summary>
		struct CommonResourceForCS {
			//粒の情報
			Microsoft::WRL::ComPtr<ID3D12Resource> grainsResource;
			uint32_t grainsSrvIndex = 0u;	//VS用
			uint32_t grainsUavIndex = 0u;	//CS用
			//粒のIndex情報
			Microsoft::WRL::ComPtr<ID3D12Resource> grainIndicesResource;
			uint32_t grainIndicesSrvIndex = 0u;	//VS用
			uint32_t grainIndicesUavIndex = 0u;	//CS用
			//エミッターの範囲情報
			Microsoft::WRL::ComPtr<ID3D12Resource> emitterRangeResource;
			uint32_t emitterRangeSrvIndex = 0u;	//VS用
			uint32_t emitterRangeUavIndex = 0u;	//CS用
			//フリーリストのインデックス情報
			Microsoft::WRL::ComPtr<ID3D12Resource> freeListIndexResource;
			uint32_t freeListIndexUavIndex = 0u;		//CS用
			//フリーリストの情報
			Microsoft::WRL::ComPtr<ID3D12Resource> freeListResource;
			uint32_t freeListUavIndex = 0u;		//CS用
			//エミッター情報
			Microsoft::WRL::ComPtr<ID3D12Resource> emitterResource;
			EmitterForCS* mappedEmitter;
			uint32_t emitterSrvIndex = 0u;
			//JSON情報
			Microsoft::WRL::ComPtr<ID3D12Resource> jsonInfoResource;
			JsonInfoForCS* mappedJsonInfo;
			uint32_t jsonInfoSrvIndex = 0u;
			//総合情報
			Microsoft::WRL::ComPtr<ID3D12Resource> generalInfoResource;
			GeneralInfoForCS* mappedGeneralInfo;
		};

	private:
		static std::unique_ptr<ParticleManager> instance_;

		ParticleManager() = default;//コンストラクタ隠蔽
		~ParticleManager() = default;//デストラクタ隠蔽
		ParticleManager(ParticleManager&) = delete;//コピーコンストラクタ封印
		ParticleManager& operator=(ParticleManager&) = delete;//コピー代入演算子封印

		friend struct std::default_delete<ParticleManager>;
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// シングルトンインスタンスの取得
		/// </summary>
		/// <returns>シングルトンインスタンス</returns>
		static ParticleManager* GetInstance();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 描画
		/// </summary>
		void Draw();
		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();

		/// <summary>
		/// パーティクルをコンテナに登録
		/// </summary>
		/// <param name="name">名前</param>
		/// <param name="particle">パーティクルのポインタ</param>
		void Regist(const std::string& name, Particle* particle);
		/// <summary>
		/// 登録されたパーティクル名の変更
		/// </summary>
		/// <param name="preName">変更前の名前</param>
		/// <param name="postName">変更後の名前</param>
		bool Rename(const std::string& preName, const std::string& postName);
		/// <summary>
		/// 登録されたパーティクルを削除
		/// </summary>
		/// <param name="name">名前</param>
		void Delete(const std::string& name);

		/// <summary>
		/// 名前を決める関数
		/// </summary>
		/// <param name="name">名前</param>
		/// <returns>重複しない名前</returns>
		std::string GenerateName(const std::string& name);

		/// ============================== ///
		///		setter
		/// ============================== ///

		/// <summary>
		/// カメラのセット
		/// </summary>
		/// <param name="camera">カメラ</param>
		void SetCamera(BaseCamera* camera) { camera_ = camera; }

	private:
		/// ============================== ///
		///		メンバ関数(private)
		/// ============================== ///

		/// <summary>
		/// グラフィックスパイプライン
		/// </summary>
		void GenerateGraphicsPipeline();
		/// <summary>
		/// コンピュートパイプライン
		/// </summary>
		void GenerateComputePipeline();

		/// <summary>
		/// UAVバッファの初期化
		/// </summary>
		void InitUAVBuffer();

		/// <summary>
		/// 初期化用CPSO(コンピュートパイプラインステートオブジェクト)
		/// </summary>
		void InitCPSOOption();
		/// <summary>
		/// 発行用CPSO(コンピュートパイプラインステートオブジェクト)
		/// </summary>
		void EmitCPSOOption();
		/// <summary>
		/// 更新用CPSO(コンピュートパイプラインステートオブジェクト)
		/// </summary>
		void UpdateCPSOOption();
		/// <summary>
		/// 粒情報解析用CPSO(コンピュートパイプラインステートオブジェクト)
		/// </summary>
		void LocksmithCPSOOption();

		/// <summary>
		/// 共通のCS用リソースの作成
		/// </summary>
		/// <returns>共通のCS用リソース</returns>
		CommonResourceForCS CreateCommonResourceForCS();

		/// <summary>
		/// エミッターIDの割り当て
		/// </summary>
		/// <returns>エミッターID</returns>
		uint32_t AllocateEmitterID();
		/// <summary>
		/// エミッターIDの破棄
		/// </summary>
		/// <param name="id">エミッターID</param>
		void DiscardEmitterID(uint32_t id);

		/// ============================== ///
		///		インスタンス
		/// ============================== ///

		BaseCamera* camera_ = nullptr;

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//Gルートシグネチャ
		Microsoft::WRL::ComPtr<ID3D12RootSignature> gRootSignature = nullptr;
		//グラフィックスパイプライン
		std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, (int)BlendMode::kMaxBlendModeNum> graphicsPipelineState;
		//Cルートシグネチャ(init,emit,update,locksmith分あるので4つ)
		std::array<Microsoft::WRL::ComPtr<ID3D12RootSignature>, 4> cRootSignature;
		//コンピュートパイプライン
		std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, 4> computePipelineState;

		//パーティクルのコンテナ
		std::unordered_map<std::string, Particle*> particles;

		//粒の最大数 (必要に応じて増やす)
		static const int kMaxNumGrains = 2000000;
		//エミッターの最大数 (必要に応じて増やす)
		static const int kMaxNumEmitters = 1500;

		//共通のCS用リソース
		CommonResourceForCS commonResourceForCS_;

		//エミッターID用カウンタ
		uint32_t emitterIDCounter_ = 0u;
		std::list<uint32_t> freeEmitterIDList_;

	};

}