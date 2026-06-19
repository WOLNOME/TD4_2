#pragma once
#include "nodeBase/NodeBase.h"
#include <memory>
#include <array>

namespace Norm {

	/// <summary>
	/// 条件に応じてTrue/Falseのいずれかの子を実行するためのノード
	/// </summary>
	class BranchNodeBase : public NodeBase {
	public:
		//コンストラクタ
		explicit BranchNodeBase(int _nodeID, BlackBoard* _blackBoard, std::unique_ptr<INode> _trueNode, std::unique_ptr<INode> _falseNode);
		//デストラクタ
		virtual ~BranchNodeBase();
		//初期化
		virtual void Initialize() override;
		//更新
		virtual void Update() override;
		//終了
		virtual void Finalize() override;

	protected:
		//状態フラグの取得
		virtual const bool IsCondition() = 0;

	protected:
		std::array<std::unique_ptr<INode>, 2> mpBranchNodes = { nullptr, nullptr }; // True,Falseそれぞれのノード
		int mSatisfyIndex = -1; // 条件を満たしているノードのインデックス
	};

}