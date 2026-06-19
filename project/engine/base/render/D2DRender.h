#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "MainRender.h"
#include <d2d1_3.h>
#include <d3d11on12.h>
#include <wrl.h>
#include <vector>
#include <memory>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace Norm {

	/// <summary>
	/// Direct2Dの描画を行うクラス
	/// シングルトンパターンで実装
	/// </summary>
	class D2DRender {
	private://コンストラクタ等の隠蔽
		static std::unique_ptr<D2DRender> instance_;

		D2DRender() = default;//コンストラクタ隠蔽
		~D2DRender() = default;//デストラクタ隠蔽
		D2DRender(D2DRender&) = delete;//コピーコンストラクタ封印
		D2DRender& operator=(D2DRender&) = delete;//コピー代入演算子封印

		friend struct std::default_delete<D2DRender>;
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		//シングルトンインスタンスの取得
		static D2DRender* GetInstance();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();

		/// <summary>
		/// 描画前処理
		/// </summary>
		void PreDraw();
		/// <summary>
		/// 描画後処理
		/// </summary>
		void PostDraw();

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// D3D11On12Deviceの取得
		/// </summary>
		/// <returns>D3D11On12Device</returns>
		ID3D11On12Device* GetD3D11On12Device() const { return d3d11On12Device.Get(); }
		/// <summary>
		/// D3D11On12DeviceContextの取得
		/// </summary>
		/// <returns>D3D11On12DeviceContext</returns>
		ID3D11DeviceContext* GetD3D11On12DeviceContext() const { return d3d11On12DeviceContext.Get(); }
		/// <summary>
		/// D2DFactoryの取得
		/// </summary>
		/// <returns>D2DFactory</returns>
		ID2D1Factory3* GetD2DFactory() const { return d2dFactory.Get(); }
		/// <summary>
		/// D2DDeviceContextの取得
		/// </summary>
		/// <returns>D2DDeviceContext</returns>
		ID2D1DeviceContext2* GetD2DDeviceContext() const { return d2dDeviceContext.Get(); }

	private:
		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// D2Dリソースの生成
		/// </summary>
		void CreateD2DResources();

		/// ============================== ///
		///		インスタンス
		/// ============================== ///

		WinApp* winapp = WinApp::GetInstance();
		DirectXCommon* dxcommon = DirectXCommon::GetInstance();
		MainRender* mainrender = MainRender::GetInstance();

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//D3D11On12Device
		ComPtr<ID3D11On12Device> d3d11On12Device = nullptr;
		//D3D11On12DeviceContext
		ComPtr<ID3D11DeviceContext> d3d11On12DeviceContext = nullptr;
		//D2DFactory
		ComPtr<ID2D1Factory3> d2dFactory = nullptr;
		//D2DDevice
		ComPtr<ID2D1DeviceContext2> d2dDeviceContext = nullptr;
		std::vector<ComPtr<ID3D11Resource>> wrappedBackBuffers;
		std::vector<ComPtr<ID2D1Bitmap1>> d2dRenderTargets;
	};

}