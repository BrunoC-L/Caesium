#include "classElementNode.h"

void ClassElementNode::build() {
	this->nodes = {
		_OR_
			_AND_
				MAKE(PPPQualifierNode)(),
				MAKE(TokenNode)(COLON)
			__,
			MAKE(UsingNode)(),
			MAKE(MacroNode)(),
			MAKE(ClassMemberNode)(),
			MAKE(ConstructorNode)(),
		__,
	};
}
