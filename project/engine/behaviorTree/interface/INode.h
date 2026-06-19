#pragma once

namespace Norm {

	/// <summary>
	/// ノードの結果
	/// </summary>
	enum class NodeResult {
		Idle,		//待機中
		Running,	//実行中
		Success,	//成功
		Fail		//失敗
	};

	/// <summary>
	/// インターフェース
	/// </summary>
	class INode {
	public:
		//仮想デストラクタ
		virtual ~INode() = default;
		//初期化
		virtual void Initialize() = 0;
		//更新
		virtual void Update() = 0;
		//終了時
		virtual void Finalize() = 0;
		//デバッグ
		virtual void Debug() = 0;
		//ノードの状態を取得
		virtual NodeResult GetNodeResult() const = 0;
		//再評価するかを取得
		virtual bool GetIsRevaluation() = 0;

	};

}