#pragma once
#include <windows.h>
#include <wrl.h>
#include <cstdint>
#include <memory>

namespace Norm {

	/// <summary>
	/// Windowsアプリケーションを管理するクラス
	/// シングルトンパターンで実装
	/// </summary>
	class WinApp {
	private://コンストラクタ等の隠蔽
		static std::unique_ptr<WinApp> instance_;

		WinApp() = default;//コンストラクタ隠蔽
		~WinApp() = default;//デストラクタ隠蔽
		WinApp(WinApp&) = delete;//コピーコンストラクタ封印
		WinApp& operator=(WinApp&) = delete;//コピー代入演算子封印
		
		friend struct std::default_delete<WinApp>;

	public:
		/// ============================== ///
		///		公開定数
		///	============================== ///

		//クライアント領域のサイズ
		static const int32_t kClientWidth = 1280;
		static const int32_t kClientHeight = 720;

		/// ============================== ///
		///		メンバ関数
		///	============================== ///

		/// <summary>
		/// シングルトンインスタンスの取得
		/// </summary>
		/// <returns>インスタンス</returns>
		static WinApp* GetInstance();

		/// <summary>
		/// ウィンドウプロシージャ
		/// </summary>
		/// <param name="hwnd">ウィンドウハンドル</param>
		/// <param name="msg">ウィンドウメッセージ</param>
		/// <param name="wparam">追加のメッセージ情報（WPARAM 型）</param>
		/// <param name="lparam">追加のメッセージ情報（LPARAM 型）</param>
		/// <returns>処理結果（LRESULT）。未処理メッセージは DefWindowProc に渡す。</returns>
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		/// <summary>
		/// ウィンドウクラスおよびウィンドウを初期化する
		/// </summary>
		void Initialize();
		/// <summary>
		/// ウィンドウ関連のリソースを解放する
		/// </summary>
		void Finalize();

		/// <summary>
		/// メッセージキューを処理する
		/// </summary>
		/// <returns>アプリケーションを継続する場合は true、終了する場合は false を返す。</returns>
		bool ProcessMessage();

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// ウィンドウハンドルを取得する
		/// </summary>
		/// <returns>アプリケーションウィンドウの HWND</returns>
		HWND GetHwnd() const { return hwnd; }
		/// <summary>
		/// アプリケーションインスタンスハンドルを取得する
		/// </summary>
		/// <returns>Win32 アプリケーションのインスタンスハンドル（HINSTANCE）</returns>
		HINSTANCE GetHInstance() const { return wc.hInstance; }


	private:
		/// ============================== ///
		///		メンバ変数
		///	============================== ///

		//ウィンドウハンドル
		HWND hwnd = nullptr;
		//ウィンドウクラスの設定
		WNDCLASS wc{};

	};

}