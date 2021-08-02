#include "codeBlockNode.h"
#include "statementsNode.h"

void CodeBlockNode::build() {

	nodes = {
		_OR_
			_AND_
				MAKE(TokenNode)(BRACEOPEN),
				MAKE(StatementsNode)(),
				MAKE(TokenNode)(BRACECLOSE),
			__,
			MAKE(StatementNode)()
		__
	};
}