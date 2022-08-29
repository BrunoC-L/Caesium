#pragma once
#include "classNode.h"
#include "functionNode.h"
#include <fstream>
#include "node_structs.h"
#include "importNode.h"

class FileNode : public Node {
public:
	baseCtor(FileNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				_STAR_
					MAKE2(ImportNode)
				___,
				_STAR_ _OR_
					MAKE2(ClassNode),
					MAKE2(FunctionNode),
				____,
				TOKEN(END)
			__
		};
	}

	std::unique_ptr<NodeStructs::File> getStruct() {
		std::unique_ptr<NodeStructs::File> res = std::make_unique<NodeStructs::File>();
		for (const std::shared_ptr<Node>& orNode : nodes[0]->nodes[1]->nodes) {
			auto c = NODE_CAST(ClassNode, orNode->nodes[0]);
			auto f = NODE_CAST(FunctionNode, orNode->nodes[0]);
			if (c.get())
				res->classes.push_back(c->getStruct());
			else
				res->functions.push_back(f->getStruct());
		}
		return res;
	}
};
