#pragma once

namespace Norm {

	/// <summary>
	/// 当たり判定の全属性
	/// </summary>
	enum class CollisionAttribute {
		Player,						//プレイヤー
		Enemy,						//敵	
		Block, // 通常のブロック
		Area, // エリアブロック（敵が範囲外に出ないようにするため、ステージ周囲に敷き詰められたブロック）

		Nothingness,				//虚無オブジェクト
	};
}