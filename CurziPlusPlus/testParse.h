#pragma once
#include <iostream>
#include "toCpp.h"
#include "structurizer.h"

template <typename... Ts>
void testParse(int line, int n_indent, std::string program, bool expectedToBuild = true) {

	constexpr auto green = "\033[1;32m";
	constexpr auto red = "\033[1;31m";
	constexpr auto reset = "\033[0m";

	std::forward_list<TOKENVALUE> tokens(Tokenizer(program).read());
	Grammarizer g(tokens);

	bool nodeBuilt = false;
	if (expectedToBuild) {
		auto n = And<Ts...>(n_indent);
		nodeBuilt = n.build(&g);
	}
	else {
		// if it is expected to fail, try to parse END to make sure it has to get the entire input
		// otherwise they might succeed by skipping tokens at the end with Star or Opts
		auto n = And<Ts..., Token<END>>(n_indent);
		nodeBuilt = n.build(&g);
	}

	bool programReadEntirely = g.it == g.tokens.end();
	while (!programReadEntirely && (g.it->first == NEWLINE || g.it->first == END))
		programReadEntirely = ++g.it == g.tokens.end();
	auto nodeBuiltColor = nodeBuilt == expectedToBuild ? green : red;
	auto programReadEntirelyColor = programReadEntirely == expectedToBuild ? green : red;
	std::cout << "LINE " << line << (line < 100 ? " : " : ": ")
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
	}
}

void testParse() {
	std::cout << "PARSE TESTS\n";

	testParse<CommaStar<And<Token<WORD>>>>(__LINE__, 0, "y");
	testParse<Import>(__LINE__, 0, "import string\n");
	testParse<And<IndentToken, Token<WORD>>>(__LINE__, 3, "\t\t\ta");
	testParse<CommaStar<Token<WORD>>>(__LINE__, 0, "y");
	testParse<CommaStar<And<Token<WORD>>>>(__LINE__, 0, "y");
	testParse<Opt<Import>>(__LINE__, 0, "\n");
	testParse<Type>(__LINE__, 0, "type A:\n");
	testParse<Type>(__LINE__, 0, "type A:\n\tA a\n");
	testParse<Type>(__LINE__, 0, "type A:\n\tA a\n\tA a\n\tA a\n\tA a\n");
	testParse<Type>(__LINE__, 1, "type A:\n\t\tA a\n\t\tA a\n\t\tA a\n\t\tA a\n");
	testParse<Type>(__LINE__, 1, "type A:\n\t\tA a\n");
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
	testParse<TemplateDeclaration>(__LINE__, 0, "template <x>");
	testParse<Template<Type>>(__LINE__, 0, "template <x> type u:\n");
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
	testParse<Type>(__LINE__, 0, "type A:\n\tE<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n");
	testParse<Function>(__LINE__, 0, "E<F<H,I>>::G method1():\n");
	testParse<Type>(__LINE__, 0, "type A:\n\tE<F<H,I>>::G method1():\n");
	testParse<Type>(__LINE__, 0, "type A:\n\tE<F<H,I>>::G member1");
	testParse<Type>(__LINE__, 0, "type A:\n\tE<F<H,I>>::G method1(K k, U u, R<U,E<H>,I>::V kuv):\n\t\tfor i in arr:\n");
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
	testParse<Type>(__LINE__, 0, "type B:");
	testParse<File>(__LINE__, 0, "type B:");
	testParse<IfStatement>(__LINE__, 0, "if a:\n\tb\nelse:\n\tc\n");
	testParse<Indent<Indent<Indent<IfStatement>>>>(__LINE__, 0, "if a:\n\t\t\t\tb\n\t\t\telse:\n\t\t\t\tc\n");
	testParse<Star<Statement>>(__LINE__, 0, "Set<int> someContainer = {}\n");
	testParse<Star<Statement>>(__LINE__, 0, "Set<int> someContainer = {}\nSet<int> someContainer = {}\n");
	testParse<Star<Statement>>(__LINE__, 0, "Set<int> someContainer = {}\nfor i in someContainer:\n");
	testParse<Star<Statement>>(__LINE__, 0, "Set<int> someContainer = {}\nfor i in someContainer:\nvector<int> arr = {}\nfor i in arr :\nMap<int, std::string> m = {}\nfor k, v in m:\n");
	testParse<And<AdditiveExpression, Token<LT>, AdditiveExpression, Token<GT>, AdditiveExpression>>(__LINE__, 0, "Set<Int> x");
	testParse<Expression>(__LINE__, 0, "Set<Int> x");

	std::cout << "=====================\nREVERSING LOGIC OF TESTS\nRED TRUE FOR `BUILT` IS OK IF `ENTIRELY` IS GREEN FALSE\n=====================\n";
	// basically previous tests ensure good code should work
	// and these tests ensure bad code should fail
	testParse<Type>(__LINE__, 1, "type A:\n\tA a\n\tA a\n\tA a\n\tA a\n", false);
	testParse<Type>(__LINE__, 0, "type A:\n\t\tA a\n\t\tA a\n\t\tA a\n\t\tA a\n", false);
}
