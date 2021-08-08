#include "classMember.h"

template <typename T>
void ClassMemberNode<T>::build() {
	this->nodes = {
		_OR_
			MAKE(MethodNode)(),
			MAKE(MemberVariableNode)(),
		__,
	};
}
