#pragma once

namespace Norm {

	/// <summary>
	/// メッシュの種類
	/// </summary>
	enum class MeshType {
		Model,				//通常モデル
		AnimationModel,		//アニメーションモデル
		Shape,				//単純形状

		kMaxNumMeshType,
	};

	/// <summary>
	/// オブジェクト名用グラフィックスパイプラインの種類
	/// </summary>
	enum class NameGPS {
		Normal,			//通常
		SkyBox,			//スカイボックス

		kMaxNumNameGPS,	//最大数
	};

	/// <summary>
	///	ステンシルの役割
	/// </summary>
	enum class StencilRole {
		Normal,				//通常モデル
		OlTarget,			//アウトライン適用モデル
		Outline,			//アウトラインモデル

		kMaxNumStencilRole,	//最大数
	};

}