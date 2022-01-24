#include "typenameNode.h"

void TypenameNode::build() {
	this->nodes = {
		_AND_
			WORD_TOKEN("word"),
			_STAR_("typenameExtensions") _OR_
				MAKE_NAMED(NSTypenameNode, "NSTypename"),
				MAKE_NAMED(PointerTypenameNode, "PointerTypename"),
				MAKE_NAMED(TemplateTypenameNode, "TemplateTypename"),
				MAKE_NAMED(ParenthesisTypenameNode, "ParenthesisTypename"),
			____
		__
	};
}
