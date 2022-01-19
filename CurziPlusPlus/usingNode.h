#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"

class UsingNode : public Node {
public:
	baseCtor(UsingNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				TOKEN(USING),
				TOKEN(WORD),
				TOKEN(EQUAL),
				MAKE(TypenameNode)(),
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
