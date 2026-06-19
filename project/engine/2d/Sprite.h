#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include "MyMath.h"
#include "Handle.h"

namespace Norm {

	//初期化用のタグ
	struct SpriteTag {};
	struct UVScrollTag {};
	struct TextTag {};

	/// ============================== ///
	///		列挙体
	/// ============================== ///

	/// <summary>
	/// スプライトの描画順タグ
	/// </summary>
	enum class Order {
		Back0,
		Back1,
		Back2,
		Back3,
		Back4,
		Back5,
		Front0,
		Front1,
		Front2,
		Front3,
		Front4,
		Front5,

		SceneTransition,	//シーン遷移用のタグ

		kMaxOrderNum,
	};

	/// <summary>
	/// スプライト単体の処理全般を管理するクラス
	/// </summary>
	class Sprite {
		//スプライトマネージャーに公開
		friend class SpriteManager;
	private:
		/// ============================== ///
		///		列挙体(private)
		/// ============================== ///

		/// <summary>
		/// テクスチャの種類
		/// </summary>
		enum class TextureKind {
			Normal,
			UVScroll,
			Text,

			kMaxTextureKindNum,
		};

		/// ============================== ///
		///		構造体(private)
		/// ============================== ///

		/// <summary>
		/// UVスクロール用のデータ構造体
		/// </summary>
		struct UVScrollData {
			int sheetsNum;			//合計枚数
			int currentSheetNum;	//現在のシートナンバー(0から始まる)
			float switchTime;		//切り替え時間
			float switchTimer;		//切り替えタイマー
			bool isLoop;			//ループさせるか
			bool isPlay;			//UVスクロールを動かす
			bool isFinished;		//終了したか
		};

	public:
		/// ============================== ///
		///		構造体(public)
		/// ============================== ///

		/// <summary>
		/// 頂点データ
		/// </summary>
		struct VertexData {
			Vector4 position;
			Vector2 texcoord;
			Vector3 normal;
		};
		/// <summary>
		/// マテリアル
		/// </summary>
		struct Material {
			Vector4 color;
			Matrix4x4 uvTransform;
			int32_t isTexture;
		};
		/// <summary>
		/// 座標変換行列
		/// </summary>
		struct TransformationMatrix {
			Matrix4x4 WVP;
			Matrix4x4 World;
		};
		/// <summary>
		/// GPU用リソース群
		/// </summary>
		struct ResourceForGPU {
			//頂点リソース
			Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
			VertexData* vertexData = nullptr;
			D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
			//インデックスリソース
			Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
			uint32_t* indexData = nullptr;
			D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
			//マテリアルリソース
			Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
			Material* materialData = nullptr;
			//座標変換行列リソース
			Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;
			TransformationMatrix* transformationMatrixData = nullptr;
			//テクスチャ用SRV番号
			uint32_t srvIndex = 0u;
		};

	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Sprite(bool _isNotUsingScene = false);
		/// <summary>
		/// デストラクタ
		/// </summary>
		~Sprite();
		/// <summary>
		/// 初期化(通常スプライト)
		/// </summary>
		/// <param name="">タグ</param>
		/// <param name="_name">名前</param>
		/// <param name="_order">描画順</param>
		/// <param name="_textureHandle">テクスチャハンドル</param>
		void Initialize(SpriteTag, const std::string& _name, const Order& _order, uint32_t _textureHandle);
		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="">タグ</param>
		/// <param name="_name">名前</param>
		/// <param name="_order">描画順</param>
		/// <param name="_sheetsNum">連番枚数</param>
		/// <param name="_switchTime">切り替えにかかる時間</param>
		/// <param name="_textureHandle">テクスチャハンドル</param>
		void Initialize(UVScrollTag, const std::string& _name, const Order& _order, int _sheetsNum, float _switchTime, bool _isLoop, uint32_t _textureHandle);
		/// <summary>
		/// 初期化(テキスト)
		/// </summary>
		/// <param name="">タグ</param>
		/// <param name="_name">名前</param>
		/// <param name="_order">描画順</param>
		void Initialize(TextTag, const std::string& _name, const Order& _order);
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 描画
		/// </summary>
		void Draw();
		/// <summary>
		/// ImGuiデバッグ表示
		/// </summary>
		void DebugWithImGui();

		/// ============================== ///
		///		getter
		/// ============================== ///

		/// <summary>
		/// 位置を取得する
		/// </summary>
		const Vector2& GetPosition() const { return position; }
		/// <summary>
		/// 回転角度を取得する
		/// </summary>
		float GetRotation() const { return rotation; }
		/// <summary>
		/// サイズを取得する
		/// </summary>
		const Vector2& GetSize() const { return size; }
		/// <summary>
		/// カラーを取得する
		/// </summary>
		const Vector4& GetColor() const { return resource_.materialData->color; }
		/// <summary>
		/// アンカーポイントを取得する
		/// </summary>
		const Vector2& GetAnchorPoint() const { return anchorPoint; }
		/// <summary>
		/// X方向の反転状態を取得する
		/// </summary>
		bool GetFlipX() const { return isFlipX_; }
		/// <summary>
		/// Y方向の反転状態を取得する
		/// </summary>
		bool GetFlipY() const { return isFlipY_; }
		/// <summary>
		/// テクスチャの左上座標を取得する
		/// </summary>
		const Vector2& GetTextureLeftTop() const { return textureLeftTop; }
		/// <summary>
		/// テクスチャのサイズを取得する
		/// </summary>
		const Vector2& GetTextureSize() const { return textureSize; }
		/// <summary>
		/// UVスクロールの終了フラグを取得する(UVスクロールかつループしない設定のみ)
		/// </summary>
		/// <returns>UVスクロールの終了フラグ</returns>
		bool GetIsFinishedUVScroll() const { return uvScrollData_.isFinished; }
		/// <summary>
		/// 表示状態の取得
		/// </summary>
		/// <returns>表示状態</returns>
		bool GetIsDisplay()const { return isDisplay_; }

		/// ============================== ///
		///		setter
		/// ============================== ///

		/// <summary>
		/// 位置を設定する
		/// </summary>
		/// <param name="position">設定する位置</param>
		void SetPosition(const Vector2& position) { this->position = position; }
		/// <summary>
		/// 回転角度を設定する
		/// </summary>
		/// <param name="rotation">設定する回転角度</param>
		void SetRotation(float rotation) { this->rotation = rotation; }
		/// <summary>
		/// サイズを設定する
		/// </summary>
		/// <param name="size">設定するサイズ</param>
		void SetSize(const Vector2& size) { this->size = size; }
		/// <summary>
		/// カラーを設定する
		/// </summary>
		/// <param name="color">設定するカラー</param>
		void SetColor(const Vector4& color) { resource_.materialData->color = color; }
		/// <summary>
		/// アンカーポイントを設定する
		/// </summary>
		/// <param name="anchorPoint">設定するアンカーポイント</param>
		void SetAnchorPoint(const Vector2& anchorPoint) { this->anchorPoint = anchorPoint; }
		/// <summary>
		/// X方向の反転状態を設定する
		/// </summary>
		/// <param name="isFlipX">反転状態</param>
		void SetFlipX(bool isFlipX) { isFlipX_ = isFlipX; }
		/// <summary>
		/// Y方向の反転状態を設定する
		/// </summary>
		/// <param name="isFlipY">反転状態</param>
		void SetFlipY(bool isFlipY) { isFlipY_ = isFlipY; }
		/// <summary>
		/// テクスチャの左上座標を設定する
		/// </summary>
		/// <param name="leftTop">設定する左上座標</param>
		void SetTextureLeftTop(const Vector2& leftTop) { textureLeftTop = leftTop; }
		/// <summary>
		/// テクスチャのサイズを設定する
		/// </summary>
		/// <param name="size">設定するテクスチャサイズ</param>
		void SetTextureSize(const Vector2& size) { textureSize = size; }
		/// <summary>
		/// シェイクオフセットを設定する
		/// </summary>
		/// <param name="_offset">設定するシェイクオフセット</param>
		void SetShakeOffset(const Vector2& _offset) { shakeOffset_ = _offset; }
		/// <summary>
		/// 表示状態を設定する
		/// </summary>
		/// <param name="_isDisplay">表示状態</param>
		void SetIsDisplay(bool _isDisplay) { isDisplay_ = _isDisplay; }
		/// <summary>
		/// テクスチャを設定する
		/// </summary>
		/// <param name="_textureHandle">設定するテクスチャハンドル</param>
		void SetTexture(Handle _textureHandle) { textTextureHandle_ = _textureHandle; }
		/// <summary>
		/// テクスチャを設定する（ハンドル指定）
		/// </summary>
		/// <param name="_textureHandle">設定するテクスチャハンドル</param>
		void SetTexture(uint32_t _textureHandle);
		/// <summary>
		/// UVスクロールの動作フラグを設定
		/// </summary>
		/// <param name="_isPlay">UVスクロールの動作フラグ</param>
		void SetIsPlayUVScroll(bool _isPlay) { uvScrollData_.isPlay = _isPlay; }
		/// <summary>
		/// UVスクロールのシート設定をする（0から始まる）
		/// </summary>
		/// <param name="_sheetNum">シートの番号</param>
		void SetUVScrollSheetNum(int _sheetNum) { uvScrollData_.currentSheetNum = _sheetNum; }
		/// <summary>
		/// UVスクロールが終了したかを設定
		/// </summary>
		/// <param name="_isFinishedUVScroll">UVスクロールが終了したか</param>
		void SetIsFinishedUVScroll(bool _isFinishedUVScroll) { uvScrollData_.isFinished = _isFinishedUVScroll; }

	private:
		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// GPU用リソースの生成
		/// </summary>
		/// <returns> GPU用リソース</returns>
		ResourceForGPU CreateResourceForGPU();

		/// <summary>
		/// テクスチャサイズの調整
		/// </summary>
		void AdjustTextureSize();

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//名前
		std::string name_;
		//シーンタグ
		std::string sceneTag_;

		//リソース
		ResourceForGPU resource_;

		//描画順タグ
		Order order_ = Order::kMaxOrderNum;

		//テクスチャ
		uint32_t textureHandle_ = 0u;
		Handle textTextureHandle_;
		//テクスチャの種類
		TextureKind textureKind_ = TextureKind::kMaxTextureKindNum;


		//パラメーター
		Vector2 position = { 0.0f,0.0f };
		float rotation = 0.0f;
		Vector2 size = { 640.0f,360.0f };
		Vector2 anchorPoint = { 0.0f,0.0f };
		//左右フリップ
		bool isFlipX_ = false;
		//上下フリップ
		bool isFlipY_ = false;
		//テクスチャ左上座標
		Vector2 textureLeftTop = { 0.0f,0.0f };
		//テクスチャ切り出しサイズ
		Vector2 textureSize = { 100.0f,100.0f };

		Vector2 shakeOffset_ = {};

		bool isDisplay_ = true;

		UVScrollData uvScrollData_ = {};	//UVスクロールのデータ


	};

}