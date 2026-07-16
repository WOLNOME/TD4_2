#pragma once

namespace Norm {

	/// <summary>
	/// 当たり判定の全属性
	/// </summary>
	enum class CollisionAttribute {
		Player,						//プレイヤー
		Enemy,						//敵
		EnemyArea,					//敵の大き目のコライダー
		EnemyFoot,					//敵の足元
		Block,
		Area, // エリアブロック（敵が範囲外に出ないようにするため、ステージ周囲に敷き詰められたブロック）

		Nothingness,				//虚無オブジェクト
	};
}