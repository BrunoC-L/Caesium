#include "typenameNode.h"
#include "emptyNode.h"

// May have missed something but damn this looks bad but it works

bool TypenameNode::build(Grammarizer* g) {
	return
	_AND_(vNode({
		MAKE(TokenNode)(WORD),
		_OR_(vNode({
			MAKE(NSTypenameNode)(),
			_OR_(vNode({
				_AND_(vNode({
					MAKE(TemplateTypenameNode)(),
					MAKE(NSTypenameNode)(),
				})),
				MAKE(TemplateTypenameNode)(),
			})),
			MAKE(EmptyNode)()
		}))
	}))
#ifdef DEBUG
	->debugbuild(g);
#else
	->build(g);
#endif // DEBUG
}
