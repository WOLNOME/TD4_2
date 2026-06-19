#include "Inverter.h"

namespace Norm {

	Inverter::Inverter(int _nodeID, BlackBoard* _blackBoard, std::unique_ptr<INode> _childNode) : DecoratorNodeBase(_nodeID, _blackBoard) {
		//子ノードのセット
		SetNode(std::move(_childNode));
	}

	Inverter::~Inverter() {
	}

	void Inverter::Initialize() {
		//基底クラスの初期化
		DecoratorNodeBase::Initialize();
	}

	void Inverter::Update() {
		// 子ノードを実行
		mChildNode->Update();
		// 子ノードの結果を取得
		NodeResult result = mChildNode->GetNodeResult();
		bool isRevaluaiton = mChildNode->GetIsRevaluation();
		// 結果を反転させる
		if (result == NodeResult::Success) {
			mNodeResult = NodeResult::Fail;
			return;
		}
		else if (result == NodeResult::Fail) {
			mNodeResult = NodeResult::Success;
			return;
		}

		// 子ノードが実行中の場合は、Inverterも実行中にする
		mNodeResult = NodeResult::Running;
	}

	void Inverter::Finalize() {
		//基底クラスの終了
		DecoratorNodeBase::Finalize();
	}

}