#pragma once
#include "node.h"
#include "grammarizer.h"
#include "andorNode.h"
#include "tokenNode.h"
#include "emptyNode.h"

class TypenameNode : public Node {
public:
	baseCtor(TypenameNode);
	virtual bool build(Grammarizer* g) override;
};

class NSTypenameNode : public Node {
public:
	baseCtor(NSTypenameNode);
	virtual bool build(Grammarizer* g) override {
		return
		_AND_(vNode({
			MAKE(TokenNode)(NS),
			MAKE(TypenameNode)()
		}))
#ifdef DEBUG
		->debugbuild(g);
#else
		->build(g);
#endif // DEBUG
	}
};

class TemplateListNode : public Node {
public:
	baseCtor(TemplateListNode);
	virtual bool build(Grammarizer* g) override {
		return
		_OR_(vNode({
			_AND_(vNode({
				_AND_(vNode({
					MAKE(TypenameNode)(),
					MAKE(TokenNode)(COMMA),
					MAKE(TemplateListNode)(),
				})),
			})),
			MAKE(TypenameNode)(),
		}))
#ifdef DEBUG
		->debugbuild(g);
#else
		->build(g);
#endif // DEBUG
	}
};

class TemplateTypenameNode : public Node {
public:
	baseCtor(TemplateTypenameNode);
	virtual bool build(Grammarizer* g) override {
		return
		_AND_(vNode({
			MAKE(TokenNode)(LT),
			MAKE(TemplateListNode)(),
			MAKE(TokenNode)(GT),
		}))
#ifdef DEBUG
		->debugbuild(g);
#else
		->build(g);
#endif // DEBUG
	}
};
