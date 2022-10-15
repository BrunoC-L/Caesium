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
	testParse(__LINE__, { MAKE2(ClassNode), "class A:\n" });
	testParse(__LINE__, { MAKE2(ClassNode), "class A extends B:\n" });
	testParse(__LINE__, { MAKE2(ClassNode), "class A extends B:\n\tA a\n" });
	testParse(__LINE__, { MAKE2(ClassNode), "class A extends B:\n\tA a\n\tA a\n\tA a\n\tA a\n" });
	testParse(__LINE__, { MAKE_INDENTED(ClassNode, 1), "class A extends B:\n\t\tA a\n\t\tA a\n\t\tA a\n\t\tA a\n" });
	testParse(__LINE__, { MAKE_INDENTED(ClassNode, 1), "class A extends B:\n\t\tA a\n" });
	testParse(__LINE__, { MAKE2(IfStatementNode), "if a:\n" });
	testParse(__LINE__, { MAKE2(StatementNode), "if a:\n" });
	testParse(__LINE__, { MAKE_INDENTED(StatementNode, 1), "\tif a:\n" });
	testParse(__LINE__, { MAKE2(IfStatementNode), "if a:\n\tb\n" });
	testParse(__LINE__, { _AND_
		TOKEN(IF),
			MAKE2(ExpressionNode),
			MAKE2(ColonIndentCodeBlockNode),
			_OPT_
				MAKE2(ElseStatementNode)
			___
		__, "if a:\n\tb\n" });
	testParse(__LINE__, { MAKE2(IfStatementNode), "if a:\n\tif a:\n\t\tb\n" });
	testParse(__LINE__, { MAKE_INDENTED(IfStatementNode, 1), "if a:\n\t\tb\n" });
	testParse(__LINE__, { MAKE2(ForStatementNode), "for a in b:\n\tb\n" });
	testParse(__LINE__, { MAKE2(ForStatementNode), "for a in b:\n" });
	testParse(__LINE__, { MAKE2(StatementNode), "for a in b:\n\tif a:\n\t\tb\n" });
	testParse(__LINE__, { MAKE2(StatementNode), "for i in arr:\n" });
	testParse(__LINE__, { MAKE2(TypenameNode), "E" });
	testParse(__LINE__, { MAKE2(TypenameNode), "E<E>" });
	testParse(__LINE__, { MAKE2(TypenameNode), "E::E" });
	testParse(__LINE__, { MAKE2(TypenameNode), "E::E<F>" });
	testParse(__LINE__, { MAKE2(TemplateTypenameNode), "<>" });
	testParse(__LINE__, { MAKE2(TemplateTypenameNode), "<E>" });
	testParse(__LINE__, { MAKE2(TypenameNode), "E::E<>" });
	testParse(__LINE__, { MAKE2(TypenameNode), "E<E>" });
	testParse(__LINE__, { MAKE2(TypenameNode), "E<E<E>>" });
	testParse(__LINE__, { MAKE2(TypenameNode), "E<E<E<E::E>>>" });
	testParse(__LINE__, { MAKE2(TypenameNode), "E<E<E<E::E>>,E>" });
	testParse(__LINE__, { MAKE2(TypenameNode), "E<E<E<E<E<E>>>>>" });
	testParse(__LINE__, { MAKE2(TypenameNode), "E<E<E<E < E<   E,>>,  > >,E < E < E<  E< E ,> >,>>>" });
	testParse(__LINE__, { MAKE2(TypenameNode), "E<F<H,I>>::G" });
	testParse(__LINE__, { MAKE2(MethodNode), "private static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n" });
	testParse(__LINE__, { MAKE_INDENTED(MethodNode, 1), "private static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\n\n\t\n\t\n\n\t\tfor i in arr:\n" });
	testParse(__LINE__, { MAKE2(ClassNode), "class A extends F<H, I>:\n\tprivate static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n" });
	testParse(__LINE__, { MAKE2(ClassNode), "class A extends F<H, I>:\n\tprivate static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\t\tfor i in arr:\n" });
	testParse(__LINE__, { MAKE_INDENTED(MethodNode, 1), "private static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n" });
	testParse(__LINE__, { MAKE_INDENTED(MethodNode, 1), "private static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\t\tfor i in arr:\n" });
	testParse(__LINE__, { MAKE_INDENTED(ClassElementNode, 1), "private static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\t\tfor i in arr:\n" });
	testParse(__LINE__, { MAKE_INDENTED(StatementNode, 1), "\tfor i in arr:\n" });
	testParse(__LINE__, { MAKE2(MethodNode), "void a():\n" });
	testParse(__LINE__, { MAKE_INDENTED(CodeBlockNode, 1), "\tif a:\n" });
	testParse(__LINE__, { MAKE2(WhileStatementNode), "while a:\n" });
	testParse(__LINE__, { MAKE2(ReturnStatementNode), "return a, b, \n" });
	testParse(__LINE__, { MAKE2(ReturnStatementNode), "return a, b\n" });
	testParse(__LINE__, { MAKE2(ImportNode), "import a from b" });
	testParse(__LINE__, { TOKEN(END), "\n" });
	testParse(__LINE__, { MAKE2(FileNode), "import OS from System" });
	testParse(__LINE__, { MAKE2(ClassNode), "class B:" });
	testParse(__LINE__, { MAKE2(FileNode), "class B:" });
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
	FileNode fileNode;
	std::forward_list<TOKENVALUE> tokens(Tokenizer(program).read());
	Grammarizer g(tokens);
	bool b = fileNode.build(&g);
	if (!b)
		throw std::exception("not built");
	std::cout << fileName << ": built\n";
	std::unique_ptr<NodeStructs::File> f = fileNode.getStruct();
	toCPP{}.transpile(h, cpp, f);
}

int main(int argc, char** argv) {

	AndNode2<OrNode2<ClassNode, AndNode2<ClassNode>>, ClassNode> node1;
	OrNode2<ClassNode, AndNode2<ClassNode>> x1 = node1.get<OrNode2<ClassNode, AndNode2<ClassNode>>>();
	OrNode2<ClassNode, AndNode2<ClassNode>> x2 = node1.get<0>();
	node1.build(nullptr);

	OPTNode2<ClassNode> node2([]() { return ClassNode{}; });
	node2.build(nullptr);

	StarNode2<ClassNode> node3([]() { return ClassNode{}; }, 0);
	CommaStarNode2<ClassNode> node4([]() { return ClassNode{}; }, 0);
	PlusNode2<ClassNode> node5([]() { return ClassNode{}; }, 0);
	CommaPlusNode2<ClassNode> node6([]() { return ClassNode{}; }, 0);

	using c = ClassNode;
	using ocac = OrNode2<c, AndNode2<c>>;
	using acocac = AndNode2<c, ocac>;
	StarNode2<acocac> node7([]() { return acocac{}; }, 0);
	node7.get<ocac>();

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
