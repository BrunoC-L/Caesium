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
#define MAKE(T) std::make_shared<T>
#define baseCtor(T) T() {\
	name = #T;\
}

#define nodeclass(T) class T : public Node {\
public:\
	baseCtor(T);\
	virtual void build() override;\
};
