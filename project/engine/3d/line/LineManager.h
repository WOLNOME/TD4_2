#pragma once
#include "MyMath.h"
#include <wrl.h>
#include <d3d12.h>
#include <list>
#include <memory>

namespace Norm {

	class BaseCamera;

	/// <summary>
	/// 全てのライン描画を管理するクラス
	/// </summary>
	class LineManager {
	private:
		/// ============================== ///
		///		構造体
		/// ============================== ///

		/// <summary>
		/// 頂点データ構造体
		/// </summary>
		struct VertexForGPU {
			Vector4 position;
			float vertexIndex;
		};
		/// <summary>
		/// ラインデータ構造体(インスタンスごと)
		/// </summary>
		struct LineForGPU {
			Vector4 start;
			Vector4 end;
			Vector4 color;
		};
		/// <summary>
		/// ライン用リソース構造体
		/// </summary>
		struct LineResource {
			Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
			D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
			VertexForGPU* vertexData;
			Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;
			LineForGPU* instancingData;
			uint32_t srvIndex;
		};
		/// <summary>
		/// ライン構造体
		/// </summary>
		struct Line {
			Vector4 start;
			Vector4 end;
			Vector4 color;
		};
	private://コンストラクタ等の隠蔽
		static std::unique_ptr<LineManager> instance_;

		LineManager() = default;//コンストラクタ隠蔽
		~LineManager() = default;//デストラクタ隠蔽
		LineManager(LineManager&) = delete;//コピーコンストラクタ封印
		LineManager& operator=(LineManager&) = delete;//コピー代入演算子封印

		friend struct std::default_delete<LineManager>;
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// インスタンス取得関数
		/// </summary>
		/// <returns>インスタンス</returns>
		static LineManager* GetInstance();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 描画
		/// </summary>
		void Draw();
		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();

		/// <summary>
		/// ライン作成関数
		/// </summary>
		/// <param name="start">始点</param>
		/// <param name="end">終点</param>
		/// <param name="color">色</param>
		void CreateLine(Vector3 start, Vector3 end, Vector4 color);

		/// ============================== ///
		///		setter
		/// ============================== ///

		/// <summary>
		/// カメラセット関数
		/// </summary>
		/// <param name="_camera">カメラ</param>
		void SetCamera(BaseCamera* _camera) { camera_ = _camera; }

	private:
		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// グラフィックスパイプライン作成関数
		/// </summary>
		void GenerateGraphicsPipeline();
		/// <summary>
		/// ライン用リソース作成関数
		/// </summary>
		/// <returns></returns>
		LineResource MakeLineResource();
		/// <summary>
		/// SRV設定関数
		/// </summary>
		void SettingSRV();

		/// ============================== ///
		///		インスタンス
		/// ============================== ///

		BaseCamera* camera_ = nullptr;

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//ルートシグネチャ
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
		//グラフィックスパイプライン
		Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;

		//線のコンテナ
		std::list<Line> lines_;
		//ライン用リソース
		LineResource lineResource_;

	};

}