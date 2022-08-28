#pragma once
#include "node.h"
#include "grammarizer.h"
#include "usingNode.h"
#include "pppQualifierNode.h"
#include "macroNode.h"
#include "classMember.h"
#include "constructor.h"

class ClassElementNode : public Node {
public:
	baseCtor(ClassElementNode);

	virtual void build() override {
		this->nodes = {
			_OR_
				MAKE_NAMED(UsingNode, "using"),
				//MAKE_NAMED(MacroNode, "using"),
				MAKE_NAMED(ClassMemberNode, "using"),
				MAKE_NAMED(ConstructorNode, "using"),
			__,
		};
	}

	std::unique_ptr<NodeStructs::ClassElement> getStruct() {
		std::unique_ptr<NodeStructs::ClassElement> res = std::make_unique<NodeStructs::ClassElement>();
		return res;
	}
};
