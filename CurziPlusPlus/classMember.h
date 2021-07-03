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

	virtual bool build(Grammarizer* g) override {
		return
		_AND_(vNode({
			MAKE(ClassMemberQualifiers)(),
			MAKE(TypenameNode)(),
			MAKE(TokenNode)(WORD),
			_OR_(vNode({
				_AND_(vNode({
					MAKE(ArgumentsNode)(),
					MAKE(CodeBlockNode)(),
				})),
				MAKE(TokenNode)(SEMICOLON),
			})),
		}))
#ifdef DEBUG
		->debugbuild(g);
#else
		->build(g);
#endif // DEBUG
	}
};
