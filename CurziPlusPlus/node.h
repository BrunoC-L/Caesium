#pragma once
#include <iostream>
#include <memory>
#include <vector>

class Grammarizer;

class Node {
public:
	std::vector<std::shared_ptr<Node>> nodes;
	std::string name = "Node";
	virtual void build() {}
	virtual bool build(Grammarizer* g);
};

using vNode = std::vector<std::shared_ptr<Node>>;
