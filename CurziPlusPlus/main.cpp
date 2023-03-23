#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include "grammar.h"
#include "toCpp.h"
#include "structurizer.h"

auto green = "\033[1;32m", red = "\033[1;31m", reset = "\033[0m";

template <typename... Ts>
void testParse(int i, int n_indent, std::string program, bool expectedToBuild = true) {
	std::forward_list<TOKENVALUE> tokens(Tokenizer(program).read());
	Grammarizer g(tokens);

	bool nodeBuilt = false;
	if (expectedToBuild)
		nodeBuilt = And<Ts...>(n_indent).build(&g);
	else
		// if it is expected to fail, try to parse END to make sure it has to get the entire input
		// otherwise they might succeed by skipping tokens at the end with Star or Opts
		nodeBuilt = And<Ts..., Token<END>>(n_indent).build(&g);

	bool programReadEntirely = g.it == g.tokens.end();
	while (!programReadEntirely && (g.it->first == NEWLINE || g.it->first == END))
		programReadEntirely = ++g.it == g.tokens.end();
	auto nodeBuiltColor = nodeBuilt == expectedToBuild ? green : red;
	auto programReadEntirelyColor = programReadEntirely == expectedToBuild ? green : red;
	std::cout << "LINE " << i << (i < 10 ? " : " : ": ")
		<< "built: " << nodeBuiltColor << nodeBuilt << reset
		<< ", entirely: " << programReadEntirelyColor << programReadEntirely << reset << "\n";
	bool res = nodeBuilt && programReadEntirely;
	if (res != expectedToBuild) {
		std::cout << program << "\n\n";
		Grammarizer g2(tokens);
		while (g2.it != g.it) {
			std::cout << g2.it->second << " ";
			++g2.it;
		}
		std::cout << "\n";
		return;
	}
}

void testParse() {
	testParse<CommaStar<And<Token<WORD>>>>(__LINE__, 0, "y");
	testParse<Import>(__LINE__, 0, "import string\n");
	testParse<And<IndentToken, Token<WORD>>>(__LINE__, 3, "\t\t\ta");
	testParse<CommaStar<Token<WORD>>>(__LINE__, 0, "y");
	testParse<CommaStar<And<Token<WORD>>>>(__LINE__, 0, "y");
	testParse<Opt<Import>>(__LINE__, 0, "\n");
	testParse<Class>(__LINE__, 0, "class A:\n");
	testParse<Class>(__LINE__, 0, "class A extends B:\n");
	testParse<Class>(__LINE__, 0, "class A extends B:\n\tA a\n");
	testParse<Class>(__LINE__, 0, "class A extends B:\n\tA a\n\tA a\n\tA a\n\tA a\n");
	testParse<Class>(__LINE__, 1, "class A extends B:\n\t\tA a\n\t\tA a\n\t\tA a\n\t\tA a\n");
	testParse<Class>(__LINE__, 1, "class A extends B:\n\t\tA a\n");
	testParse<IfStatement>(__LINE__, 0, "if a:\n");
	testParse<Statement>(__LINE__, 0, "if a:\n");
	testParse<Statement>(__LINE__, 1, "\tif a:\n");
	testParse<IfStatement>(__LINE__, 0, "if a:\n\tb\n");
	testParse<IfStatement>(__LINE__, 0, "if a:\n\tif a:\n\t\tb\n");
	testParse<IfStatement>(__LINE__, 1, "if a:\n\t\tb\n");
	testParse<ForStatement>(__LINE__, 0, "for a in b:\n\tb\n");
	testParse<ForStatement>(__LINE__, 0, "for a in b:\n");
	testParse<Statement>(__LINE__, 0, "for a in b:\n\tif a:\n\t\tb\n");
	testParse<Statement>(__LINE__, 0, "for i in arr:\n");
	testParse<Typename>(__LINE__, 0, "E");
	testParse<Typename>(__LINE__, 0, "E<E>");
	testParse<Typename>(__LINE__, 0, "E::E");
	testParse<Typename>(__LINE__, 0, "E::E<F>");
	testParse<TemplateTypename>(__LINE__, 0, "<>");
	testParse<TemplateTypename>(__LINE__, 0, "<E>");
	testParse<Typename>(__LINE__, 0, "E::E<>");
	testParse<Typename>(__LINE__, 0, "E<E>");
	testParse<Typename>(__LINE__, 0, "E<E<E>>");
	testParse<Typename>(__LINE__, 0, "E<E<E<E::E>>>");
	testParse<Typename>(__LINE__, 0, "E<E<E<E::E>>,E>");
	testParse<Typename>(__LINE__, 0, "E<E<E<E<E<E>>>>>");
	testParse<Typename>(__LINE__, 0, "E<E<E<E < E<   E,>>,  > >,E < E < E<  E< E ,> >,>>>");
	testParse<Typename>(__LINE__, 0, "E<F<H,I>>::G");
	testParse<Function>(__LINE__, 0, "E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n");
	testParse<Function>(__LINE__, 1, "E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\n\n\t\n\t\n\n\t\tfor i in arr:\n");
	testParse<Class>(__LINE__, 0, "class A extends F<H, I>:\n\tE<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n");
	testParse<Function>(__LINE__, 0, "E<F<H,I>>::G method1():\n");
	testParse<Class>(__LINE__, 0, "class A extends F<H, I>:\n\tE<F<H,I>>::G method1():\n");
	testParse<Class>(__LINE__, 0, "class A extends F<H, I>:\n\tE<F<H,I>>::G member1");
	testParse<Class>(__LINE__, 0, "class A extends F<H, I>:\n\tE<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\t\tfor i in arr:\n");
	testParse<Function>(__LINE__, 1, "E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n");
	testParse<Function>(__LINE__, 1, "E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\t\tfor i in arr:\n");
	testParse<ClassElement>(__LINE__, 1, "E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\t\tfor i in arr:\n");
	testParse<ClassElement>(__LINE__, 1, "E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n");
	testParse<Statement>(__LINE__, 1, "\tfor i in arr:\n");
	testParse<Function>(__LINE__, 0, "void a():\n");
	testParse<Star<Statement>>(__LINE__, 1, "\tif a:\n");
	testParse<WhileStatement>(__LINE__, 0, "while a:\n");
	testParse<ReturnStatement>(__LINE__, 0, "return a, b, \n");
	testParse<ReturnStatement>(__LINE__, 0, "return a, b\n");
	testParse<Import>(__LINE__, 0, "import a");
	testParse<Token<END>>(__LINE__, 0, "\n");
	testParse<File>(__LINE__, 0, "import OS");
	testParse<Class>(__LINE__, 0, "class B:");
	testParse<File>(__LINE__, 0, "class B:");
	testParse<IfStatement>(__LINE__, 0, "if a:\n\tb\nelse:\n\tc\n");
	testParse<Indent<Indent<Indent<IfStatement>>>>(__LINE__, 0, "if a:\n\t\t\t\tb\n\t\t\telse:\n\t\t\t\tc\n");
	testParse<Star<Statement>>(__LINE__, 0, "Set<int> someContainer\n");
	testParse<Star<Statement>>(__LINE__, 0, "Set<int> someContainer\nSet<int> someContainer\n");
	testParse<Star<Statement>>(__LINE__, 0, "Set<int> someContainer\nfor i in someContainer:\n");
	testParse<Star<Statement>>(__LINE__, 0, "Set<int> someContainer\nfor i in someContainer:\nvector<int> arr\nfor i in arr :\nMap<int, std::string> m\nfor k, v in m:\n");

	std::cout << "=====================\nREVERSING LOGIC OF TESTS\nRED TRUE FOR `BUILT` IS OK IF `ENTIRELY` IS GREEN FALSE\n=====================\n";
	// basically previous tests are against false negatives (good code should work)
	// and these tests are against false positives (bad code should fail)
	testParse<Class>(__LINE__, 1, "class A extends B:\n\tA a\n\tA a\n\tA a\n\tA a\n", false);
	testParse<Class>(__LINE__, 0, "class A extends B:\n\t\tA a\n\t\tA a\n\t\tA a\n\t\tA a\n", false);
}

void transpile(const std::filesystem::path& fileName, std::string folder) {
	const std::string outFileNameNoExt = folder + std::string(reinterpret_cast<const char*>(fileName.stem().c_str())) + +"/out";
	std::ifstream caesium(fileName);
	std::ofstream h(outFileNameNoExt + ".h", std::ios::trunc);
	std::ofstream cpp(outFileNameNoExt + ".cpp", std::ios::trunc);
	if (!caesium.is_open())
		throw std::exception();
	if (!h.is_open())
		throw std::exception();
	if (!cpp.is_open())
		throw std::exception();
	std::string program;
	std::getline(caesium, program, '\0');
	File file(0);
	std::forward_list<TOKENVALUE> tokens(Tokenizer(program).read());
	Grammarizer g(tokens);
	if (build_optional_primitive(file, &g)) {
		std::cout << fileName << ": built\n";
		const NodeStructs::File& f = getStruct(file);
		toCPP{}.transpile(h, cpp, f);
	}
	else
		throw std::exception("not built");
}

int main(int argc, char** argv) {

	std::cout << std::boolalpha;
	testParse();
	std::cout << "\n\n";
	for (int i = 1; i < argc; ++i)
		for (const auto& file : std::filesystem::directory_iterator(argv[i])) {
			const std::filesystem::path& fileName = file.path();
			if (fileName.extension() == ".curzi")
				transpile(fileName, std::string(argv[i]) + "\\out\\");
		}
	return 0;
}
