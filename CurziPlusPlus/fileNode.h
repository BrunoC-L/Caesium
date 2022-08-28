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
				_STAR_("")
					MAKE_NAMED(ImportNode, "imports")
				___,
				_STAR_("classes_or_functions") _OR_
					MAKE_NAMED(ClassNode, "Class"),
					MAKE_NAMED(FunctionNode, "Function"),
				____,
				TOKEN(END)
			__
		};
	}

	std::unique_ptr<NodeStructs::File> getStruct() {
		std::unique_ptr<NodeStructs::File> res = std::make_unique<NodeStructs::File>();
		for (const std::shared_ptr<Node>& orNode : nodes[0]->nodes[1]->nodes)
			if (orNode->nodes[0]->identifier == "Class")
				res->classes.push_back(NODE_CAST(ClassNode, orNode->nodes[0])->getStruct());
			else
				res->functions.push_back(NODE_CAST(FunctionNode, orNode->nodes[0])->getStruct());
		return res;
	}
};
