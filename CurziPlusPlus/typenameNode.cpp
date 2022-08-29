#include "typenameNode.h"

void TypenameNode::build() {
	this->nodes = {
		_AND_
			TOKEN(WORD),
			_OPT_ _OR_
				MAKE2(NSTypenameNode),
				MAKE2(TemplateTypenameNode),
			____
		__
	};
}

std::unique_ptr<NodeStructs::Typename> TypenameNode::getStruct() {
	std::unique_ptr<NodeStructs::Typename> res = std::make_unique<NodeStructs::Typename>();
	res->type = NODE_CAST(TokenNode<WORD>, nodes[0]->nodes[0])->value;
	if (nodes[0]->nodes[1]->nodes.size()) {
		auto nst = NODE_CAST(NSTypenameNode, nodes[0]->nodes[1]->nodes[0]->nodes[0]);
		auto tt = NODE_CAST(TemplateTypenameNode, nodes[0]->nodes[1]->nodes[0]->nodes[0]);
		if (nst.get())
			res->extensions = nst->getTypenameExtensions();
		else
			res->extensions = tt->getTypenameExtensions();
	}
	return res;
}
