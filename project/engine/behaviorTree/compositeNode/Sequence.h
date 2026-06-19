#pragma once
#include "compositeNode/CompositeNodeBase.h"

namespace Norm {

	/// <summary>
	/// シークエンスノード
	/// </summary>
	class Sequence : public CompositeNodeBase {
	public:
		//コンストラクタ
		explicit Sequence(int _nodeID, BlackBoard* _blackBoard) : CompositeNodeBase(_nodeID, _blackBoard) {}
		
		//初期化
		void Initialize() override;
		//更新
		void Update() override;
		//終了
		void Finalize() override;

	private:
		//ノードのインクリメント
		void NodeIncrement() override;
	};

}