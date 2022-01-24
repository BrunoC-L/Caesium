#pragma once
#include "andorNode.h"
#include "tokenNode.h"
#include "pppQualifierNode.h"
#include "typenameNode.h"
#include "macros.h"

class MultipleInheritanceNode : public Node {
public:
	baseCtor(MultipleInheritanceNode);

	virtual void build() override {
		this->nodes = {
			_COMMA_PLUS_("typenames")
				MAKE_NAMED(TypenameNode, "typename")
			___
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
