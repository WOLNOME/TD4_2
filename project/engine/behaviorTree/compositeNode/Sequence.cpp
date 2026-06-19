#include "Sequence.h"

namespace Norm {

	void Sequence::Initialize() {
		//基底クラスの初期化
		CompositeNodeBase::Initialize();
	}

	void Sequence::Update() {
		//稼働中の子ノードの更新
		mChildNodes[mRunningNodeIndex]->Update();
		auto result = mChildNodes[mRunningNodeIndex]->GetNodeResult();
		if (!mIsRevaluation) {
			mIsRevaluation = mChildNodes[mRunningNodeIndex]->GetIsRevaluation();
		}

		if (result == NodeResult::Success) {
			// 次回Sequenceに向けてノード番号を進める
			NodeIncrement();
			return;
		}

		// もし失敗が返されたらノード終了
		if (result == NodeResult::Fail) {
			Finalize();
		}

		mNodeResult = result;
	}

	void Sequence::Finalize() {
		//基底クラスの終了
		CompositeNodeBase::Finalize();
	}

	void Sequence::NodeIncrement() {
		while (true) {
			//現在のノードの後始末
			mChildNodes[mRunningNodeIndex]->Finalize();
			//インデックスを進める
			mRunningNodeIndex++;
			//もしすべての子ノードを試しても成功したら
			if (mRunningNodeIndex >= mChildNodes.size()) {
				mNodeResult = NodeResult::Success;
				Finalize();
				return;
			}
			//次に回すノードの初期化
			mChildNodes[mRunningNodeIndex]->Initialize();
			//ノードの更新
			mChildNodes[mRunningNodeIndex]->Update();
			auto result = mChildNodes[mRunningNodeIndex]->GetNodeResult();
			if (!mIsRevaluation) {
				mIsRevaluation = mChildNodes[mRunningNodeIndex]->GetIsRevaluation();
			}

			//もし成功が返されたら次のノードに進む
			if (result == NodeResult::Success) {
				continue; // 次を即評価
			}
			//もし失敗が返されたらノード終了
			if (result == NodeResult::Fail) {
				Finalize();
			}

			mNodeResult = result;
			return;
		}
	}

}