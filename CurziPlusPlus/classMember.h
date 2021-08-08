#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "classMemberQualifiers.h"
#include "argumentsNode.h"
#include "codeBlockNode.h"

template <typename T>
class ClassMemberNode : public Node<T> {
public:
	baseCtor(ClassMemberNode);

	virtual void build() override;

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};

template <typename T>
class MethodNode : public Node<T> {
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

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};

template <typename T>
class MemberVariableNode : public Node<T> {
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

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};
