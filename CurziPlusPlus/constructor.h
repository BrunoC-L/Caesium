#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "classMemberQualifiers.h"
#include "argumentsNode.h"
#include "codeBlockNode.h"

class ConstructorNode : public Node {
public:
	baseCtor(ConstructorNode);

	virtual void build() override {
		nodes = {
			_AND_
				_OPT_
					MAKE(PPPQualifierNode)()
				___,
				MAKE(TokenNode)(WORD),
				_AND_
					MAKE(TokenNode)(PARENOPEN),
					MAKE(ArgumentsSignatureNode)(),
					MAKE(TokenNode)(PARENCLOSE),
					MAKE(CodeBlockNode)(),
				__,
			__
		};
	}
};
