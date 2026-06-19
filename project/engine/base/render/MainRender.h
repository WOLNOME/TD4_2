#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <cstdint>
#include <array>
#include <memory>

namespace Norm {

	/// <summary>
	/// ほぼ全ての描画を行うクラス
	/// シングルトンパターンで実装
	/// </summary>
	class MainRender {
	private://コンストラクタ等の隠蔽
		static std::unique_ptr<MainRender> instance_;

		MainRender() = default;//コンストラクタ隠蔽
		~MainRender() = default;//デストラクタ隠蔽
		MainRender(MainRender&) = delete;//コピーコンストラクタ封印
		MainRender& operator=(MainRender&) = delete;//コピー代入演算子封印

		friend struct std::default_delete<MainRender>;
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// シングルトンインスタンスの取得
		/// </summary>
		/// <returns>シングルトンインスタンス</returns>
		static MainRender* GetInstance();

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
		/// ImGui描画前処理
		/// </summary>
		void PreImGuiDraw();
		/// <summary>
		/// 描画後処理
		/// </summary>
		void PostDraw();
		/// <summary>
		/// 画面切り替え処理
		/// </summary>
		void ExchangeScreen();

		/// <summary>
		/// コマンドの準備
		/// </summary>
		void ReadyNextCommand();

		/// <summary>
		/// リソースの状態遷移用関数
		/// </summary>
		/// <param name="resource">リソースのポインタ</param>
		/// <param name="beforeState">前状態</param>
		/// <param name="afterState">後状態</param>
		void TransitionResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// コマンドアロケーターの取得
		/// </summary>
		/// <returns>コマンドアロケーター</returns>
		ID3D12CommandAllocator* GetCommandAllocator() const { return commandAllocator.Get(); }
		/// <summary>
		/// コマンドリストの取得
		/// </summary>
		/// <returns>コマンドリスト</returns>
		ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }
		/// <summary>
		/// スワップチェーンのバッファ数を取得
		/// </summary>
		/// <returns>スワップチェーンのバッファ数</returns>
		size_t GetSwapChainBufferCount() const { DXGI_SWAP_CHAIN_DESC1 desc{}; swapChain->GetDesc1(&desc); return desc.BufferCount; }
		/// <summary>
		/// スワップチェーンの取得
		/// </summary>
		/// <returns>スワップチェーン</returns>
		IDXGISwapChain4* GetSwapChain() const { return swapChain.Get(); }
		/// <summary>
		/// スワップチェーンのリソースの取得
		/// </summary>
		/// <param name="index">番号</param>
		/// <returns>スワップチェーンのリソース</returns>
		ID3D12Resource* GetSwapChainResource(uint32_t index) const { return swapChainResources[index].Get(); }
		/// <summary>
		/// DSVインデックス(オフスクで使うため)の取得
		/// </summary>
		/// <returns>DSVインデックス</returns>
		uint32_t GetDSVIndex()const { return dsvIndex; }

	private:
		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// コマンドの初期化
		/// </summary>
		void InitCommand();
		/// <summary>
		/// スワップチェーンの生成
		/// </summary>
		void GenerateSwapChain();
		/// <summary>
		/// 深度バッファの生成
		/// </summary>
		void GenerateDepthBuffer();
		/// <summary>
		/// RTVの初期化
		/// </summary>
		void InitRenderTargetView();
		/// <summary>
		/// DSVの初期化
		/// </summary>
		void InitDepthStencilView();
		/// <summary>
		/// ビューポートの初期化
		/// </summary>
		void InitViewPort();
		/// <summary>
		/// シザーレクトの初期化
		/// </summary>
		void InitScissorRect();

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//コマンドアロケーター
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
		//コマンドリスト
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
		//スワップチェーン
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;
		//スワップチェーンから引っ張て来たリソース
		std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources;
		//深度描画用のリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource = nullptr;
		//RTVインデックス
		std::array<uint32_t, 2> rtvIndices;
		//DSVインデックス
		uint32_t dsvIndex = 0;
		//ビューポート
		D3D12_VIEWPORT viewport{};
		//シザー矩形
		D3D12_RECT scissorRect{};

	};

}