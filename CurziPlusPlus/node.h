#pragma once
#include <iostream>
#include <memory>
#include <vector>

class Grammarizer;

class Node {
public:
	std::string name = "Node";
	virtual bool build(Grammarizer* g) = 0;

	virtual bool debugbuild(Grammarizer* g);
};

using vNode = std::vector<std::shared_ptr<Node>>;
#define MAKE(T) std::make_shared<T>
#define baseCtor(T) T() {\
	name = #T;\
}
