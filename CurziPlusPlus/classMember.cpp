#include "classMember.h"


void ClassMemberNode::build() {
	this->nodes = {
		_OR_
			MAKE(MethodNode)(),
			MAKE(MemberVariableNode)(),
		__,
	};
}
