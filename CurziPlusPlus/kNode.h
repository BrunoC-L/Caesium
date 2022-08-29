#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include <functional>

class KNode : public Node {
protected:
	std::function<std::shared_ptr<Node>()> builder;
public:
	KNode(std::function<std::shared_ptr<Node>()> builder, int n_indent = 0) : Node(n_indent) , builder(builder) {}
	virtual bool cnd() = 0;

	virtual bool build(Grammarizer* g) override {
		while (true) {
			std::shared_ptr<Node> node = this->builder();
			bool parsed = node->build(g);
			if (parsed)
				this->nodes.push_back(node);
			else
				break;
		}
		return cnd();
	}
};

class CommaDelimitedKNode : public KNode {
public:
	CommaDelimitedKNode(std::function<std::shared_ptr<Node>()> builder) : KNode(builder) {}

	virtual bool build(Grammarizer* g) override {
		while (true) {
			std::shared_ptr<Node> node = this->builder();
			bool parsed = node->build(g);
			if (parsed) {
				this->nodes.push_back(node);
				parsed = TokenNode<COMMA>().build(g);
			}
			if (!parsed)
				break;
		}
		return cnd();
	}
};

class StarNode : public KNode {
public:
	StarNode(std::function<std::shared_ptr<Node>()> builder) : KNode(builder) {}
	virtual bool cnd() override {
		return true;
	}
};

class CommaStarNode : public CommaDelimitedKNode {
public:
	CommaStarNode(std::function<std::shared_ptr<Node>()> builder) : CommaDelimitedKNode(builder) {}
	virtual bool cnd() override {
		return true;
	}
};

class PlusNode : public KNode {
public:
	PlusNode(std::function<std::shared_ptr<Node>()> builder) : KNode(builder) {}
	virtual bool cnd() override {
		return this->nodes.size() > 0;
	}
};

class CommaPlusNode : public CommaDelimitedKNode {
public:
	CommaPlusNode(std::function<std::shared_ptr<Node>()> builder) : CommaDelimitedKNode(builder) {}
	virtual bool cnd() override {
		return this->nodes.size() > 0;
	}
};

// technically isnt a KNode but at heart it is
class OPTNode : public Node {
public:
	std::function<std::shared_ptr<Node>()> builder;
	OPTNode(std::function<std::shared_ptr<Node>()> builder) : Node(), builder(builder) {}

	virtual bool build(Grammarizer* g) override {
		std::shared_ptr<Node> node = this->builder();
		bool parsed = node->build(g);
		if (parsed)
			this->nodes.push_back(node);
		return true;
	}
};
