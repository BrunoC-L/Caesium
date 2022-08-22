#pragma once
#include "classInheritanceNode.h"
#include "classelementnode.h"
#include "indentNode.h"

class ClassNode : public Node {
public:
	baseCtor(ClassNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				TOKEN(CLASS),
				WORD_TOKEN("word"),
				_OPT_("opt_inheritance")
					MAKE_NAMED(ClassInheritanceNode, "ClassInheritance")
				___,
				TOKEN(COLON),
				TOKEN(NEWLINE),
				_STAR_("classElements")
					_AND_
						MAKE(IndentNode)(n_indent + 1),
						MAKE_NAMED_INDENTED(ClassElementNode, "ClassElement", n_indent + 1)
					__
				___,
			__,
		};
	}

	std::unique_ptr<NodeStructs::Class> getStruct() {
		std::unique_ptr<NodeStructs::Class> res = std::make_unique<NodeStructs::Class>();
		res->name = NODE_CAST(WordTokenNode, nodes[0]->nodes[1])->value;
		if (nodes[0]->nodes[2]->nodes.size())
			res->inheritances = NODE_CAST(ClassInheritanceNode, nodes[0]->nodes[2]->nodes[0])->getInheritance();
		return res;
	}
};
