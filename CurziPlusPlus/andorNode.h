#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"

class AndNode : public Node {
public:
	AndNode(vNode v) {
		name = "AndNode";
		nodes = v;
	}

	virtual bool build(Grammarizer* g) override {
		auto temp = g->it;
		for (const auto& node : nodes)
			if (!node->build(g)) {
				g->it = temp;
				return false;
			}
		return true;
	}

	virtual JSON toJSON() override {
		JSON res;
		for (const auto& node : nodes) {
			if (node->name == "TokenNode") {
				std::string key = Tokenizer::tokenLookup[((TokenNode*)&(*node))->t];
				while (res.has(key))
					key += "+";
				res[Tokenizer::tokenLookup[((TokenNode*)&(*node))->t]] = node->toJSON();
			}
			else {
				while (res.has(node->name))
					node->name += "+";
				res[node->name] = node->toJSON();
			}
		}
		return res;
	}
};

class OrNode : public Node {
public:
	OrNode(vNode v) {
		name = "OrNode";
		nodes = v;
	}

	virtual bool build(Grammarizer* g) override {
		for (const auto& node : nodes)
			if (node->build(g)) {
				nodes = { node };
				return true;
			}
		return false;
	}
};
