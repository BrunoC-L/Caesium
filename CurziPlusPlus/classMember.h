#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "classMemberQualifiers.h"
#include "argumentsNode.h"
#include "statementNode.h"

class MethodNode : public Node {
public:
	baseCtor(MethodNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(ClassMemberQualifiers)(),
				MAKE(TypenameNode)(),
				WORD_TOKEN(),
				TOKEN(PARENOPEN),
				MAKE(ArgumentsSignatureNode)(),
				TOKEN(PARENCLOSE),
				MAKE(ColonIndentCodeBlockNode)(n_indent),
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
				WORD_TOKEN(),
				TOKEN(NEWLINE),
			__,
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class ClassMemberNode : public Node {
public:
	baseCtor(ClassMemberNode);

	virtual void build() override {
		this->nodes = {
			_OR_
				MAKE(MethodNode)(n_indent),
				MAKE(MemberVariableNode)(n_indent),
			__,
		};
	}

	virtual void accept(NodeVisitor * v) override {
		v->visit(this);
	}
};
