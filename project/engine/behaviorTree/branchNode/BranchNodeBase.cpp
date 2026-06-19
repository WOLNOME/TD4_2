#include "BranchNodeBase.h"

namespace Norm {

	BranchNodeBase::BranchNodeBase(int _nodeID, BlackBoard* _blackBoard, std::unique_ptr<INode> _trueNode, std::unique_ptr<INode> _falseNode) : NodeBase(_nodeID, _blackBoard) {
		mpBranchNodes[0] = std::move(_trueNode);
		mpBranchNodes[1] = std::move(_falseNode);
	}

	BranchNodeBase::~BranchNodeBase() {
	}

	void BranchNodeBase::Initialize() {
		//基底クラスの初期化
		NodeBase::Initialize();

		if (IsCondition()) mSatisfyIndex = 0;
		else mSatisfyIndex = 1;

		mpBranchNodes[mSatisfyIndex]->Initialize();
	}

	void BranchNodeBase::Update() {
		//条件を満たしているノードの更新
		mpBranchNodes[mSatisfyIndex]->Update();
		mNodeResult = mpBranchNodes[mSatisfyIndex]->GetNodeResult();
		mIsRevaluation = mpBranchNodes[mSatisfyIndex]->GetIsRevaluation();
	}

	void BranchNodeBase::Finalize() {
		//基底クラスの終了
		NodeBase::Finalize();
		mpBranchNodes[mSatisfyIndex]->Finalize();
		mSatisfyIndex = -1;
	}

}