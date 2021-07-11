#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "emptyNode.h"
#include "expressionNode.h"
#include "kNode.h"

class ArgumentsSignatureNode : public Node {
public:
	baseCtor(ArgumentsSignatureNode);

	virtual void build() override {
		nodes = {
			__OPT1
				_AND_
					MAKE(TypenameNode)(),
					MAKE(TokenNode)(WORD),
					__OPT1
						_AND_
							MAKE(TokenNode)(COMMA),
							MAKE(ArgumentsSignatureNode)(),
						__
					OPT1__
				__
			OPT1__
		};
	}
};


class ArgumentsNode : public Node {
public:
	baseCtor(ArgumentsNode);

	virtual void build() override {
		nodes = {
			_AND_
				MAKE(TokenNode)(PARENOPEN),
				_OR_
					_AND_
						MAKE(ExpressionNode)(),
						__STAR
							_AND_
								MAKE(TokenNode)(COMMA),
								MAKE(ExpressionNode)(),
							__
						STAR__,
					__,
					MAKE(EmptyNode)()
				__,
				MAKE(TokenNode)(PARENCLOSE),
			__
		};
	}
};
