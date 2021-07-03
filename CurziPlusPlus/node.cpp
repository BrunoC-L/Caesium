#include "node.h"
#include "grammarizer.h"

bool Node::debugbuild(Grammarizer* g) {
	_ASSERT(name != "Node");
	const auto& indent = g->getIndent();
	std::cout << indent << name << "\n";
	g->inc();
	bool worked = build(g);
	g->dec();
	std::cout << indent << name << (worked ? " worked" : " failed") << "\n";
	return worked;
}
