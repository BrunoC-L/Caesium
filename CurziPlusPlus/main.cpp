#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>

#include "fileNode.h"
#include "toCPP.h"

auto green = "\033[1;32m", red = "\033[1;31m", reset = "\033[0m";
std::string testfolder = "";

void testParse(int i, std::tuple<std::shared_ptr<Node>, std::string>&& testCase) {
	std::string& program = get<std::string>(testCase);
	std::shared_ptr<Node>& expected = get<std::shared_ptr<Node>>(testCase);
	std::forward_list<TOKENVALUE> tokens(Tokenizer(program).read());
	Grammarizer g(tokens);
	bool nodeBuilt = expected->build(&g);
	bool programReadEntirely = g.it == g.tokens.end();
	while (!programReadEntirely && (g.it->first == NEWLINE || g.it->first == END))
		programReadEntirely = ++g.it == g.tokens.end();
	auto nodeBuiltColor = nodeBuilt ? green : red;
	auto programReadEntirelyColor = programReadEntirely ? green : red;
	std::cout << "LINE " << i << (i < 10 ? " : " : ": ")
		<< "built: " << nodeBuiltColor << nodeBuilt << reset
		<< ", entirely: " << programReadEntirelyColor << programReadEntirely << reset << "\n";
	if (!nodeBuilt || !programReadEntirely) {
		std::cout << program << "\n\n";
		Grammarizer g2(tokens);
		while (g2.it != g.it) {
			std::cout << g2.it->second << " ";
			++g2.it;
		}
		return;
	}
}

void ManualTest() {
	std::string program = "E<E>";
	TypenameNode expected("", 0);
	std::forward_list<TOKENVALUE> tokens(Tokenizer(program).read());
	Grammarizer g(tokens);
	bool nodeBuilt = ((Node*) &expected)->build(&g); // VS2022 :)
	bool programReadEntirely = g.it == g.tokens.end();
	while (!programReadEntirely && (g.it->first == NEWLINE || g.it->first == END))
		programReadEntirely = ++g.it == g.tokens.end();
	auto nodeBuiltColor = nodeBuilt ? green : red;
	auto programReadEntirelyColor = programReadEntirely ? green : red;
	std::cout << "LINE " << __LINE__ << (__LINE__ < 10 ? " : " : ": ")
		<< "built: " << nodeBuiltColor << nodeBuilt << reset
		<< ", entirely: " << programReadEntirelyColor << programReadEntirely << reset << "\n";
	if (!nodeBuilt || !programReadEntirely) {
		std::cout << program << "\n\n";
		Grammarizer g2(tokens);
		while (g2.it != g.it) {
			std::cout << g2.it->second << " ";
			++g2.it;
		}
		return;
	}
	else {
		auto T = expected.getStruct();
		return;
	}
}

void testParse() {
	int n_indent = 0; // for macros
	for (const auto& entry : std::filesystem::directory_iterator(testfolder))
		std::filesystem::remove_all(entry.path());
	testParse(__LINE__, { MAKE(ClassNode)(""), "class A:\n" });
	testParse(__LINE__, { MAKE(ClassNode)(""), "class A extends B:\n" });
	testParse(__LINE__, { MAKE(ClassNode)(""), "class A extends B:\n\tA a\n" });
	testParse(__LINE__, { MAKE(ClassNode)(""), "class A extends B:\n\tA a\n\tA a\n\tA a\n\tA a\n" });
	testParse(__LINE__, { MAKE(ClassNode)("", 1), "class A extends B:\n\t\tA a\n\t\tA a\n\t\tA a\n\t\tA a\n" });
	testParse(__LINE__, { MAKE(ClassNode)("", 1), "class A extends B:\n\t\tA a\n" });
	testParse(__LINE__, { MAKE(IfStatementNode)(""), "if a:\n" });
	testParse(__LINE__, { MAKE(StatementNode)(""), "if a:\n" });
	testParse(__LINE__, { MAKE(IfStatementNode)(""), "if a:\n\tb\n" });
	testParse(__LINE__, { MAKE(IfStatementNode)(""), "if a:\n\tif a:\n\t\tb\n" });
	testParse(__LINE__, { MAKE(IfStatementNode)("", 1), "if a:\n\t\tb\n" });
	testParse(__LINE__, { MAKE(ForStatementNode)(""), "for a in b:\n\tb\n" });
	testParse(__LINE__, { MAKE(ForStatementNode)(""), "for a in b:\n" });
	testParse(__LINE__, { MAKE(StatementNode)(""), "for a in b:\n\tif a:\n\t\tb\n" });
	testParse(__LINE__, { MAKE(StatementNode)(""), "for i in arr:\n" });
	testParse(__LINE__, { MAKE(TypenameNode)(""), "E" });
	testParse(__LINE__, { MAKE(TypenameNode)(""), "E*" });
	testParse(__LINE__, { MAKE(TypenameNode)(""), "E<E**>**" });
	testParse(__LINE__, { MAKE(TypenameNode)(""), "E::E" });
	testParse(__LINE__, { MAKE(TypenameNode)(""), "E::E<F>" });
	testParse(__LINE__, { MAKE(TemplateTypenameNode)(""), "<>" });
	testParse(__LINE__, { MAKE(TemplateTypenameNode)(""), "<E>" });
	testParse(__LINE__, { MAKE(TypenameNode)(""), "E::E<>***&" });
	testParse(__LINE__, { MAKE(TypenameNode)(""), "E<E>" });
	testParse(__LINE__, { MAKE(TypenameNode)(""), "E<E<E>>" });
	testParse(__LINE__, { MAKE(TypenameNode)(""), "E<E<E<E::E>>>" });
	testParse(__LINE__, { MAKE(TypenameNode)(""), "E<E<E<E::E>>,E>" });
	testParse(__LINE__, { MAKE(TypenameNode)(""), "E<E<E<E<E<E>>>>>" });
	testParse(__LINE__, { MAKE(TypenameNode)(""), "E<E<E<E < E<   E,>>,  > >,E < E < E<  E< E ,> >,>>>" });
	testParse(__LINE__, { MAKE(TypenameNode)(""), "E<F<H,I>>::G" });
	testParse(__LINE__, { MAKE(MethodNode)(""), "private static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n" });
	testParse(__LINE__, { MAKE(MethodNode)("", 1), "private static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\n\n\t\n\t\n\n\t\tfor i in arr:\n" });
	testParse(__LINE__, { MAKE(ClassNode)(""), "class A extends F<H, I>:\n\tprivate static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n" });
	testParse(__LINE__, { MAKE(ClassNode)(""), "class A extends F<H, I>:\n\tprivate static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\t\tfor i in arr:\n" });
	testParse(__LINE__, { MAKE(MethodNode)("", 1), "private static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n" });
	testParse(__LINE__, { MAKE(MethodNode)("", 1), "private static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\t\tfor i in arr:\n" });
	testParse(__LINE__, { MAKE(ClassElementNode)("", 1), "private static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\t\tfor i in arr:\n" });
	testParse(__LINE__, { MAKE(StatementNode)("", 1), "\tfor i in arr:\n" });
	testParse(__LINE__, { MAKE(MethodNode)(""), "void a():\n" });
	testParse(__LINE__, { MAKE(CodeBlockNode)("", 1), "\tif a:\n" });
	testParse(__LINE__, { MAKE(WhileStatementNode)(""), "while a:\n" });
	testParse(__LINE__, { MAKE(ReturnStatementNode)(""), "return a, b, \n" });
	testParse(__LINE__, { MAKE(ReturnStatementNode)(""), "return a, b\n" });
	
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
	std::string filecontent;
	std::getline(caesium, filecontent, '\0');
	FileNode fileNode("", 0);
	std::forward_list<TOKENVALUE> tokens(Tokenizer(filecontent).read());
	Grammarizer g(tokens);
	((Node*) &fileNode)->build(&g); // VS2022 :)
	std::unique_ptr<NodeStructs::File> f = fileNode.getStruct();
	toCPP{}.transpile(h, cpp, f);
}

int main(int argc, char** argv) {
	auto folder = std::string(argv[1]); // point to repo "tests" folder path
	if (folder.ends_with("/") || folder.ends_with("\\"))
		folder = folder.substr(0, folder.length() - 1);
	testfolder = folder + "/out/tests";
	std::cout << std::boolalpha;

	//ManualTest();

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
