#include "node.h"
#include "grammarizer.h"

template <typename T>
bool Node<T>::build(Grammarizer* g) {
	build();
	return nodes[0]->build(g);
}
