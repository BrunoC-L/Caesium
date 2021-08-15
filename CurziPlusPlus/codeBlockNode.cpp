#include "codeBlockNode.h"
#include "statementNode.h"


void CodeBlockNode::build() {
	this->nodes = {
		_OR_
			MAKE(BracedCodeBlockNode)(),
			MAKE(StatementNode)()
		__
	};
}

void BracedCodeBlockNode::build() {
	this->nodes = {
		_AND_
			MAKE(TokenNode)(BRACEOPEN),
			_STAR_
				MAKE(StatementNode)()
			___,
			MAKE(TokenNode)(BRACECLOSE),
		__
	};
}
