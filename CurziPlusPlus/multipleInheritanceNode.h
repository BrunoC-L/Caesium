#pragma once
#include "andorNode.h"
#include "tokenNode.h"
#include "pppQualifierNode.h"
#include "typenameNode.h"
#include "macros.h"

class MultipleInheritanceNode : public Node {
public:
	baseCtor(MultipleInheritanceNode);

	virtual void prepare() override {
		this->nodes = {
			_COMMA_PLUS_
				MAKE2(TypenameNode)
			___
		};
	}

	std::vector<std::unique_ptr<NodeStructs::Typename>> getInheritance() {
		std::vector<std::unique_ptr<NodeStructs::Typename>> res;
		for (const auto& node : nodes[0]->nodes)
			res.emplace_back(NODE_CAST(TypenameNode, node)->getStruct());
		return res;
	}
};
