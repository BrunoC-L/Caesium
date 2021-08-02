#include "typenameNode.h"
#include "kNode.h"

void TypenameNode::build() {
	nodes = {
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
