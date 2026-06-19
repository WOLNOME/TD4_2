#pragma once
#include "interface/INode.h"
#include "BlackBoard.h"

namespace Norm {

	/// <summary>
	/// 各ノードが継承する基底クラス
	/// </summary>
	class NodeBase : public INode {
	protected:
		//コンストラクタ
		explicit NodeBase(int _nodeID, BlackBoard* _blackBoard) : mNodeID(_nodeID), mpBlackBoard(_blackBoard) {}
		//仮想デストラクタ
		virtual ~NodeBase() = default;
		//初期化
		virtual void Initialize() override { mNodeResult = NodeResult::Running; mIsRevaluation = false; }
		//更新
		virtual void Update() override {}
		//終了
		virtual void Finalize() override {}
		//デバッグ
		virtual void Debug() override {}

		//結果を取得
		NodeResult GetNodeResult() const override { return mNodeResult; }
		//再評価するかを取得
		bool GetIsRevaluation() override;

		NodeResult mNodeResult = NodeResult::Idle;  //ノードの状態
		bool mIsRevaluation = false;				//再評価するか

		BlackBoard* mpBlackBoard = nullptr; //ブラックボード
		int mNodeID = -1; //ノードID
	};

}