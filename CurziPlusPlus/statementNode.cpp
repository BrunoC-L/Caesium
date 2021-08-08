#include "statementNode.h"
template <typename T>
void StatementNode<T>::build() {
	this->nodes = {
		_OR_
			_AND_
				MAKE(ExpressionNode)(),
				MAKE(TokenNode)(SEMICOLON),
			__,
			MAKE(IfStatementNode)(),
			MAKE(ForStatementNode)(),
			MAKE(TokenNode)(SEMICOLON),
		__,
	};
}
