#pragma once
#include "multipleInheritanceNode.h"

class ClassInheritanceNode : public Node {
public:
	baseCtor(ClassInheritanceNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				TOKEN(EXTENDS),
				MAKE_NAMED(MultipleInheritanceNode, ""),
			__
		};
	}

	std::vector<std::unique_ptr<NodeStructs::Typename>> getInheritance() {
		return NODE_CAST(MultipleInheritanceNode, nodes[0]->nodes[1])->getInheritance();
	}
};
