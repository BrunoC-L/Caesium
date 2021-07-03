#include <iostream>
#include <fstream>
#include "tokenizer.h"
#include "grammarizer.h"
#include "classNode.h"

void printOutFileTranspiled(const std::string& fileName) {
	std::string program;
	{
		std::ifstream t(fileName);
		std::stringstream buffer;
		buffer << t.rdbuf();
		program = buffer.str();
	}
	std::forward_list<TOKENVALUE> tokens(Tokenizer(program).read());
	Grammarizer g(tokens);

	ClassNode node;
#ifdef DEBUG
	std::cout << "\n" << fileName << ": " << (node.debugbuild(&g) ? "PASS" : "FAIL") << "\n";
#else
	std::cout << fileName << ": " << (node.build(&g) ? "PASS" : "FAIL") << "\n";
#endif // DEBUG
	std::cout << "\n" + program  + "\n";
}

int main(int argc, char* argv[]) {
	for (int i = 1; i < argc; ++i)
		printOutFileTranspiled(argv[i]);
	return 0;
}
