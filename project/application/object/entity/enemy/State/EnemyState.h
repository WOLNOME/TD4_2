#pragma once
/// ===前方宣言=== ///
class BaseEnemy;

///=====================================================/// 
/// EnemyState
/// Enemyの状態を管理する基盤クラス
///=====================================================///
class EnemyState {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Enter(BaseEnemy* enemy) = 0;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void Exit();

protected:

	BaseEnemy* enemy_ = nullptr;
};

