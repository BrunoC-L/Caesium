#pragma once
#include "node.h"
#include "grammarizer.h"
#include <functional>

class KNode : public Node {
protected:
	std::function<std::shared_ptr<Node>()> builder;
public:
	KNode(std::function<std::shared_ptr<Node>()> builder) : builder(builder) {}
	virtual bool cnd() = 0;

	virtual bool build(Grammarizer* g) override {
		while (true) {
			std::shared_ptr<Node> node = builder();
			bool parsed = node->build(g);
			if (parsed)
				nodes.push_back(node);
			else
				break;
		}
		return cnd();
	}
};

class StarNode : public KNode {
public:
	StarNode(std::function<std::shared_ptr<Node>()> builder) : KNode(builder) {
		name = "StarNode";
	}
	virtual bool cnd() override {
		return true;
	}
};

class PlusNode : public KNode {
public:
	PlusNode(std::function<std::shared_ptr<Node>()> builder) : KNode(builder) {
		name = "PlusNode";
	}
	virtual bool cnd() override {
		return nodes.size() > 0;
	}
};

class Opt1Node : public KNode {
public:
	Opt1Node(std::function<std::shared_ptr<Node>()> builder) : KNode(builder) {
		name = "PlusNode";
	}
	virtual bool cnd() override {
		return nodes.size() <= 1;
	}
};

#define __STAR MAKE(StarNode)([&](){ return 
#define STAR__ ;})

#define __PLUS MAKE(PlusNode)([&](){ return 
#define PLUS__ ;})

#define __OPT1 MAKE(Opt1Node)([&](){ return 
#define OPT1__ ;})
