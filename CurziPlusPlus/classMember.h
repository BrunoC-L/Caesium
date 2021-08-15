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

	virtual void build() override;

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class MethodNode : public Node {
public:
	baseCtor(MethodNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(ClassMemberQualifiers)(),
				MAKE(TypenameNode)(),
				MAKE(TokenNode)(WORD),
				MAKE(TokenNode)(PARENOPEN),
				MAKE(ArgumentsSignatureNode)(),
				MAKE(TokenNode)(PARENCLOSE),
				MAKE(CodeBlockNode)(),
			__,
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class MemberVariableNode : public Node {
public:
	baseCtor(MemberVariableNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(ClassMemberQualifiers)(),
				MAKE(TypenameNode)(),
				MAKE(TokenNode)(WORD),
				MAKE(TokenNode)(SEMICOLON),
			__,
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
