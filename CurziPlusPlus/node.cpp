#include "node.h"
#include "grammarizer.h"

bool Node::build(Grammarizer* g) {
	build();
	return nodes[0]->build(g);
}
