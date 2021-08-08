#include "typenameNode.h"
#include "kNode.h"

template <typename T>
void TypenameNode<T>::build() {
	this->nodes = {
		_AND_
			MAKE(TokenNode)(WORD),
			_STAR_ _OR_
				MAKE(NSTypenameNode)(),
				MAKE(PointerTypenameNode)(),
				MAKE(TemplateTypenameNode)(),
				MAKE(ParenthesisTypenameNode)(),
			____
		__
	};
}
