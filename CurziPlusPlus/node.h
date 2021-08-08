#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include "nodeVisitor.h"

class Grammarizer;

template <typename T>
class Node {
public:
	std::vector<std::shared_ptr<Node>> nodes;
	std::string name = "Node";
	virtual void build() {}
	virtual bool build(Grammarizer* g);
	virtual T accept(NodeVisitor<T>* v) = 0;
};

template <typename T>
using vNode = std::vector<std::shared_ptr<Node<T>>>;
