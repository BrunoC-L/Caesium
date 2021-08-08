#pragma once
#include "classNode.h"

template <typename T>
class FileNode : public Node<T> {
public:
	baseCtor(FileNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(ClassNode)(),
				MAKE(TokenNode)(END)
			__
		};
	}

	virtual T accept(NodeVisitor<T>* v) {
		return v->visit(this);
	}
};
