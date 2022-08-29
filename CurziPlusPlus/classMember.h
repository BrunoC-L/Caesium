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
				MAKE2(ClassMemberQualifiers),
				MAKE2(TypenameNode),
				TOKEN(WORD),
				TOKEN(PARENOPEN),
				MAKE2(ArgumentsSignatureNode),
				TOKEN(PARENCLOSE),
				MAKE2(ColonIndentCodeBlockNode),
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
				MAKE2(ClassMemberQualifiers),
				MAKE2(TypenameNode),
				TOKEN(WORD),
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
				MAKE2(MethodNode),
				MAKE2(MemberVariableNode),
			__,
		};
	}
};
