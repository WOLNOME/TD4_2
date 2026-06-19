#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "TextTextureRender.h"
#include "D2DRender.h"
#include "Handle.h"
#include "Vector2.h"
#include "Vector4.h"
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <wrl.h>
#include <string>
#include <list>
#include <vector>
#include <unordered_map>
#include <memory>

#pragma comment(lib, "dwrite.lib")

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace Norm {

	///=======================///
	///		　　列挙型
	///=======================///

	/// <summary>
	/// フォント
	/// </summary>
	enum class Font {
		Meiryo,
		YuGothic,
		YuMincho,
		UDDegitalN_B,
		UDDegitalN_R,
		UDDegitalNK_B,
		UDDegitalNK_R,
		UDDegitalNP_B,
		UDDegitalNP_R,
		OnionScript,

		kMaxFontNum,
	};
	/// <summary>
	/// フォントスタイル
	/// </summary>
	enum class FontStyle {
		Normal,		//通常
		Oblique,	//斜体(通常フォントをプログラムで斜体にする)
		Italic,		//斜体(フォントファイルベース)

		kMaxFontStyleNum,
	};

	///=======================///
	///		　　構造体
	///=======================///

	/// <summary>
	/// テキストのパラメータ
	/// </summary>
	struct TextParam {
		std::wstring text;									//書き込むテキスト
		Font font = Font::kMaxFontNum;						//フォント
		FontStyle fontStyle = FontStyle::kMaxFontStyleNum;	//フォントスタイル
		float size = 0.0f;									//文字のサイズ
		Vector4 color;										//文字の色
	};
	/// <summary>
	/// アウトラインのパラメータ
	/// </summary>
	struct EdgeParam {
		uint32_t isEdgeDisplay = 0u;	//アウトライン表示フラグ
		float width = 0.0f;				//アウトラインの幅
		Vector2 slideRate;				//アウトラインのスライド量
		Vector4 color;					//アウトラインの色
	};

	/// <summary>
	/// テキストテクスチャを管理するクラス
	/// シングルトンパターンで実装
	/// </summary>
	class TextTextureManager {
	private:
		/// ============================== ///
		///		構造体
		/// ============================== ///

		/// <summary>
		/// テキストのリソース
		/// </summary>
		struct TextResource {
			ComPtr<ID3D12Resource> resource;
			Vector4* color = nullptr;		//テキストの色(PSに送る)
			TextParam param;
			TextParam preParam;
		};

		/// <summary>
		/// アウトラインのリソース
		/// </summary>
		struct EdgeResource {
			ComPtr<ID3D12Resource> resource;
			EdgeParam* param = nullptr;
			EdgeParam preParam;
		};

		/// <summary>
		/// 各テキストテクスチャの必須項目
		/// </summary>
		struct TextTextureItem {
			ComPtr<ID3D12Resource> resource;				//テクスチャリソース
			ComPtr<ID3D12Resource> copyResource;			//コピー用テクスチャリソース
			ComPtr<ID3D11Resource> wrappedResource;			//D2D用のラップリソース
			ComPtr<ID2D1Bitmap1> d2dRenderTarget;			//D2D用のレンダーターゲット
			ComPtr<ID2D1SolidColorBrush> solidColorBrush;	//D2D用のブラシ
			ComPtr<IDWriteTextFormat> textFormat;			//DWrite用のテキストフォーマット
			TextResource textResource;						//テキストのリソース
			EdgeResource edgeResource;						//アウトラインのリソース
			uint32_t rtvIndex = 0u;								//RTVインデックス
			uint32_t srvIndex = 0u;								//SRVインデックス
			uint32_t srvCopyIndex = 0u;							//コピーリソース用SRVインデックス

			UINT width = 0u;									//テクスチャの横幅
			UINT height = 0u;								//テクスチャの縦幅
			bool isDrawThisFrame = true;
		};

	private://コンストラクタ等の隠蔽
		static std::unique_ptr<TextTextureManager> instance_;

		TextTextureManager() = default;//コンストラクタ隠蔽
		~TextTextureManager() = default;//デストラクタ隠蔽
		TextTextureManager(TextTextureManager&) = delete;//コピーコンストラクタ封印
		TextTextureManager& operator=(TextTextureManager&) = delete;//コピー代入演算子封印

		friend struct std::default_delete<TextTextureManager>;
	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		//シングルトンインスタンスの取得
		static TextTextureManager* GetInstance();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();
		/// <summary>
		/// ImGuiでテキストデバッグ
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		void DebugWithImGui(Handle _handle);

		/// <summary>
		/// D2Dで文字列を描画する
		/// </summary>
		void WriteTextOnD2D();
		/// <summary>
		/// D3D12でデコレーションを描画する
		/// </summary>
		void DrawDecorationOnD3D12();
		/// <summary>
		/// 次のフレームで使用可能なリソース状態に遷移する
		/// </summary>
		void ReadyNextResourceState();

		/// <summary>
		/// テクスチャの読み込み
		/// </summary>
		/// <param name="_textParam">テキストパラメーター</param>
		/// <returns>ハンドル</returns>
		Handle LoadTextTexture(const TextParam& _textParam);
		/// <summary>
		/// テクスチャの読み込み
		/// </summary>
		/// <param name="_id">番号</param>
		/// <returns>ハンドル</returns>
		Handle LoadTextTexture(uint32_t _id);

		/// <summary>
		/// 各種パラメータの編集
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <param name="_textParam">テキストパラメーター</param>
		void EditTextParam(Handle _handle, const TextParam& _textParam);
		/// <summary>
		/// アウトラインパラメータの編集
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <param name="_textParam">アウトラインパラメーター</param>
		void EditEdgeParam(Handle _handle, const EdgeParam& _edgeParam);

		//各パラメータの個別編集
		template <typename... Args>
		/// <summary>
		/// テキスト内容の編集
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <param name="text">テキスト</param>
		/// <param name="...args">参照変数等</param>
		void EditTextString(Handle _handle, const std::wstring& text, Args&&... args) {
			//使用可能なハンドルかチェック
			CheckHandle(_handle);

			//テキストを生成
			textTextureMap[_handle.id].textResource.param.text = std::vformat(text, std::make_wformat_args(args...));
		}

		/// <summary>
		/// フォントを編集する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <param name="_font">フォント</param>
		void EditTextFont(Handle _handle, const Font& _font);
		/// <summary>
		/// フォントスタイルを編集する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <param name="_fontStyle">フォントスタイル</param>
		void EditTextFontStyle(Handle _handle, const FontStyle& _fontStyle);
		/// <summary>
		/// テキストサイズを編集する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <param name="_size">文字サイズ</param>
		void EditTextSize(Handle _handle, const float _size);
		/// <summary>
		/// テキストカラーを編集する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <param name="_color">文字色</param>
		void EditTextColor(Handle _handle, const Vector4& _color);
		/// <summary>
		/// 文字の縁取り表示を設定する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <param name="_isDisplay">縁取りを表示するかどうか</param>
		void EditIsEdgeDisplay(Handle _handle, const bool _isDisplay);
		/// <summary>
		/// 縁取りの太さを設定する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <param name="_width">縁取りの太さ</param>
		void EditEdgeWidth(Handle _handle, const float _width);
		/// <summary>
		/// 縁取りのスライド率を設定する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <param name="_slideRate">縁取りのスライド率</param>
		void EditEdgeSlideRate(Handle _handle, const Vector2& _slideRate);
		/// <summary>
		/// 縁取りの色を設定する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <param name="_color">縁取りの色</param>
		void EditEdgeColor(Handle _handle, const Vector4& _color);
		/// <summary>
		/// テクスチャの幅を取得する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <returns>テクスチャの幅</returns>
		const UINT GetTextureWidth(Handle _handle);
		/// <summary>
		/// テクスチャの高さを取得する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <returns>テクスチャの高さ</returns>
		const UINT GetTextureHeight(Handle _handle);

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// テキスト文字列を取得する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <returns>テキスト文字列</returns>
		const std::wstring& GetTextString(Handle _handle);
		/// <summary>
		/// フォントを取得する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <returns>フォント</returns>
		const Font& GetTextFont(Handle _handle);
		/// <summary>
		/// フォントスタイルを取得する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <returns>フォントスタイル</returns>
		const FontStyle& GetTextFontStyle(Handle _handle);
		/// <summary>
		/// テキストサイズを取得する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <returns>文字サイズ</returns>
		const float GetTextSize(Handle _handle);
		/// <summary>
		/// テキストカラーを取得する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <returns>文字色</returns>
		const Vector4& GetTextColor(Handle _handle);

		/// <summary>
		/// 縁取り表示の有無を取得する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <returns>縁取りを表示する場合はtrue</returns>
		const bool GetIsEdgeDisplay(Handle _handle);
		/// <summary>
		/// 縁取りの太さを取得する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <returns>縁取りの太さ</returns>
		const float GetEdgeWidth(Handle _handle);
		/// <summary>
		/// 縁取りのスライド率を取得する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <returns>縁取りのスライド率</returns>
		const Vector2& GetEdgeSlideRate(Handle _handle);
		/// <summary>
		/// 縁取りの色を取得する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <returns>縁取りの色</returns>
		const Vector4& GetEdgeColor(Handle _handle);

		/// <summary>
		/// SRVインデックスを取得する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <returns>SRVインデックス</returns>
		uint32_t GetSrvIndex(Handle _handle);
		/// <summary>
		/// GPU用SRVハンドルを取得する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		/// <returns>GPUディスクリプタハンドル</returns>
		D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(Handle _handle);

	private:
		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// ハンドルの有効性を確認する
		/// </summary>
		/// <param name="_handle">ハンドル</param>
		void CheckHandle(Handle _handle);
		/// <summary>
		/// テキストテクスチャアイテムを作成する
		/// </summary>
		/// <param name="_textParam">テキストパラメータ</param>
		/// <returns>作成したテキストテクスチャアイテム</returns>
		TextTextureItem CreateTextTextureItem(const TextParam& _textParam);
		/// <summary>
		/// SolidColorBrushを作成する
		/// </summary>
		/// <param name="color">ブラシの色</param>
		/// <returns>作成したSolidColorBrush</returns>
		ComPtr<ID2D1SolidColorBrush> CreateSolidColorBrush(const Vector4& color);
		/// <summary>
		/// テキストフォーマットを作成する
		/// </summary>
		/// <param name="_font">フォント</param>
		/// <param name="_fontStyle">フォントスタイル</param>
		/// <param name="fontSize">フォントサイズ</param>
		/// <returns>作成したテキストフォーマット</returns>
		ComPtr<IDWriteTextFormat> CreateTextFormat(const Font& _font, const FontStyle& _fontStyle, const float fontSize) noexcept;
		/// <summary>
		/// リソースのステートを遷移させる
		/// </summary>
		/// <param name="pResource">リソース</param>
		/// <param name="before">遷移前のステート</param>
		/// <param name="after">遷移後のステート</param>
		void TransitionState(ID3D12Resource* pResource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
		/// <summary>
		/// IDWriteファクトリを生成する
		/// </summary>
		void GenerateIDWriteFactory();
		/// <summary>
		/// フォントファイルを生成する
		/// </summary>
		void GenerateFontFile();
		/// <summary>
		/// フォント名とスタイルからフォントキーを生成する
		/// </summary>
		/// <param name="fontName">フォント名</param>
		/// <param name="style">フォントスタイル</param>
		/// <returns>生成されたフォントキー</returns>
		std::string GenerateFontKey(const std::wstring& fontName, const FontStyle& style);
		/// <summary>
		/// グラフィックスパイプラインを生成する
		/// </summary>
		void GenerateGraphicsPipeline();
		/// <summary>
		/// 描画前の参照チェックを行う
		/// </summary>
		void CheckAllReference();
		/// <summary>
		/// テクスチャサイズを揃える
		/// </summary>
		/// <param name="_id">テクスチャID</param>
		void ArrangeTextureSize(uint32_t _id);


		/// ============================== ///
		///		インスタンス
		/// ============================== ///

		//省略変数
		WinApp* winapp = WinApp::GetInstance();
		DirectXCommon* dxcommon = DirectXCommon::GetInstance();
		TextTextureRender* ttrender = TextTextureRender::GetInstance();
		D2DRender* d2drender = D2DRender::GetInstance();

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//マネージャ全体での保存用変数
		ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;			//ルートシグネチャ
		ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;	//グラフィックスパイプライン

		ComPtr<IDWriteFactory8> directWriteFactory = nullptr;
		ComPtr<IDWriteFontCollection1> dwriteFontCollection = nullptr;
		std::unordered_map<std::string, ComPtr<IDWriteFontFace3>> fontFaceMap;	//各フォントで保持しておく項目

		//テキストテクスチャのコンテナ
		std::unordered_map<uint32_t, TextTextureItem> textTextureMap;	//各テキストで保持しておく項目(作成及び
		//最新の空きインデックス
		uint32_t useIndex = 0;
		//解放済みインデックスを管理するリスト(描画前参照チェック関数によって割り当てられる)
		std::list<uint32_t> freeIndices;
		//参照カウンタ
		std::list<Handle> referenceCounter;

		//アウトラインの最大幅
		const float kMaxEdgeWidth_ = 3.0f;

	};

	///フォントを追加する時の注意点
	//1. ttfもしくはttcファイルをResourcesフォルダ内のfontsフォルダに入れる
	//2. このファイルのFont列挙型に新たに追加する。
	//3. TextWrite.cppのReturnFontName関数にfontファイル名を新たに追加する(wstringを使っているが、あとでstringに直す処理を挟むので日本語禁止)
	//4. TextWrite.cppのDebugWithImGui関数のフォント欄に新たに追加する
	//5. TextWriteManager.cppのDrawOutline関数内のアウトラインの位置計算に新たに追加する。


}