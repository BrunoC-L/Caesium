#include "typenameNode.h"
#include "kNode.h"

void TypenameNode::build() {
	this->nodes = {
		_AND_
			TOKEN(WORD),
			_STAR_ _OR_
				MAKE(NSTypenameNode)(),
				MAKE(PointerTypenameNode)(),
				MAKE(TemplateTypenameNode)(),
				MAKE(ParenthesisTypenameNode)(),
			____
		__
	};
}
