#pragma once
#include <iostream>
#include "toCpp.hpp"
#include "structurizer.hpp"
#include "colored_text.hpp"

template <typename... Ts>
bool testParse(int line, int n_indent, std::string program, bool expectedToBuild = true) {
	auto tokens = Tokenizer(program).read();
	tokens_and_iterator g{ tokens, tokens.begin() };

	bool nodeBuilt = expectedToBuild ?
		And<Ts...>(n_indent).build(g) :
		// if it is expected to fail, try to parse END to make sure it has to get the entire input
		// otherwise they might succeed by skipping tokens at the end with Star or Opts
		And<Ts..., Token<END>>(n_indent).build(g);

	bool programReadEntirely = g.it == g.tokens.end();
	while (!programReadEntirely && (g.it->first == NEWLINE || g.it->first == END))
		programReadEntirely = ++g.it == g.tokens.end();

	if ((nodeBuilt && programReadEntirely) != expectedToBuild) {
		std::cout << "LINE " << line << (line < 100 ? " : " : ": ")
			<< "built: " << colored_text_from_bool(nodeBuilt)
			<< ", entirely: " << colored_text_from_bool(programReadEntirely) << "\n";

		std::cout << program << "\n\n";
		auto save = g.it;
		g.it = g.tokens.begin();
		while (g.it != save) {
			std::cout << g.it->second << " ";
			++g.it;
		}
		std::cout << "\n";
		return false;
	}
	return true;
}

bool testParse() {
	bool ok = true;

	ok &= testParse<Token<END>>(__LINE__, 0, "\n");

	ok &= testParse<Import>(__LINE__, 0, "import a");

	ok &= testParse<Typename>(__LINE__, 0, "E");
	ok &= testParse<Typename>(__LINE__, 0, "E<E>");
	ok &= testParse<Typename>(__LINE__, 0, "E::E");
	ok &= testParse<Typename>(__LINE__, 0, "E::E<F>");
	ok &= testParse<Typename>(__LINE__, 0, "E::E<>");
	ok &= testParse<Typename>(__LINE__, 0, "E<E>");
	ok &= testParse<Typename>(__LINE__, 0, "E<E<E>>");
	ok &= testParse<Typename>(__LINE__, 0, "E<E<E<E::E>>>");
	ok &= testParse<Typename>(__LINE__, 0, "E<E<E<E::E>>,E>");
	ok &= testParse<Typename>(__LINE__, 0, "E<E<E<E<E<E>>>>>");
	ok &= testParse<Typename>(__LINE__, 0, "E<E<E<E < E<   E,>>,  > >,E < E < E<  E< E ,> >,>>>");
	ok &= testParse<Typename>(__LINE__, 0, "E<F<H,I>>::G");
	ok &= testParse<Typename>(__LINE__, 0, "A | B");

	ok &= testParse<TemplateTypenameExtension>(__LINE__, 0, "<>");
	ok &= testParse<TemplateTypenameExtension>(__LINE__, 0, "<E>");

	ok &= testParse<Expression>(__LINE__, 0, "{}");
	ok &= testParse<Expression>(__LINE__, 0, "Set<Int> x");

	ok &= testParse<IfStatement>(__LINE__, 0, "if a:\n");
	ok &= testParse<IfStatement>(__LINE__, 0, "if a:\n\tb\nelse:\n\tc\n");
	ok &= testParse<ForStatement>(__LINE__, 0, "for a in b:\n\tif a:\n\t\tb\n");
	ok &= testParse<ForStatement>(__LINE__, 0, "for i in arr:\n");
	ok &= testParse<ForStatement>(__LINE__, 0, "for i in arr:\n\ti");
	ok &= testParse<ForStatement>(__LINE__, 0, "for i in arr:\n\t\ti", false);
	ok &= testParse<WhileStatement>(__LINE__, 0, "while a:\n");
	ok &= testParse<ReturnStatement>(__LINE__, 0, "return a, b\n");
	ok &= testParse<ReturnStatement>(__LINE__, 0, "return a, b, \n");

	ok &= testParse<Type>(__LINE__, 0, "type A:\n");
	ok &= testParse<Type>(__LINE__, 0, "type A:\n\tA a\n");
	ok &= testParse<Type>(__LINE__, 0, "type A:\n\tA a\n\tA a\n\tA a\n\tA a\n");
	ok &= testParse<Type>(__LINE__, 1, "type A:\n\t\tA a\n\t\tA a\n\t\tA a\n\t\tA a\n");
	ok &= testParse<Type>(__LINE__, 1, "type A:\n\t\tA a\n");
	ok &= testParse<Type>(__LINE__, 0, "type A:\n");
	ok &= testParse<Type>(__LINE__, 0, "type A:\n\tE<F<H,I>>::G method1(K ref k, U ref u, R<U,E<H>,I>::V ref kuv):\n");
	ok &= testParse<Type>(__LINE__, 0, "type A:\n\tE<F<H,I>>::G method1():\n");
	ok &= testParse<Type>(__LINE__, 0, "type A:\n\tE<F<H,I>>::G member1");
	ok &= testParse<Type>(__LINE__, 0, "type A:\n\tE<F<H,I>>::G method1(K ref k, U ref u, R<U,E<H>,I>::V ref kuv):\n\t\tfor i in arr:\n");
	ok &= testParse<Type>(__LINE__, 0, "type B:");

	ok &= testParse<TemplateDeclaration>(__LINE__, 0, "template <x>");
	ok &= testParse<TemplateDeclaration>(__LINE__, 0, "template <B, A>");

	ok &= testParse<Template<Type>>(__LINE__, 0, "template <x>\ntype u:\n");
	ok &= testParse<Template<Type>>(__LINE__, 0, "template <B, A>\ntype A:\n");

	ok &= testParse<File>(__LINE__, 0, "type A:\n\nInt main(Vector<String> ref s):\n");
	ok &= testParse<File>(__LINE__, 0, "\nint main():\n");
	ok &= testParse<File>(__LINE__, 0, "Int main(Vector<String> ref args):\n\tCat cat = {}\n");
	ok &= testParse<File>(__LINE__, 0, "Int main(Vector<String> ref args):\n\tCat cat = {}\n");
	ok &= testParse<File>(__LINE__, 0, "import OS");
	ok &= testParse<File>(__LINE__, 0, "type B:");

	ok &= testParse<FunctionParameter>(__LINE__, 0, "T ref t");
	ok &= testParse<FunctionParameter>(__LINE__, 0, "T ref! t");

	ok &= testParse<Function>(__LINE__, 0, "E<F<H,I>>::G method1(K ref k, U ref u, R<U,E<H>,I>::V ref kuv):\n");
	ok &= testParse<Function>(__LINE__, 1, "E<F<H,I>>::G method1(K ref k, U val u, R<U,E<H>,I>::V val kuv):\n\n\n\t\n\t\n\n\t\tfor i in arr:\n");
	ok &= testParse<Function>(__LINE__, 0, "E<F<H,I>>::G method1():\n");
	ok &= testParse<Function>(__LINE__, 1, "E<F<H,I>>::G method1(K ref k, U ref u, R<U,E<H>,I>::V ref kuv):\n");
	ok &= testParse<Function>(__LINE__, 1, "E<F<H,I>>::G method1(K ref k, U ref u, R<U,E<H>,I>::V ref kuv):\n\t\tfor i in arr:\n");
	ok &= testParse<Function>(__LINE__, 0, "void a():\n");
	ok &= testParse<Function>(__LINE__, 1, "E<F<H,I>>::G method1(K ref k, U ref u, R<U,E<H>,I>::V ref kuv):\n\t\tfor i in arr:\n");
	ok &= testParse<Function>(__LINE__, 1, "E<F<H,I>>::G method1(K ref k, U ref u, R<U,E<H>,I>::V ref kuv):\n");

	ok &= testParse<IfStatement>(__LINE__, 0, "if a:\n");
	ok &= testParse<IfStatement>(__LINE__, 0, "if a:\n\tb\n");
	ok &= testParse<IfStatement>(__LINE__, 0, "if a:\n\tif a:\n\t\tb\n");
	ok &= testParse<IfStatement>(__LINE__, 1, "if a:\n\t\tb\n");
	ok &= testParse<ForStatement>(__LINE__, 0, "for a in b:\n\tb\n");
	ok &= testParse<ForStatement>(__LINE__, 0, "for a in b:\n");

	// previous tests ensure good code works and these tests ensure bad code fails
	ok &= testParse<Type>(__LINE__, 1, "type A:\n\tA a\n\tA a\n\tA a\n\tA a\n", false);
	ok &= testParse<Type>(__LINE__, 0, "type A:\n\t\tA a\n\t\tA a\n\t\tA a\n\t\tA a\n", false);
	return ok;
}
