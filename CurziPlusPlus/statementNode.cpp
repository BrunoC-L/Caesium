#include "statementNode.h"

void CodeBlockNode::prepare() {
	this->nodes = {
		_STAR_
			MAKE2(StatementNode)
		___
	};
}
