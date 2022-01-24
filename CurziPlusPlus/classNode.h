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
				_OPT_
					MAKE_NAMED(ClassInheritanceNode, "ClassInheritance")
				___,
				TOKEN(COLON),
				TOKEN(NEWLINE),
				_STAR_
					_AND_
						MAKE(IndentNode)(n_indent + 1),
						MAKE_NAMED_INDENTED(ClassElementNode, "ClassElement", n_indent + 1)
					__
				___,
			__,
		};
	}

	virtual void accept(NodeVisitor* v) {
		v->visit(this);
	}
};
