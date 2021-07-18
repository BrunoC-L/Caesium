#include "typenameNode.h"
#include "emptyNode.h"
#include "kNode.h"

// even including the file where these were defined the IDE still gave an error...
#define _OPT_ MAKE(OPTNode)([&](){ return
#define ___ ;})

void TypenameNode::build() {
	nodes = {
	_AND_
		MAKE(TokenNode)(WORD),
		_OR_
			MAKE(NSTypenameNode)(),
			MAKE(PointerTypenameNode)(),
			_AND_
				MAKE(TemplateTypenameNode)(),
				_OPT_
					MAKE(NSTypenameNode)()
				___
			__,
			MAKE(EmptyNode)()
		__
	__
	};
}
