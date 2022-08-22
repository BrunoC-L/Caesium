#include "typenameNode.h"

void TypenameNode::build() {
	this->nodes = {
		_AND_
			WORD_TOKEN("word"),
			_OPT_("extension") _OR_
				MAKE_NAMED(NSTypenameNode, "NSTypename"),
				MAKE_NAMED(PointerTypenameNode, "PointerTypename"),
				MAKE_NAMED(TemplateTypenameNode, "TemplateTypename"),
			____
		__
	};
}

std::unique_ptr<NodeStructs::Typename> TypenameNode::getStruct() {
	std::unique_ptr<NodeStructs::Typename> res = std::make_unique<NodeStructs::Typename>();
	res->type = NODE_CAST(WordTokenNode, nodes[0]->nodes[0])->value;
	if (nodes[0]->nodes[1]->nodes.size()) {
		if (nodes[0]->nodes[1]->nodes[0]->nodes[0]->identifier == "NSTypename")
			res->extensions = NODE_CAST(NSTypenameNode, nodes[0]->nodes[1]->nodes[0]->nodes[0])->getTypenameExtensions();
		else if (nodes[0]->nodes[1]->nodes[0]->nodes[0]->identifier == "PointerTypename")
			res->extensions = NODE_CAST(PointerTypenameNode, nodes[0]->nodes[1]->nodes[0]->nodes[0])->getTypenameExtensions();
		else
			res->extensions = NODE_CAST(TemplateTypenameNode, nodes[0]->nodes[1]->nodes[0]->nodes[0])->getTypenameExtensions();
	}
	return res;
}
