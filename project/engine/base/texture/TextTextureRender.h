#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <array>
#include <memory>

namespace Norm {

	/// <summary>
	/// テキストテクスチャの描画を行うクラス
	/// シングルトンパターンで実装
	/// </summary>
	class TextTextureRender {
	private://コンストラクタ等の隠蔽
		static std::unique_ptr<TextTextureRender> instance_;

		TextTextureRender() = default;//コンストラクタ隠蔽
		~TextTextureRender() = default;//デストラクタ隠蔽
		TextTextureRender(TextTextureRender&) = delete;//コピーコンストラクタ封印
		TextTextureRender& operator=(TextTextureRender&) = delete;//コピー代入演算子封印

		friend struct std::default_delete<TextTextureRender>;
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// インスタンスの取得
		/// </summary>
		/// <returns>インスタンス</returns>
		static TextTextureRender* GetInstance();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();

		/// <summary>
		/// ビューポートを設定する
		/// </summary>
		/// <param name="_width">幅</param>
		/// <param name="_height">高さ</param>
		void SettingViewPort(UINT _width, UINT _height);
		/// <summary>
		/// シザー矩形を設定する
		/// </summary>
		/// <param name="_width">幅</param>
		/// <param name="_height">高さ</param>
		void SettingScissorRect(UINT _width, UINT _height);
		/// <summary>
		/// 描画後の処理を行う
		/// </summary>
		void PostDraw();
		/// <summary>
		/// 次のコマンドを実行する準備を行う
		/// </summary>
		void ReadyNextCommand();

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// コマンドアロケーターを取得する
		/// </summary>
		/// <returns>コマンドアロケーター</returns>
		ID3D12CommandAllocator* GetCommandAllocator() const { return commandAllocator.Get(); }
		/// <summary>
		/// コマンドリストを取得する
		/// </summary>
		/// <returns>コマンドリスト</returns>
		ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }

	private:
		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// コマンドを初期化する
		/// </summary>
		void InitCommand();
		/// <summary>
		/// ビューポートを初期化する
		/// </summary>
		void InitViewPort();
		/// <summary>
		/// シザー矩形を初期化する
		/// </summary>
		void InitScissorRect();

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//コマンドアロケーター
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
		//コマンドリスト
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
		//ビューポート
		D3D12_VIEWPORT viewport{};
		//シザー矩形
		D3D12_RECT scissorRect{};

	};

}