#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include "json.h"

class Grammarizer;

class Node {
public:
	std::vector<std::shared_ptr<Node>> nodes;
	std::string name = "Node";
	virtual void build() {}
	virtual bool build(Grammarizer* g);
	virtual JSON toJSON();
};

using vNode = std::vector<std::shared_ptr<Node>>;
