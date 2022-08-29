#include "statementNode.h"

void CodeBlockNode::build() {
	this->nodes = {
		_STAR_
			MAKE2(StatementNode)
		___
	};
}
