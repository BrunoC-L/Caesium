#include "codeBlockNode.h"
#include "statementsNode.h"

void CodeBlockNode::build() {
	nodes = {
		_AND_
			MAKE(TokenNode)(BRACEOPEN),
			MAKE(StatementsNode)(),
			MAKE(TokenNode)(BRACECLOSE),
		__,
	};
}
