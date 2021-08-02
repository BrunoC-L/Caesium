#include <iostream>
#include <fstream>
#include "tokenizer.h"
#include "grammarizer.h"
#include "fileNode.h"
#include "statementsNode.h"
#include <algorithm>
#include <filesystem>

void transpile(const std::filesystem::path& fileName, const std::filesystem::path& outFileName);
void test();

int main(int argc, char* argv[]) {
#ifdef DEBUG
	bool testing = true;
#else
	bool testing = false;
#endif // DEBUG
	testing = false;

	if (testing)
		test();
	else
		for (int i = 1; i < argc; ++i)
			for (const auto& file : std::filesystem::directory_iterator(argv[i])) {
				const std::filesystem::path& fileName = file.path();
				if (fileName.extension() == ".curzi")
					transpile(fileName, std::string(argv[i]) + "\\" + std::string(reinterpret_cast<const char*>(file.path().stem().c_str())) + ".json");
			}
	return 0;
}

void test(std::string program, std::shared_ptr<Node> expected) {
	std::forward_list<TOKENVALUE> tokens(Tokenizer(program).read());
	Grammarizer g(tokens);
	bool nodeBuilt = expected->build(&g);
	auto temp = g.it;
	bool programReadEntirely = (++temp) == g.tokens.end();

	std::cout << "\n" << ((nodeBuilt && programReadEntirely) ? "PASS" : "FAIL") << "\n\n";
	for (auto it = g.tokens.begin(); it != g.it; ++it)
		std::cout << it->second;
	std::cout << "\n" << program + "\n\n";
}

void test() {
	std::vector<std::pair<std::string, std::shared_ptr<Node>>> tests = {
		{
			"((a.a.a()).a(a.a.a() * a.a()()(), a()(), a.a().a.a)().a * a())() * a.a * a;",
			MAKE(StatementNode)(),
		},
	};
	for (auto& t : tests)
		test(t.first, t.second);
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
	JSON json = expected->toJSON();
	std::ofstream output(outFileName);
	output << json.asString(" ", false);
	bool programReadEntirely = g.it == g.tokens.end();

	std::cout << fileName << ": " << ((nodeBuilt && programReadEntirely) ? "PASS" : "FAIL") << "\n\n";
	for (auto it = g.tokens.begin(); it != g.it; ++it)
		std::cout << it->second;
	std::cout << "\n\n" << program + "\n";
}
