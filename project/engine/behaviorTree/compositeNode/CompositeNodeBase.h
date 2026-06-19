#pragma once
#include "nodeBase/NodeBase.h"
#include <vector>
#include <memory>

namespace Norm {

	/// <summary>
	/// Compositeノードの基底クラス
	/// </summary>
	class CompositeNodeBase : public NodeBase {
	public:
		//コンストラクタ
		explicit CompositeNodeBase(int _nodeID, BlackBoard* _blackBoard) : NodeBase(_nodeID, _blackBoard) {}
		//デストラクタ
		virtual ~CompositeNodeBase();

		//初期化
		virtual void Initialize() override;
		//終了
		virtual void Finalize() override;

		//ノードの追加
		void AddNode(std::unique_ptr<INode> _node);

	protected:
		//ノードのインクリメント
		virtual void NodeIncrement() = 0;

	protected:
		// 子ノード群
		std::vector<std::unique_ptr<INode>> mChildNodes;
		// 現在動かしているノードのインデックス
		int mRunningNodeIndex{ 0 };
	};

}