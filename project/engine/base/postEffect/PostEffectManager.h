#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <array>
#include <vector>
#include <memory>
#include "Vector3.h"
#include "Vector4.h"

namespace Norm {

	/// <summary>
	/// ポストエフェクトの種類
	/// </summary>
	enum class PostEffectKind {
		None,					// 何もしない
		Grayscale,				// グレースケール
		Vignette,				// ヴィネット	
		BoxFilter,				// ボックスフィルター
		GaussianFilter,			// ガウシアンフィルター
		LuminanceBaseOutline,	// 輝度ベースのアウトライン
		RadialBlur,				// ラジアルブラー
		Dissolve,				// ディゾルブ
		Random,					// ランダム
		HSVFilter,				// HSVフィルター

		kMaxNumPostEffectKind,	// ポストエフェクトの最大数
	};
	/*もしポストエフェクトを追加した場合
	ルートシグネチャを追加したい場合は別途設定必須
	グラフィックスパイプラインではPSを増やす
	ImGuiにも追加しておく
	描画の個別設定も別途必要*/

	/// <summary>
	/// 全てのポストエフェクトを管理するクラス
	/// シングルトンパターンで実装
	/// </summary>
	class PostEffectManager {
	private:
		/// ============================== ///
		///		構造体
		/// ============================== ///

		/// <summary>
		/// ディゾルブ用データ
		/// </summary>
		struct DissolveData {
			float threshold;	//閾値

			//追加予定項目
			//全体の色、エッジの色、エッジの大きさ
		};
		/// <summary>
		/// ディゾルブ用リソース
		/// </summary>
		struct DissolveResource {
			Microsoft::WRL::ComPtr<ID3D12Resource> resource;
			DissolveData* data;
			uint32_t textureHandle;		//ディゾルブに使用するテクスチャ
		};
		/// <summary>
		/// ランダム用データ
		/// </summary>
		struct RandomData {
			float seed;	//シード値
		};
		/// <summary>
		/// ランダム用リソース
		/// </summary>
		struct RandomResource {
			Microsoft::WRL::ComPtr<ID3D12Resource> resource;
			RandomData* data;
		};
		/// <summary>
		/// HSVフィルター用データ
		/// </summary>
		struct HSVFilterData {
			Vector3 hsvColor;	//HSVの色
		};
		/// <summary>
		/// HSVフィルター用リソース
		/// </summary>
		struct HSVFilterResource {
			Microsoft::WRL::ComPtr<ID3D12Resource> resource;
			HSVFilterData* data;
		};

		/// <summary>
		/// 全ポストエフェクトのリソース管理用構造体
		/// </summary>
		struct PostEffectResource {
			DissolveResource dissolveResource;
			RandomResource randomResource;
			HSVFilterResource hsvResource;
		};

	private://コンストラクタ等の隠蔽
		static std::unique_ptr<PostEffectManager> instance_;

		PostEffectManager() = default;//コンストラクタ隠蔽
		~PostEffectManager() = default;//デストラクタ隠蔽
		PostEffectManager(PostEffectManager&) = delete;//コピーコンストラクタ封印
		PostEffectManager& operator=(PostEffectManager&) = delete;//コピー代入演算子封印

		friend struct std::default_delete<PostEffectManager>;
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// シングルトンインスタンスの取得
		/// </summary>
		/// <returns>シングルトンインスタンス</returns>
		static PostEffectManager* GetInstance();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();

		/// <summary>
		/// オブジェクト描画前処理
		/// </summary>
		void PreObjectDraw();

		/// <summary>
		/// シーンのコピー
		/// </summary>
		void CopySceneToRenderTexture();

		/// <summary>
		/// デバッグ用ImGui
		/// </summary>
		void DebugWithImGui();

		/// <summary>
		/// オフスクの初期化
		/// </summary>
		void InitOffScreenRenderingOption();
		/// <summary>
		/// オフスクのグラフィックスパイプラインの生成
		/// </summary>
		void GenerateRenderTextureGraphicsPipeline();
		/// <summary>
		/// 固有リソースの初期化
		/// </summary>
		void InitUniqueResources();

		/// ============================== ///
		///		setter
		/// ============================== ///

		/// <summary>
		/// ポストエフェクトの種類の設定
		/// </summary>
		/// <param name="_kind">ポストエフェクトの種類</param>
		void SetPostEffect(const PostEffectKind& _kind) { currentPostEffectKind = _kind; }

	private:
		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//レンダーテクスチャのリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> renderTextureResource = nullptr;
		//レンダーテクスチャのSRVインデックス
		uint32_t srvIndex = 0;
		//ルートシグネチャ
		std::array<Microsoft::WRL::ComPtr<ID3D12RootSignature>, (int)PostEffectKind::kMaxNumPostEffectKind> rootSignature;
		//グラフィックスパイプライン
		std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, (int)PostEffectKind::kMaxNumPostEffectKind> graphicsPipelineState;
		//RTVのディスクリプタハンドル
		uint32_t rtvIndex = 0;
		//レンダーテクスチャのクリアカラー
		const Vector4 kRenderTragetClearValue = Vector4(0, 0, 1, 1);

		//現在適用しているポストエフェクトの種類
		PostEffectKind currentPostEffectKind = PostEffectKind::None;

		//ポストエフェクトのリソース
		PostEffectResource postEffectResource;
	};

}