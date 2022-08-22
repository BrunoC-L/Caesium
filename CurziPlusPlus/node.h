#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include "node_structs.h"

class Grammarizer;

class Node {
public:
	Node(std::string identifier, int n_indent = 0) : identifier(identifier), n_indent(n_indent) {}
	int n_indent;
	std::vector<std::shared_ptr<Node>> nodes;
	std::string name = "Node";
	std::string identifier;
	virtual void build() {}
	virtual bool build(Grammarizer* g) {
		build();
		return nodes[0]->build(g);
	}
};

using vNode = std::vector<std::shared_ptr<Node>>;
