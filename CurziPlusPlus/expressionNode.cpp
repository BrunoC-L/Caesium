#include "expressionNode.h"
#include "kNode.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "emptyNode.h"
#include "codeBlockNode.h"
#include "argumentsNode.h"

void ParenExpressionNode::build() {
	nodes = {
		_OR_
			_AND_
				MAKE(TokenNode)(PARENOPEN),
				MAKE(ExpressionNode)(),
				MAKE(TokenNode)(PARENCLOSE),
			__,
			MAKE(TokenNode)(WORD),
		__
	};
}

void ExpressionNode::build() {
	nodes = {
		MAKE(MultiplicativeExpressionNode)(),
	};
	/*_OR_
		_AND_
			MAKE(TokenNode)(PARENOPEN),
			MAKE(ExpressionNode)(),
			MAKE(TokenNode)(PARENCLOSE),
		__,
		MAKE(PostfixExpressionNode)(),
	__,*/
}

void PostfixExpressionNode::build() {
	nodes = {
		_AND_
			MAKE(ParenExpressionNode)(),
			__STAR
				_OR_
					_AND_
						MAKE(TokenNode)(DOT),
						MAKE(TokenNode)(WORD),
					__,
					MAKE(ArgumentsNode)(),
				__
			STAR__
		__,
	};
}

void MultiplicativeExpressionNode::build() {
	nodes = {
		_AND_
			MAKE(PostfixExpressionNode)(),
			__STAR
				_AND_
					_OR_
						MAKE(TokenNode)(ASTERISK),
						MAKE(TokenNode)(SLASH),
						MAKE(TokenNode)(PERCENT),
					__,
					MAKE(PostfixExpressionNode)(),
				__
			STAR__
		__
	};
}