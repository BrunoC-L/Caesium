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

// technically isnt a KNode (inheritance wise), but at heart it is
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

//////////////////////////////////////////

//////////////////////////////////////////

//////////////////////////////////////////

//////////////////////////////////////////

//////////////////////////////////////////

//////////////////////////////////////////

//////////////////////////////////////////

template <typename T, typename CND, bool requiresComma>
class KNode2 : public CND {
	std::function<T()> builder;
	std::vector<T> nodes;
	int n_indent;
public:
	KNode2(std::function<T()> builder, int n_indent = 0) : n_indent(n_indent), builder(builder) {}

	bool build(Grammarizer* g) {
		while (true) {
			auto node = this->builder();
			bool parsed = node.build(g);
			if (parsed) {
				nodes.push_back(node);
				if constexpr (requiresComma)
					parsed = TokenNode<COMMA>().build(g);
			}
			if (!parsed)
				break;
		}
		return cnd(nodes);
	}

	// to get `list of b's` from `(abc)*` for example
	template <typename T>
	std::vector<T> get() const {
		std::vector<T> res;
		for (const auto& node : nodes)
			res.push_back(node.get<T>());
		return res;
	}
};

class StarCnd {
	template <typename T>
	bool cnd(const std::vector<T>& nodes) {
		return true;
	}
};

class PlusCnd {
	template <typename T>
	bool cnd(const std::vector<T>& nodes) {
		return nodes.size();
	}
};

template <typename T>
using StarNode2 = KNode2<T, StarCnd, false>;

template <typename T>
using CommaStarNode2 = KNode2<T, StarCnd, true>;

template <typename T>
using PlusNode2 = KNode2<T, PlusCnd, false>;

template <typename T>
using CommaPlusNode2 = KNode2<T, PlusCnd, true>;

template <typename T>
class OPTNode2 {
public:
	std::optional<T> node;
	std::function<T()> builder;
	OPTNode2(std::function<T()> builder): builder(builder) {}

	bool build(Grammarizer* g) {
		auto node = this->builder();
		bool parsed = node.build(g);
		if (parsed)
			this->node = node;
		return parsed;
	}
};
