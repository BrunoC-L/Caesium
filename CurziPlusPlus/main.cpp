#include <iostream>
#include <fstream>
#include "tokenizer.h"
#include "grammarizer.h"
#include "fileNode.h"
#include <algorithm>
#include <filesystem>
#include "statementNode.h"
#include "JSONNodeVisitor.h"

void transpile(const std::filesystem::path& fileName, const std::filesystem::path& outFileName);

int main(int argc, char* argv[]) {
	for (int i = 1; i < argc; ++i)
		for (const auto& file : std::filesystem::directory_iterator(argv[i])) {
			const std::filesystem::path& fileName = file.path();
			if (fileName.extension() == ".curzi")
				transpile(fileName, std::string(argv[i]) + "\\" + std::string(reinterpret_cast<const char*>(file.path().stem().c_str())) + ".json");
		}
	return 0;
}

void transpile(const std::filesystem::path& fileName, const std::filesystem::path& outFileName) {
	std::string program;
	{
		std::ifstream input(fileName);
		std::stringstream buffer;
		buffer << input.rdbuf();
		program = buffer.str();
	}
	std::forward_list<TOKENVALUE> tokens(Tokenizer(program).read());
	Grammarizer g(tokens);
	Node* expected = new FileNode();
	bool nodeBuilt = expected->build(&g);
	bool programReadEntirely = g.it == g.tokens.end();

	if (nodeBuilt && programReadEntirely) {
		JSONNodeVisitor* v = new JSONNodeVisitor();
		expected->accept(v);
		JSON out = v->getValue();
		std::ofstream output(outFileName);
		output << out.asString(" ", false) << std::endl;
	}

	std::cout << fileName << ": " << ((nodeBuilt && programReadEntirely) ? "PASS" : "FAIL") << "\n";
}
