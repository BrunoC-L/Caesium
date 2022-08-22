#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include <functional>

class KNode : public Node {
protected:
	std::function<std::shared_ptr<Node>()> builder;
public:
	KNode(std::string name, std::function<std::shared_ptr<Node>()> builder, int n_indent = 0) : Node(name, n_indent) , builder(builder) {}
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
	CommaDelimitedKNode(std::string name, std::function<std::shared_ptr<Node>()> builder) : KNode(name, builder) {}

	virtual bool build(Grammarizer* g) override {
		while (true) {
			std::shared_ptr<Node> node = this->builder();
			bool parsed = node->build(g);
			if (parsed) {
				this->nodes.push_back(node);
				parsed = TokenNode<COMMA>("comma").build(g);
			}
			if (!parsed)
				break;
		}
		return cnd();
	}
};

class StarNode : public KNode {
public:
	StarNode(std::string name, std::function<std::shared_ptr<Node>()> builder) : KNode(name, builder) {
		this->name = "StarNode";
	}
	virtual bool cnd() override {
		return true;
	}
};

class CommaStarNode : public CommaDelimitedKNode {
public:
	CommaStarNode(std::string name, std::function<std::shared_ptr<Node>()> builder) : CommaDelimitedKNode(name, builder) {
		this->name = "CommaStarNode";
	}
	virtual bool cnd() override {
		return true;
	}
};

class PlusNode : public KNode {
public:
	PlusNode(std::string name, std::function<std::shared_ptr<Node>()> builder) : KNode(name, builder) {
		this->name = "PlusNode";
	}
	virtual bool cnd() override {
		return this->nodes.size() > 0;
	}
};

class CommaPlusNode : public CommaDelimitedKNode {
public:
	CommaPlusNode(std::string name, std::function<std::shared_ptr<Node>()> builder) : CommaDelimitedKNode(name, builder) {
		this->name = "CommaPlusNode";
	}
	virtual bool cnd() override {
		return this->nodes.size() > 0;
	}
};

// technically isnt a KNode but at heart it is
class OPTNode : public Node {
public:
	std::function<std::shared_ptr<Node>()> builder;
	OPTNode(std::string name, std::function<std::shared_ptr<Node>()> builder) : Node(name), builder(builder) {
		this->name = "OPTNode";
	}

	virtual bool build(Grammarizer* g) override {
		std::shared_ptr<Node> node = this->builder();
		bool parsed = node->build(g);
		if (parsed)
			this->nodes.push_back(node);
		return true;
	}
};
