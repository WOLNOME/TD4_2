#pragma once
#include "nodeBase/NodeBase.h"
#include <memory>

namespace Norm {

    /// <summary>
    /// 条件分岐ノードの基底クラス
    /// </summary>
    class DecoratorNodeBase : public NodeBase {
    public:
        //コンストラクタ
        explicit DecoratorNodeBase(int _nodeID, BlackBoard* _blackBoard);
        //デストラクタ
        virtual ~DecoratorNodeBase();

        //初期化
        virtual void Initialize() override;
        //更新
        virtual void Finalize() override;

        //子ノードのセット
        void SetNode(std::unique_ptr<INode> _node);
        
    protected:
        std::unique_ptr<INode> mChildNode = nullptr;
    };


}