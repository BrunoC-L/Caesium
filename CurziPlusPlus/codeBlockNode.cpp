#include "codeBlockNode.h"
#include "statementNode.h"

template <typename T>
void CodeBlockNode<T>::build() {

	this->nodes = {
		_OR_
			_AND_
				MAKE(TokenNode)(BRACEOPEN),
				_STAR_
					MAKE(StatementNode)()
				___,
				MAKE(TokenNode)(BRACECLOSE),
			__,
			MAKE(StatementNode)()
		__
	};
}
