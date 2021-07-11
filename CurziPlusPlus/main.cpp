#include <iostream>
#include <fstream>
#include "tokenizer.h"
#include "grammarizer.h"
#include "classNode.h"
#include "statementsNode.h"
#include <algorithm>

void transpile(const std::string& fileName);
void test();

int main(int argc, char* argv[]) {
#ifdef DEBUG
	bool testing = true;
#else
	bool testing = false;
#endif // DEBUG
	//testing = false;

	if (testing)
		test();
	else
		for (int i = 1; i < argc; ++i)
			transpile(argv[i]);
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
			"a.a.a().a(a.a.a() * a.a()()(), a()(), a.a().a.a)().a * a()() * a.a * a;",
			MAKE(StatementNode)(),
		},
	};
	for (auto& t : tests)
		test(t.first, t.second);
}

void transpile(const std::string& fileName) {
	if (fileName.substr(std::max(unsigned(0), fileName.length() - 6)) != ".curzi")
		throw std::exception("bad filename, .curzi please");
	std::string program;
	{
		std::ifstream t(fileName);
		std::stringstream buffer;
		buffer << t.rdbuf();
		program = buffer.str();
	}
	std::forward_list<TOKENVALUE> tokens(Tokenizer(program).read());
	Grammarizer g(tokens);
	ClassNode expected;
	bool nodeBuilt = expected.build(&g);
	auto temp = g.it;
	bool programReadEntirely = (++temp) == g.tokens.end();

	std::cout << fileName << ": " << ((nodeBuilt && programReadEntirely) ? "PASS" : "FAIL") << "\n\n";
	for (auto it = g.tokens.begin(); it != g.it; ++it)
		std::cout << it->second;
	std::cout << "\n\n" << program + "\n";
}
