#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>

#include "fileNode.h"
#include "toCPP.h"

auto green = "\033[1;32m", red = "\033[1;31m", reset = "\033[0m";

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

void testParse() {
	int n_indent = 0; // for macros using scope n_indent
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
	testParse(__LINE__, { MAKE(TypenameNode)(""), "E<E>" });
	testParse(__LINE__, { MAKE(TypenameNode)(""), "E::E" });
	testParse(__LINE__, { MAKE(TypenameNode)(""), "E::E<F>" });
	testParse(__LINE__, { MAKE(TemplateTypenameNode)(""), "<>" });
	testParse(__LINE__, { MAKE(TemplateTypenameNode)(""), "<E>" });
	testParse(__LINE__, { MAKE(TypenameNode)(""), "E::E<>" });
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
	testParse(__LINE__, { MAKE(ImportNode)(""), "import a from b" });
	testParse(__LINE__, { TOKEN(END), "\n" });
	testParse(__LINE__, { MAKE(FileNode)(""), "import OS from System" });
	testParse(__LINE__, { MAKE(ClassNode)(""), "class B:" });
	testParse(__LINE__, { MAKE(FileNode)(""), "class B:" });
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
	FileNode fileNode("", 0);
	std::forward_list<TOKENVALUE> tokens(Tokenizer(program).read());
	Grammarizer g(tokens);
	bool b = ((Node*) &fileNode)->build(&g); // VS2022 :)
	if (!b)
		throw std::exception("not built");
	std::cout << fileName << ": built\n";
	std::unique_ptr<NodeStructs::File> f = fileNode.getStruct();
	toCPP{}.transpile(h, cpp, f);
}

int main(int argc, char** argv) {
	std::cout << std::boolalpha;

	testParse();
	std::cout << "\n\n";
	for (int i = 1; i < argc; ++i)
		for (const auto& file : std::filesystem::directory_iterator(argv[i])) {
			const std::filesystem::path& fileName = file.path();
			if (fileName.extension() == ".curzi")
				try {
					transpile(fileName, std::string(argv[i]) + "\\out\\");
				}
				catch (const std::exception& e) {
					std::cerr << fileName << ": " << e.what() << "\n";
				}
		}
	return 0;
}
