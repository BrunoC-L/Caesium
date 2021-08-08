#pragma once
#include "node.h"
#include "grammarizer.h"
#include <functional>

template <typename T>
class KNode : public Node<T> {
protected:
	std::function<std::shared_ptr<Node<T>>()> builder;
public:
	KNode(std::function<std::shared_ptr<Node<T>>()> builder) : builder(builder) {}
	virtual bool cnd() = 0;

	virtual bool build(Grammarizer* g) override {
		while (true) {
			std::shared_ptr<Node<T>> node = this->builder();
			bool parsed = node->build(g);
			if (parsed)
				this->nodes.push_back(node);
			else
				break;
		}
		return cnd();
	}
};

template <typename T>
class StarNode : public KNode<T> {
public:
	StarNode(std::function<std::shared_ptr<Node<T>>()> builder) : KNode<T>(builder) {
		this->name = "StarNode";
	}
	virtual bool cnd() override {
		return true;
	}

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};

template <typename T>
class PlusNode : public KNode<T> {
public:
	PlusNode(std::function<std::shared_ptr<Node<T>>()> builder) : KNode<T>(builder) {
		this->name = "PlusNode";
	}
	virtual bool cnd() override {
		return this->nodes.size() > 0;
	}

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};

template <typename T>
class OPTNode : public KNode<T> {
public:
	OPTNode(std::function<std::shared_ptr<Node<T>>()> builder) : KNode<T>(builder) {
		this->name = "OPTNode";
	}

	virtual bool cnd() override {
		return this->nodes.size() <= 1;
	}

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};
