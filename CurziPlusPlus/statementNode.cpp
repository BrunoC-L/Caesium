#include "statementNode.h"

void StatementNode::build() {
	this->nodes = {
		_AND_
			MAKE(IndentNode)(n_indent),
			_OR_
				MAKE(ExpressionStatementNode)(n_indent),
				MAKE(IfStatementNode)(n_indent),
				MAKE(ForStatementNode)(n_indent),
				MAKE(IForStatementNode)(n_indent),
			__
		__
	};
}
