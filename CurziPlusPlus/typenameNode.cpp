#include "typenameNode.h"

void TypenameNode::build() {
	this->nodes = {
		_AND_
			WORD_TOKEN(),
			_STAR_ _OR_
				MAKE(NSTypenameNode)(),
				MAKE(PointerTypenameNode)(),
				MAKE(TemplateTypenameNode)(),
				MAKE(ParenthesisTypenameNode)(),
			____
		__
	};
}
