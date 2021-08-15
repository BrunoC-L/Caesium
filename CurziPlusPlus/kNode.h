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

class StarNode : public KNode {
public:
	StarNode(std::function<std::shared_ptr<Node>()> builder) : KNode(builder) {
		this->name = "StarNode";
	}
	virtual bool cnd() override {
		return true;
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class PlusNode : public KNode {
public:
	PlusNode(std::function<std::shared_ptr<Node>()> builder) : KNode(builder) {
		this->name = "PlusNode";
	}
	virtual bool cnd() override {
		return this->nodes.size() > 0;
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class OPTNode : public KNode {
public:
	OPTNode(std::function<std::shared_ptr<Node>()> builder) : KNode(builder) {
		this->name = "OPTNode";
	}

	virtual bool cnd() override {
		return this->nodes.size() <= 1;
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
