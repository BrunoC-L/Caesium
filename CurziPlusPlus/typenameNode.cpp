#include "typenameNode.h"
#include "emptyNode.h"

// May have missed something but damn this looks bad but it works

void TypenameNode::build() {
	nodes = {
	_AND_
		MAKE(TokenNode)(WORD),
		_OR_
			MAKE(NSTypenameNode)(),
			MAKE(PointerTypenameNode)(),
			_OR_
				_AND_
					MAKE(TemplateTypenameNode)(),
					MAKE(NSTypenameNode)(),
				})),
				MAKE(TemplateTypenameNode)(),
			})),
			MAKE(EmptyNode)()
		}))
	}))
	};
}
