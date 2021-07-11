#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "classMemberQualifiers.h"
#include "argumentsNode.h"
#include "codeBlockNode.h"

class ClassMemberNode : public Node {
public:
	baseCtor(ClassMemberNode);

	virtual void build() override {
		nodes = {
		_AND_
			MAKE(ClassMemberQualifiers)(),
			MAKE(TypenameNode)(),
			MAKE(TokenNode)(WORD),
			_OR_
				_AND_
					MAKE(TokenNode)(PARENOPEN),
					MAKE(ArgumentsSignatureNode)(),
					MAKE(TokenNode)(PARENCLOSE),
					MAKE(CodeBlockNode)(),
				__,
				MAKE(TokenNode)(SEMICOLON),
			__,
		__,
		};
	}
};
