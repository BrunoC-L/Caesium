#pragma once
#include "andorNode.h"
#include "tokenNode.h"
#include "pppQualifierNode.h"
#include "typenameNode.h"
#include "macros.h"

template <typename T>
class SingleInheritanceNode : public Node<T> {
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

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};
