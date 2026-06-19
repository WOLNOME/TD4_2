#pragma once
#include "Vector4.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>
#include <wrl.h>
#include <chrono>

//DirectXTex
#include "DirectXTex.h"

#pragma comment(lib,"dxcompiler.lib")

namespace Norm {

	/// <summary>
	/// DirectXの機能を使うにあたり必要な処理をまとめたクラス
	/// シングルトンパターンで実装
	/// </summary>
	class DirectXCommon {
	private://コンストラクタ等の隠蔽
		static std::unique_ptr<DirectXCommon> instance_;

		DirectXCommon() = default;//コンストラクタ隠蔽
		~DirectXCommon() = default;//デストラクタ隠蔽
		DirectXCommon(DirectXCommon&) = delete;//コピーコンストラクタ封印
		DirectXCommon& operator=(DirectXCommon&) = delete;//コピー代入演算子封印

		friend struct std::default_delete<DirectXCommon>;

	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// シングルトンインスタンスの取得
		/// </summary>
		/// <returns>シングルトンインスタンス</returns>
		static DirectXCommon* GetInstance();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();

		/// <summary>
		/// 各レンダー完了時の後処理
		/// </summary>
		void PostEachRender();
		/// <summary>
		/// 全レンダー完了時の後処理
		/// </summary>
		void PostAllRenders();

		/// <summary>
		/// ShaderのCompiler
		/// </summary>
		/// <param name="filePath">ファイルパス</param>
		/// <param name="profile">プロファイル</param>
		/// <returns>コンパイル後のBlob</returns>
		Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
			//CompilerするShaderファイルへのパス
			const std::wstring& filePath,
			//Compilerに使用するProfile
			const wchar_t* profile
		);
		/// <summary>
		/// デスクリプタヒープの生成
		/// </summary>
		/// <param name="heapType">ヒープタイプ</param>
		/// <param name="numDescriptors">デスクリプタの数</param>
		/// <param name="shaderVisible">フラッグのタイプ</param>
		/// <returns>デスクリプタヒープ</returns>
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);
		/// <summary>
		/// バッファリソースの生成
		/// </summary>
		/// <param name="sizeInBytes">サイズ</param>
		/// <returns>バッファリソース</returns>
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);
		/// <summary>
		/// UAVバッファリソースの生成
		/// </summary>
		/// <param name="sizeInBytes">サイズ</param>
		/// <returns>UAVバッファリソース</returns>
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateUAVBufferResource(size_t sizeInBytes);
		/// <summary>
		/// テクスチャリソースの生成
		/// </summary>
		/// <param name="metadata">メタデータ</param>
		/// <returns>tテクスチャリソース</returns>
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);
		/// <summary>
		/// レンダーターゲット用テクスチャリソースの生成
		/// </summary>
		/// <param name="width">横幅</param>
		/// <param name="height">縦幅</param>
		/// <param name="format">フォーマット</param>
		/// <param name="clearColor">クリアカラー</param>
		/// <returns>レンダーターゲット用テクスチャリソース</returns>
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor);

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// CPUデスクリプタハンドルの取得
		/// </summary>
		/// <param name="descriptorHeap">デスクリプタヒープ</param>
		/// <param name="descriptorSize">デスクリプタのサイズ</param>
		/// <param name="index">番号</param>
		/// <returns>CPUデスクリプタハンドル</returns>
		static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);
		/// <summary>
		/// GPUデスクリプタハンドルの取得
		/// </summary>
		/// <param name="descriptorHeap">デスクリプタヒープ</param>
		/// <param name="descriptorSize">デスクリプタのサイズ</param>
		/// <param name="index">番号</param>
		/// <returns>GPUデスクリプタハンドル</returns>
		static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);

		/// <summary>
		/// DirectX12デバイスの取得
		/// </summary>
		/// <returns>DirectX12デバイス</returns>
		ID3D12Device* GetDevice() const { return device.Get(); }
		/// <summary>
		/// DXGIファクトリーの取得
		/// </summary>
		/// <returns>DXGIファクトリー</returns>
		IDXGIFactory7* GetDXGIFactory() const { return dxgiFactory.Get(); }
		/// <summary>
		/// コマンドキューの取得
		/// </summary>
		/// <returns>コマンドキュー</returns>
		ID3D12CommandQueue* GetCommandQueue() const { return commandQueue.Get(); }
		/// <summary>
		/// コマンドキューのアドレスの取得
		/// </summary>
		/// <returns>コマンドキューのアドレス</returns>
		ID3D12CommandQueue** GetAddressOfCommandQueue() { return commandQueue.GetAddressOf(); }


	private:
		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// デバイス生成
		/// </summary>
		void GenerateDevice();
		/// <summary>
		/// コマンドの初期化
		/// </summary>
		void InitCommand();
		/// <summary>
		/// フェンスの生成
		/// </summary>
		void GenerateFence();
		/// <summary>
		/// DXCコンパイラーの生成
		/// </summary>
		void GenerateDXCCompiler();

		/// <summary>
		/// FPS固定初期化
		/// </summary>
		void InitializeFixFPS();
		/// <summary>
		/// FPS固定更新
		/// </summary>
		void UpdateFixFPS();

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//記録時間(FPS固定用)
		std::chrono::steady_clock::time_point reference_;

		//DirectX12デバイス
		Microsoft::WRL::ComPtr<ID3D12Device> device = nullptr;
		//コマンドキュー
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
		// DXGIファクトリ
		Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
		//フェンス
		Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
		//フェンス値
		uint64_t fenceValue = 0;
		//イベント
		HANDLE fenceEvent;
		//DXCユーティリティ
		Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils = nullptr;
		//DXCコンパイラ
		Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
		//インクルードハンドラ
		Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler = nullptr;

	};

}