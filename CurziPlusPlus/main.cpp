#include <iostream>
#include <fstream>
#include "tokenizer.h"
#include "grammarizer.h"
#include "fileNode.h"
#include <algorithm>
#include <filesystem>
#include "statementNode.h"
#include "JSONNodeVisitor.h"

int i = 1;
auto green = "\033[1;32m", red = "\033[1;31m", reset = "\033[0m";

void test(std::tuple<std::shared_ptr<Node>, std::string>&& testCase) {
	std::string program = get<std::string>(testCase);
	std::shared_ptr<Node> expected = get<std::shared_ptr<Node>>(testCase);
	std::forward_list<TOKENVALUE> tokens(Tokenizer(program).read());
	Grammarizer g(tokens);
	bool nodeBuilt = expected->build(&g);
	bool programReadEntirely = g.it == g.tokens.end() || (++g.it) ==  g.tokens.end(); // need to also parse END token for most rules during tests (only FileNode expects END)
	auto nodeBuiltColor = nodeBuilt ? green : red;
	auto programReadEntirelyColor = programReadEntirely ? green : red;
	std::cout << i << (i++ < 10 ? " : " : ": ")
		<< "built: " << nodeBuiltColor << nodeBuilt << reset
		<< ", entirely: " << programReadEntirelyColor << programReadEntirely << reset << "\n";
	if (!nodeBuilt || !programReadEntirely)
		std::cout << program << "\n\n";
	if (nodeBuilt) {
		auto outFileName = "C:/Users/Bruno/Desktop/Curzi/out/test/" + std::to_string(i) + ".json";
		JSONNodeVisitor v;
		expected->accept(&v);
		JSON out = v.getValue();
		std::ofstream output(outFileName);
		auto b = output.is_open();
		output << out.asString("  ", false) << std::endl;
		output.close();
	}
}

void test() {
	test({ MAKE(ClassNode)(), "class A:\n" });
	test({ MAKE(ClassNode)(), "class A extends B:\n" });
	test({ MAKE(ClassNode)(), "class A extends B:\n\tA a\n" });
	test({ MAKE(ClassNode)(), "class A extends B:\n\tA a\n\tA a\n\tA a\n\tA a\n" });
	test({ MAKE(ClassNode)(1), "class A extends B:\n\t\tA a\n\t\tA a\n\t\tA a\n\t\tA a\n" });
	test({ MAKE(ClassNode)(1), "class A extends B:\n\t\tA a\n" });
	test({ MAKE(IfStatementNode)(), "if a:\n" });
	test({ MAKE(StatementNode)(), "if a:\n" });
	test({ MAKE(IfStatementNode)(), "if a:\n\tb\n" });
	test({ MAKE(IfStatementNode)(), "if a:\n\tif a:\n\t\tb\n" });
	test({ MAKE(IfStatementNode)(1), "if a:\n\t\tb\n" });
	test({ MAKE(ForStatementNode)(), "for a in b:\n\tb\n" });
	test({ MAKE(ForStatementNode)(), "for a in b:\n" });
	test({ MAKE(StatementNode)(), "for a in b:\n\tif a:\n\t\tb\n" });
	test({ MAKE(StatementNode)(), "for i in arr:\n" });
	test({ MAKE(MethodNode)(), "private static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n" });
	test({ MAKE(MethodNode)(1), "private static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\n\n\t\n\t\n\n\t\tfor i in arr:\n" });
	test({ MAKE(ClassNode)(), "class A extends F<H, I>:\n\tprivate static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n" });
	test({ MAKE(ClassNode)(), "class A extends F<H, I>:\n\tprivate static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\t\tfor i in arr:\n" });
	test({ MAKE(MethodNode)(1), "private static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n" });
	test({ MAKE(MethodNode)(1)      , "private static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\t\tfor i in arr:\n" });
	test({ MAKE(ClassElementNode)(1), "private static E<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\t\tfor i in arr:\n" });
	test({ MAKE(StatementNode)(1), "\tfor i in arr:\n" });
	test({ MAKE(MethodNode)(), "void a():\n" });
	test({ MAKE(CodeBlockNode)(1), "\tif a:\n" });
}

void transpile(const std::filesystem::path& fileName, std::string folder);

int main(int argc, char* argv[]) {
	std::cout << std::boolalpha;
	test();
	std::cout << "\n\n";
	for (int i = 1; i < argc; ++i)
		for (const auto& file : std::filesystem::directory_iterator(argv[i])) {
			const std::filesystem::path& fileName = file.path();
			if (fileName.extension() == ".curzi")
				transpile(fileName, std::string(argv[i]) + "\\out\\");
		}
	return 0;
}

void toJSON(const std::filesystem::path& fileName, const std::string& outFileNameNoExt) {
	std::string program;
	{
		std::ifstream input(fileName);
		std::stringstream buffer;
		buffer << input.rdbuf();
		program = buffer.str();
	}
	std::forward_list<TOKENVALUE> tokens(Tokenizer(program).read());
	Grammarizer g(tokens);
	std::unique_ptr<Node> expected = std::make_unique<FileNode>();
	bool nodeBuilt = expected->build(&g);
	bool programReadEntirely = g.it == g.tokens.end();
	auto nodeBuiltColor = nodeBuilt ? green : red;
	auto programReadEntirelyColor = programReadEntirely ? green : red;
	std::cout << fileName
		<< "built: " << nodeBuiltColor << nodeBuilt << reset
		<< ", entirely: " << programReadEntirelyColor << programReadEntirely << reset << "\n";
	if (!nodeBuilt || !programReadEntirely) {
		for (const auto& t : tokens) {
			if (t.second[0] != '\\')
				std::cout << t.second;
			else if (t.second[1] == 'n') {
				std::cout << "\n";
			}
			else if (t.second[1] == 't') {
				std::cout << "\t";
			}
			else if (t.second[1] == '\\') {
				std::cout << "\\";
			}
		}
		std::cout << "\n\n";
	}
	if (nodeBuilt && programReadEntirely) {
		std::unique_ptr<JSONNodeVisitor> v = std::make_unique<JSONNodeVisitor>();
		expected->accept(v.get());
		JSON out = v->getValue();
		std::ofstream output(outFileNameNoExt + "\\out.json");
		output << out.asString("  ", false) << std::endl;
	}
}

void toCPP(const std::filesystem::path& fileName, const std::string& outFileNameNoExt) {
	std::ofstream h(outFileNameNoExt + "\\out.h");
	std::ofstream cpp(outFileNameNoExt + "\\out.cpp");
	h << "1" << std::endl;
	cpp << "1" << std::endl;
}

void transpile(const std::filesystem::path& fileName, std::string folder) {
	const std::string outFileNameNoExt = folder + std::string(reinterpret_cast<const char*>(fileName.stem().c_str()));
	toJSON(fileName, outFileNameNoExt);
	toCPP(fileName, outFileNameNoExt);
}
