#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <list>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Norm {

	class BaseCamera;

	/// <summary>
	/// 弾道トレール単体の処理全般を管理するクラス
	/// </summary>
	class BulletTrail {
		//弾丸トレールマネージャーに公開
		friend class BulletTrailManager;
	private:
		/// ============================== ///
		///		構造体
		/// ============================== ///

		/// <summary>
		/// 頂点データ
		/// </summary>
		struct VertexData {
			Vector4 position;
			Vector2 texCoord;
		};
		/// <summary>
		/// 弾丸トレール用リソース
		/// </summary>
		struct BulletTrailResource {
			Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
			D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
			VertexData* vertexData;
			Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
			D3D12_INDEX_BUFFER_VIEW indexBufferView;
			uint32_t* indexData;
		};

	public:
		/// ============================== ///
		///		メンバ関数
		/// ============================== ///

		/// <summary>
		/// コンストラクタ
		/// </summary>
		BulletTrail();
		/// <summary>
		/// デストラクタ
		/// </summary>
		~BulletTrail();

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="name">名前</param>
		/// <param name="maxVerLength">トレールの長さ</param>
		/// <param name="lengthDecayValue">減衰値</param>
		void Initialize(const std::string& name, float maxVerLength, float lengthDecayValue);
		/// <summary>
		/// 座標のクリア
		/// </summary>
		void ClearPositions();

		/// ============================== ///
		///		setter
		/// ============================== ///

		/// <summary>
		/// 座標のセット
		/// </summary>
		/// <param name="_position">座標</param>
		void SetPosition(const Vector3& _position);
		/// <summary>
		/// テクスチャのセット
		/// </summary>
		/// <param name="_textureHandle">テクスチャハンドル</param>
		void SetTexture(int32_t _textureHandle) { textureHandle_ = _textureHandle; }
		
	private:
		/// ============================== ///
		///		マネージャーへの委託処理
		/// ============================== ///

		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 描画
		/// </summary>
		/// <param name="_camera">カメラ</param>
		void Draw(BaseCamera* _camera);

		/// ============================== ///
		///		非公開メンバ関数
		/// ============================== ///

		/// <summary>
		/// 弾丸トレールリソースの生成
		/// </summary>
		/// <returns>弾丸トレールリソース</returns>
		BulletTrailResource CreateBulletTrailResource();

		/// ============================== ///
		///		メンバ変数
		/// ============================== ///

		//名前
		std::string name_;
		//シーンタグ
		std::string sceneTag_;

		//テクスチャ
		uint32_t textureHandle_ = 0u;
		//弾丸トレールのリソース
		BulletTrailResource resource_;

		//座標のリスト
		std::list<std::pair<Vector3, uint32_t>> positions_;
		//最大幅
		float verLength_;
		//幅の減少量
		float lengthDecayValue_;

		//描画するか
		bool isDisplay_ = false;

		//最大頂点数
		const int kMaxVertexNum_
			= 512;
		int indexCount_ = 0;

	};

}