#include "statementNode.h"

void CodeBlockNode::build() {
	this->nodes = {
		_STAR_("statements")
			MAKE_NAMED(StatementNode, "Statement")
		___
	};
}
