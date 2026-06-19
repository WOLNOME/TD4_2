#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <vector>
#include <array>
#include <optional>
#include <memory>
//DirectXTex
#include "DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"

namespace Norm {

	/// <summary>
	/// テクスチャ全般を管理するクラス
	/// シングルトンパターンで実装
	/// </summary>
	class TextureManager {
	private:
		/// ============================== ///
		///		構造体
		/// ============================== ///

		//テクスチャデータ
		struct TextureData {
			DirectX::TexMetadata metadata;
			Microsoft::WRL::ComPtr<ID3D12Resource> resource;
			uint32_t srvIndex;
			std::string textureName;
		};

	private://コンストラクタ等の隠蔽
		static std::unique_ptr<TextureManager> instance_;

		TextureManager() = default;//コンストラクタ隠蔽
		~TextureManager() = default;//デストラクタ隠蔽
		TextureManager(TextureManager&) = delete;//コピーコンストラクタ封印
		TextureManager& operator=(TextureManager&) = delete;//コピー代入演算子封印

		friend struct std::default_delete<TextureManager>;
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// インスタンスの取得
		/// </summary>
		/// <returns>インスタンス</returns>
		static TextureManager* GetInstance();
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();

		/// <summary>
		/// テクスチャファイルを読み込む
		/// </summary>
		/// <param name="filePath">テクスチャファイルのパス</param>
		/// <returns>テクスチャハンドル</returns>
		uint32_t LoadTexture(const std::string& filePath);

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// テクスチャのメタデータを取得する
		/// </summary>
		/// <param name="textureHandle">テクスチャハンドル</param>
		/// <returns>テクスチャのメタデータ</returns>
		const DirectX::TexMetadata& GetMetaData(uint32_t textureHandle);
		/// <summary>
		/// SRVインデックスを取得する
		/// </summary>
		/// <param name="textureHandle">テクスチャハンドル</param>
		/// <returns>SRVインデックス</returns>
		uint32_t GetSrvIndex(uint32_t textureHandle);
		/// <summary>
		/// GPUディスクリプタハンドルを取得する
		/// </summary>
		/// <param name="textureHandle">テクスチャハンドル</param>
		/// <returns>GPUディスクリプタハンドル</returns>
		D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(uint32_t textureHandle);

		/// ============================== ///
		///		メンバ変数(public)
		/// ============================== ///

		//確保するテクスチャデータの最大数
		static const uint32_t kNumTextureData = 512;

	private:
		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// テクスチャデータをGPUに転送する
		/// </summary>
		/// <param name="texture">転送先のテクスチャリソース</param>
		/// <param name="mipImages">ミップマップ画像データ</param>
		/// <returns>アップロード済みのリソース</returns>
		Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

		/// ============================== ///
		///		メンバ変数(private)
		/// ============================== ///

		//テクスチャデータコンテナ(要素数がテクスチャハンドル)
		std::array<std::optional<TextureData>, kNumTextureData> textureDatas;
		//ディレクトリパス
		std::string directoryPath_;

	};

}