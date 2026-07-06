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
		BloomExtract,			// ブルーム（明部の抽出）
		BloomComposite,			// ブルーム（加算）

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
		/// ブルーム抽出用データ
		/// </summary>
		struct BloomExtractData {
			float threshold;		//閾値
		};
		/// <summary>
		/// ブルーム抽出用リソース
		/// </summary>
		struct BloomExtractResource {
			Microsoft::WRL::ComPtr<ID3D12Resource> resource;
			BloomExtractData* data;
		};
		/// <summary>
		/// ブルーム加算用データ
		/// </summary>
		struct BloomCompositeData {
			float intensity;		//輝度
		};
		/// <summary>
		/// ブルーム加算用リソース
		/// </summary>
		struct BloomCompositeResource {
			Microsoft::WRL::ComPtr<ID3D12Resource> resource;
			BloomCompositeData* data;
		};

		/// <summary>
		/// 全ポストエフェクトのリソース管理用構造体
		/// </summary>
		struct PostEffectResource {
			DissolveResource dissolveResource;
			RandomResource randomResource;
			HSVFilterResource hsvResource;
			BloomExtractResource bloomExtractResource;
			BloomCompositeResource bloomCompositeResource;
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

		/// <summary>
		/// ポストエフェクトの追加
		/// </summary>
		/// <param name="peKind">追加するポストエフェクトの種類</param>
		/// <param name="preKind">追加するPEの前のPE（Noneなら一番前に）</param>
		void AddPostEffectOrder(PostEffectKind peKind,PostEffectKind preKind=PostEffectKind::None);

		/// <summary>
		/// ポストエフェクトの削除
		/// </summary>
		/// <param name="peKind">削除するポストエフェクト</param>
		void DeletePostEffectOrder(PostEffectKind peKind);



	private:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// ポストエフェクトの描画
		/// </summary>
		/// <param name="peKind">ポストエフェクトの種類</param>
		/// <param name="isRTRCopy">レンダーテクスチャリソースをコピーするか</param>
		void PostEffectDraw(const PostEffectKind& peKind, bool isRTRCopy = false);

		/// <summary>
		/// リソースのステートを遷移させる
		/// </summary>
		/// <param name="pResource">対象のリソース</param>
		/// <param name="before">遷移前の状態</param>
		/// <param name="after">遷移後の状態</param>
		void TransitionState(ID3D12Resource* pResource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//レンダーテクスチャのリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> renderTextureResource = nullptr;
		//レンダーテクスチャのコピーリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> cpyRenderTextureResource = nullptr;

		//レンダーテクスチャのSRVインデックス
		uint32_t srvIndex = 0;
		//レンダーテクスチャのコピーのSRVインデックス
		uint32_t cpySrvIndex = 0;
		//RTVのディスクリプタハンドル
		uint32_t rtvIndex = 0;
		//レンダーテクスチャのクリアカラー
		const Vector4 kRenderTragetClearValue = Vector4(0, 0, 1, 1);
		//ルートシグネチャ
		std::array<Microsoft::WRL::ComPtr<ID3D12RootSignature>, (int)PostEffectKind::kMaxNumPostEffectKind> rootSignature;
		//グラフィックスパイプライン
		std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, (int)PostEffectKind::kMaxNumPostEffectKind> graphicsPipelineState;

		//適用するポストエフェクトの順番
		std::vector<PostEffectKind> postEffectOrder;

		//ポストエフェクトのリソース
		PostEffectResource postEffectResource;
	};

}