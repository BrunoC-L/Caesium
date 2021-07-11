#include "statementNode.h"

void StatementNode::build() {
	nodes = {
	_OR_
		_AND_
			MAKE(ExpressionNode)(),
			MAKE(TokenNode)(SEMICOLON),
		__,
		MAKE(IfStatementNode)(),
	__,
	};
}
