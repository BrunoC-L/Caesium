#include "classElementNode.h"

template <typename T>
void ClassElementNode<T>::build() {
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
