#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "emptyNode.h"

class ArgumentsNode : public Node {
public:
	baseCtor(ArgumentsNode);

	virtual bool build(Grammarizer* g) override {
		return
		_AND_(vNode({
			MAKE(TokenNode)(PARENOPEN),
			_OR_(vNode({
				_AND_(vNode({
					MAKE(TypenameNode)(),
					MAKE(TokenNode)(WORD),
					_OR_(vNode({
						_AND_(vNode({
							MAKE(TokenNode)(COMMA),
							MAKE(ArgumentsNode)()
						}))
					})),
				})),
				MAKE(EmptyNode)()
			})),
			MAKE(TokenNode)(PARENCLOSE),
		}))
#ifdef DEBUG
		->debugbuild(g);
#else
		->build(g);
#endif // DEBUG
	}
};
