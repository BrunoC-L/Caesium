#include "classMember.h"


void ClassMemberNode::build() {
	this->nodes = {
		_OR_
			MAKE(MethodNode)(n_indent),
			MAKE(MemberVariableNode)(n_indent),
		__,
	};
}
