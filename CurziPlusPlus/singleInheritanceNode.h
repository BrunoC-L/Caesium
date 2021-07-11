#pragma once
#include "andorNode.h"
#include "tokenNode.h"
#include "pppQualifierNode.h"
#include "typenameNode.h"

class SingleInheritanceNode : public Node {
public:
	baseCtor(SingleInheritanceNode);

	virtual void build() override {
		nodes = {
		_AND_
			MAKE(PPPQualifierNode)(),
			MAKE(TypenameNode)()
		}))
		};
	}
};
