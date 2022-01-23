#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include "nodeVisitor.h"

class Grammarizer;

class Node {
public:
	Node(int n_indent = 0) : n_indent(n_indent) {}
	int n_indent;
	std::vector<std::shared_ptr<Node>> nodes;
	std::string name = "Node";
	virtual void build() {}
	virtual bool build(Grammarizer* g) {
		build();
		return nodes[0]->build(g);
	}
	virtual void accept(NodeVisitor* v) = 0;
};

using vNode = std::vector<std::shared_ptr<Node>>;
