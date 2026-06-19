#pragma once
#include "decoratorNode/DecoratorNodeBase.h"
#include <memory>

namespace Norm {

	/// <summary>
	///	子ノードの結果を反転させるノード
	/// ノードを再利用するために使用
	/// </summary>
	class Inverter : public DecoratorNodeBase {
	public:
		//コンストラクタ
		explicit Inverter(int _nodeID, BlackBoard* _blackBoard, std::unique_ptr<INode> _childNode);
		//デストラクタ
		~Inverter();

		//初期化
		void Initialize() override;
		//更新
		void Update() override;
		//終了
		void Finalize() override;
	};

}