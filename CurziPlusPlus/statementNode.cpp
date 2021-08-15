#include "statementNode.h"

void StatementNode::build() {
	this->nodes = {
		_OR_
			MAKE(ExpressionStatementNode)(),
			MAKE(IfStatementNode)(),
			MAKE(ForStatementNode)(),
			MAKE(TokenNode)(SEMICOLON),
		__,
	};
}
