#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include "nodeVisitor.h"

class Grammarizer;

class Node {
public:
	std::vector<std::shared_ptr<Node>> nodes;
	std::string name = "Node";
	virtual void build() {}
	virtual bool build(Grammarizer* g);
	virtual void accept(NodeVisitor* v) = 0;
};

using vNode = std::vector<std::shared_ptr<Node>>;
