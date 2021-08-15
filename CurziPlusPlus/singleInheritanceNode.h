#pragma once
#include "andorNode.h"
#include "tokenNode.h"
#include "pppQualifierNode.h"
#include "typenameNode.h"
#include "macros.h"

class SingleInheritanceNode : public Node {
public:
	baseCtor(SingleInheritanceNode);

	virtual void build() override {
		this->nodes = {
		_AND_
			MAKE(PPPQualifierNode)(),
			MAKE(TypenameNode)()
		}))
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
