#include "NodeBase.h"

namespace Norm {
	bool NodeBase::GetIsRevaluation() {
		bool isRevaluation = mIsRevaluation;
		mIsRevaluation = false;
		return isRevaluation;
	}
}