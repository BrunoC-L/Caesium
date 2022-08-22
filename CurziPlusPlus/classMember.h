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
				MAKE_NAMED(ClassMemberQualifiers, "ClassMemberQualifiers"),
				MAKE_NAMED(TypenameNode, "Typename"),
				WORD_TOKEN("word"),
				TOKEN(PARENOPEN),
				MAKE_NAMED(ArgumentsSignatureNode, "ArgumentsSignature"),
				TOKEN(PARENCLOSE),
				MAKE_NAMED(ColonIndentCodeBlockNode, "ColonIndentCodeBlock"),
			__,
		};
	}
};

class MemberVariableNode : public Node {
public:
	baseCtor(MemberVariableNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE_NAMED(ClassMemberQualifiers, "ClassMemberQualifiers"),
				MAKE_NAMED(TypenameNode, "Typename"),
				WORD_TOKEN("varname"),
				TOKEN(NEWLINE),
			__,
		};
	}
};

class ClassMemberNode : public Node {
public:
	baseCtor(ClassMemberNode);

	virtual void build() override {
		this->nodes = {
			_OR_
				MAKE_NAMED(MethodNode, "Method"),
				MAKE_NAMED(MemberVariableNode, "MemberVariable"),
			__,
		};
	}
};
