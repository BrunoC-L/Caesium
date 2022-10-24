#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include "grammar.h"

auto green = "\033[1;32m", red = "\033[1;31m", reset = "\033[0m";

template <typename T>
void testParse(int i, int n_indent, std::string program, bool expectedToBuild = true) {
	std::forward_list<TOKENVALUE> tokens(Tokenizer(program).read());
	Grammarizer g(tokens);

	bool nodeBuilt = And<T>(n_indent).build(&g);
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
		return;
	}
}

void testParse() {
	testParse<CommaStar<And<Token<WORD>>>>(__LINE__, 0, "y");
	testParse<Import>(__LINE__, 0, "import string from std\n");
	testParse<And<IndentToken, Token<WORD>>>(__LINE__, 3, "\t\t\ta");
	testParse<CommaStar<Token<WORD>>>(__LINE__, 0, "y");
	testParse<CommaStar<And<Token<WORD>>>>(__LINE__, 0, "y");
	testParse<OPT<Import>>(__LINE__, 0, "\n");
	testParse<Class>(__LINE__, 0, "class A:\n");
	testParse<Class>(__LINE__, 0, "class A extends B:\n");
	testParse<Class>(__LINE__, 0, "class A extends B:\n\tA a\n");
	testParse<Class>(__LINE__, 0, "class A extends B:\n\tA a\n\tA a\n\tA a\n\tA a\n");
	testParse<Class>(__LINE__, 1, "class A extends B:\n\tA a\n\tA a\n\tA a\n\tA a\n", false);
	testParse<Class>(__LINE__, 1, "class A extends B:\n\t\tA a\n\t\tA a\n\t\tA a\n\t\tA a\n");
	testParse<Class>(__LINE__, 0, "class A extends B:\n\t\tA a\n\t\tA a\n\t\tA a\n\t\tA a\n", false);
	testParse<Class>(__LINE__, 1, "class A extends B:\n\t\tA a\n");
	testParse<IfStatement>(__LINE__, 0, "if a:\n");
	testParse<Statement>(__LINE__, 0, "if a:\n");
	testParse<Statement>(__LINE__, 1, "\tif a:\n");
	testParse<IfStatement>(__LINE__, 0, "if a:\n\tb\n");
	testParse<IfStatement>(__LINE__, 0, "if a:\n\tif a:\n\t\tb\n");
	testParse<IfStatement>(__LINE__, 1, "if a:\n\t\tb\n");
	/*testParse(__LINE__, { _AND_
		TOKEN(IF),
			MAKE2(ExpressionNode),
			MAKE2(ColonIndentCodeBlockNode),
			_OPT_
				MAKE2(ElseStatementNode)
			___
		__, "if a:\n\tb\n" });*/
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
	testParse<Method>(__LINE__, 0, "private static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n");
	testParse<Method>(__LINE__, 1, "private static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\n\n\t\n\t\n\n\t\tfor i in arr:\n");
	testParse<Class>(__LINE__, 0, "class A extends F<H, I>:\n\tprivate static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n");
	testParse<Class>(__LINE__, 0, "class A extends F<H, I>:\n\tprivate static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\t\tfor i in arr:\n");
	testParse<Method>(__LINE__, 1, "private static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n");
	testParse<Method>(__LINE__, 1, "private static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\t\tfor i in arr:\n");
	testParse<ClassElement>(__LINE__, 1, "private static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\t\tfor i in arr:\n");
	testParse<Statement>(__LINE__, 1, "\tfor i in arr:\n");
	testParse<Method>(__LINE__, 0, "void a():\n");
	testParse<CodeBlock>(__LINE__, 1, "\tif a:\n");
	testParse<WhileStatement>(__LINE__, 0, "while a:\n");
	testParse<ReturnStatement>(__LINE__, 0, "return a, b, \n");
	testParse<ReturnStatement>(__LINE__, 0, "return a, b\n");
	testParse<Import>(__LINE__, 0, "import a from b");
	testParse<Token<END>>(__LINE__, 0, "\n");
	testParse<File>(__LINE__, 0, "import OS from System");
	testParse<Class>(__LINE__, 0, "class B:");
	testParse<File>(__LINE__, 0, "class B:");
}

//void transpile(const std::filesystem::path& fileName, std::string folder) {
//	const std::string outFileNameNoExt = folder + std::string(reinterpret_cast<const char*>(fileName.stem().c_str())) + +"/out";
//	std::ifstream caesium(fileName);
//	std::ofstream h(outFileNameNoExt + ".h", std::ios::trunc);
//	std::ofstream cpp(outFileNameNoExt + ".cpp", std::ios::trunc);
//	if (!caesium.is_open())
//		throw std::exception();
//	if (!h.is_open())
//		throw std::exception();
//	if (!cpp.is_open())
//		throw std::exception();
//	std::string program;
//	std::getline(caesium, program, '\0');
//	FileNode fileNode;
//	std::forward_list<TOKENVALUE> tokens(Tokenizer(program).read());
//	Grammarizer g(tokens);
//	bool b = fileNode.build(&g);
//	if (!b)
//		throw std::exception("not built");
//	std::cout << fileName << ": built\n";
//	std::unique_ptr<NodeStructs::File> f = fileNode.getStruct();
//	toCPP{}.transpile(h, cpp, f);
//}

int main(int argc, char** argv) {

	//And<Or<ClassNode, And<ClassNode>>, ClassNode> node1(0);
	////Or<ClassNode, And<ClassNode>> x1 = node1.get<Or<ClassNode, And<ClassNode>>>();
	////Or<ClassNode, And<ClassNode>> x2 = node1.get<0>();
	//node1.build(nullptr);
	//OPT<ClassNode> node2(0);
	//node2.build(nullptr);

	//Star<ClassNode> node3(0);
	//CommaStar<ClassNode> node4(0);
	//Plus<ClassNode> node5(0);
	//CommaPlus<ClassNode> node6(0);

	//using c = ClassNode;
	//using ocac = Or<c, And<c>>;
	//using acocac = And<c, ocac>;
	//Star<acocac> node7(0);
	////node7.get<ocac>();

	//Indent<ClassNode> node8(0);

	std::cout << std::boolalpha;

	testParse();
	/*std::cout << "\n\n";
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
		}*/
	return 0;
}
