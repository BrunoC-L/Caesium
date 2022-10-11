#pragma once
#include "node.h"
#include "grammarizer.h"
#include "usingNode.h"
#include "pppQualifierNode.h"
#include "classMember.h"
#include "constructor.h"

class ClassElementNode : public Node {
public:
	baseCtor(ClassElementNode);

	virtual void prepare() override {
		this->nodes = {
			_OR_
				MAKE2(UsingNode),
				MAKE2(MethodNode),
				MAKE2(MemberVariableNode),
				MAKE2(ConstructorNode),
			__,
		};
	}

	std::unique_ptr<NodeStructs::ClassElement> getStruct() {
		std::unique_ptr<NodeStructs::ClassElement> res;
		if (auto node = NODE_CAST(UsingNode, nodes[0]->nodes[0])) {
			NodeStructs::Alias x{ nullptr, nullptr };
			return std::make_unique<NodeStructs::ClassElement>(std::move(x));
		}
		else if (auto node = NODE_CAST(MethodNode, nodes[0]->nodes[0])) {
			NodeStructs::Function x{};
			return std::make_unique<NodeStructs::ClassElement>(std::move(x));
		}
		else if (auto node = NODE_CAST(MemberVariableNode, nodes[0]->nodes[0])) {
			NodeStructs::MemberVariable x{};
			return std::make_unique<NodeStructs::ClassElement>(std::move(x));
		}
		else if (auto node = NODE_CAST(ConstructorNode, nodes[0]->nodes[0])) {
			NodeStructs::Constructor x{};
			return std::make_unique<NodeStructs::ClassElement>(std::move(x));
		}
		return res;
	}
};
