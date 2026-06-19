#include "Selector.h"

namespace Norm {

	void Selector::Initialize() {
		//基底クラスの初期化
		CompositeNodeBase::Initialize();
	}

	void Selector::Update() {
		//稼働中の子ノードの更新
		mChildNodes[mRunningNodeIndex]->Update();
		auto result = mChildNodes[mRunningNodeIndex]->GetNodeResult();
		if (!mIsRevaluation) {
			mIsRevaluation = mChildNodes[mRunningNodeIndex]->GetIsRevaluation();
		}

		//もし失敗が返されたら次のノードに進める
		if (result == NodeResult::Fail) {
			// 次回Sequenceに向けてノード番号を進める
			NodeIncrement();
			return;
		}

		// もし成功が返されたらノード終了
		if (result == NodeResult::Success) {
			Finalize();
		}

		mNodeResult = result;
	}

	void Selector::Finalize() {
		//基底クラスの終了処理
		CompositeNodeBase::Finalize();

		//もしルートノードなら
		if (mNodeID == 1) {
			//再評価通知を受け取ったなら
			if (mIsRevaluation) {
				//再評価通知をリセット
				mIsRevaluation = false;
				//再びアップデートを行う
				this->Update();
			}
		}
	}

	void Selector::NodeIncrement() {
		while (true) {
			//現在のノードの後始末
			mChildNodes[mRunningNodeIndex]->Finalize();
			//インデックスを進める
			mRunningNodeIndex++;
			//もしすべての子ノードを試しても失敗したら
			if (mRunningNodeIndex >= mChildNodes.size()) {
				mNodeResult = NodeResult::Fail;
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

			//もし失敗が返されたら次のノードに進む
			if (result == NodeResult::Fail) {
				continue; // 次を即評価
			}
			//もし成功が返されたらノード終了
			if (result == NodeResult::Success) {
				Finalize();
			}

			mNodeResult = result;
			return;
		}
	}

}