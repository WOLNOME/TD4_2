#include "CompositeNodeBase.h"

namespace Norm {

	CompositeNodeBase::~CompositeNodeBase() {
		//子ノードを削除する
		mChildNodes.clear();
	}

	void CompositeNodeBase::Initialize() {
		//基底クラスの初期化
		NodeBase::Initialize();
		mRunningNodeIndex = 0;

		// 最初のノードを初期化
		if (mChildNodes.size() > 0) {
			mChildNodes[mRunningNodeIndex]->Initialize();
		}
		else {
			mNodeResult = NodeResult::Fail;
		}
	}

	void CompositeNodeBase::Finalize() {
		//基底クラスの終了処理
		NodeBase::Finalize();

		//最初のノードを初期化
		mRunningNodeIndex = 0;
		mChildNodes[mRunningNodeIndex]->Initialize();
	}

	void CompositeNodeBase::AddNode(std::unique_ptr<INode> _node) {
		//子ノードを追加
		mChildNodes.push_back(std::move(_node));
	}

}