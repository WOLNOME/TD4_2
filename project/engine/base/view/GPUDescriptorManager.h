#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>
#include <queue>
#include <memory>

namespace Norm {

	/// <summary>
	/// SRV及びUAVの管理を行うクラス
	/// シングルトンパターンで実装
	/// </summary>
	class GPUDescriptorManager {
	private:
		static std::unique_ptr<GPUDescriptorManager> instance_;

		GPUDescriptorManager() = default;//コンストラクタ隠蔽
		~GPUDescriptorManager() = default;//デストラクタ隠蔽
		GPUDescriptorManager(GPUDescriptorManager&) = delete;//コピーコンストラクタ封印
		GPUDescriptorManager& operator=(GPUDescriptorManager&) = delete;//コピー代入演算子封印

		friend struct std::default_delete<GPUDescriptorManager>;

	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// インスタンスの取得
		/// </summary>
		/// <returns>インスタンス</returns>
		static GPUDescriptorManager* GetInstance();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();

		/// <summary>
		/// 描画前設定
		/// </summary>
		/// <param name="pCommandList"></param>
		void SetDescriptorHeap(ID3D12GraphicsCommandList* pCommandList);
		/// <summary>
		/// 割り当て用関数
		/// </summary>
		/// <returns>番号</returns>
		uint32_t Allocate();
		/// <summary>
		/// 解放用関数
		/// </summary>
		/// <param name="index">番号</param>
		void Free(uint32_t index);
		/// <summary>
		/// 空きインデックスの存在確認用関数
		/// </summary>
		/// <returns>判定結果</returns>
		bool CheckCanSecured();
		/// <summary>
		/// 使用不可能インデックスを使用可能インデックスに遷移させる関数
		/// </summary>
		void TransferEnable();

		/// <summary>
		/// レンダーテクスチャ用SRVの生成関数
		/// </summary>
		/// <param name="index">番号</param>
		/// <param name="pResource">リソースのポインタ</param>
		void CreateSRVforRenderTexture(uint32_t index, ID3D12Resource* pResource);
		/// <summary>
		/// ストラクチャードバッファ用SRVの生成関数
		/// </summary>
		/// <param name="index">番号</param>
		/// <param name="pResource">リソースのポインタ</param>
		/// <param name="numElements">素材の数</param>
		/// <param name="structureByteStride">範囲</param>
		void CreateSRVforStructuredBuffer(uint32_t index, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);
		/// <summary>
		/// RWストラクチャ―ドバッファ用UAV生成関数
		/// </summary>
		/// <param name="index">番号</param>
		/// <param name="pResource">リソースのポインタ</param>
		/// <param name="numElements">素材の数</param>
		/// <param name="structureByteStride">範囲</param>
		void CreateUAVforRWStructuredBuffer(uint32_t index, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// デスクリプタヒープの取得
		/// </summary>
		/// <returns></returns>
		ID3D12DescriptorHeap* GetDescriptorHeap() const { return descriptorHeap.Get(); }
		/// <summary>
		/// CPUデスクリプタハンドルの取得
		/// </summary>
		/// <param name="index">番号</param>
		/// <returns>ハンドル</returns>
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
		/// <summary>
		/// GPUデスクリプタハンドルの取得
		/// </summary>
		/// <param name="index">番号</param>
		/// <returns>ハンドル</returns>
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

		/// ============================== ///
		///		setter
		/// ============================== ///

		/// <summary>
		/// デスクリプタテーブルのセット
		/// </summary>
		/// <param name="pCommandList">コマンドリストのポインタ</param>
		/// <param name="RootParameterIndex">ルートパラメータの番号</param>
		/// <param name="index">番号</param>
		void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* pCommandList, UINT RootParameterIndex, uint32_t index);

		/// ============================== ///
		///		メンバ変数(public)
		/// ============================== ///

		static const uint32_t kMaxHeapSize;

	private:
		/// ============================== ///
		///		メンバ変数(private)
		/// ============================== ///

		//デスクリプタのサイズ
		uint32_t descriptorSize = 0;
		//デスクリプタヒープ
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
		//最新の空きインデックス
		uint32_t useIndex = 0;

		// 使用可能空きインデックスを管理するキュー
		std::queue<uint32_t> enableIndices;
		// 使用不可能空きインデックスを管理するキュー
		std::queue<uint32_t> unenableIndices;

	};

}