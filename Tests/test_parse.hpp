#pragma once
#include <iostream>
#include "toCpp.hpp"
#include "structurizer.hpp"
#include "colored_text.hpp"

template <bool expected_to_built, typename... Ts>
bool test_parse(int line, int n_indent, std::string program) {
	auto tokens = Tokenizer(program).read();
	tokens_and_iterator g{ tokens, tokens.begin() };
	And node = [&]() {
		if constexpr (expected_to_built)
			return And<Ts...>(n_indent);
		else
			// if it is expected to fail, try to parse END to make sure it has to get the entire input
			// otherwise they might succeed by skipping tokens at the end with Star or Opts
			return And<Ts..., Token<END>>(n_indent);
	}();
	bool nodeBuilt = node.build(g.it);

	bool programReadEntirely = g.it == g.tokens.end();
	while (!programReadEntirely && (g.it->first == NEWLINE || g.it->first == END))
		programReadEntirely = ++g.it == g.tokens.end();

	if ((nodeBuilt && programReadEntirely) != expected_to_built) {
		std::cout << "LINE " << line << (line < 100 ? " : " : ": ")
			<< "built: " << colored_text_from_bool(nodeBuilt)
			<< ", entirely: " << colored_text_from_bool(programReadEntirely) << "\n";

		std::cout << program << "\n\n";
		auto it = g.tokens.begin();
		while (it != g.it) {
			std::cout << it->second << " ";
			++it;
		}
		std::cout << "\n";
		return false;
	}
	return true;
}

template <typename... Ts>
auto test_parse_correct(auto&&... es) {
	return test_parse<true, Ts...>(std::forward<decltype(es)>(es)...);
}

template <typename... Ts>
auto test_parse_incorrect(auto&&... es) {
	return test_parse<false, Ts...>(std::forward<decltype(es)>(es)...);
}

bool testParse() {
	bool ok = true;

	ok &= test_parse_correct<Token<END>>(__LINE__, 0, "");
	ok &= test_parse_incorrect<Token<END>>(__LINE__, 0, "\n");
	ok &= test_parse_correct<Token<NEWLINE>>(__LINE__, 0, "\n");
	ok &= test_parse_correct<Token<NEWLINE>>(__LINE__, 0, "\t\n");
	ok &= test_parse_correct<Token<NEWLINE>>(__LINE__, 0, " \n");
	ok &= test_parse_incorrect<Token<NEWLINE>>(__LINE__, 0, "\n\n");
	ok &= test_parse_incorrect<Token<NEWLINE>>(__LINE__, 0, "\n\t");

	ok &= test_parse_correct<Import>(__LINE__, 0, "import a");

	ok &= test_parse_correct<Typename>(__LINE__, 0, "E");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "E<E>");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "E::E");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "E::E<F>");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "E::E<>");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "E<E>");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "E<E<E>>");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "E<E<E<E::E>>>");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "E<E<E<E::E>>,E>");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "E<E<E<E<E<E>>>>>");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "E<E<E<E < E<   E,>>,  > >,E < E < E<  E< E ,> >,>>>");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "E<F<H,I>>::G");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "A | B");

	ok &= test_parse_correct<TemplateTypenameExtension>(__LINE__, 0, "<>");
	ok &= test_parse_correct<TemplateTypenameExtension>(__LINE__, 0, "<E>");

	ok &= test_parse_correct<Expression>(__LINE__, 0, "{}");
	ok &= test_parse_correct<CompareExpression>(__LINE__, 0, "2 + 2 * 2");
	ok &= test_parse_correct<CompareExpression>(__LINE__, 0, "2 + 2 * 2 + 2");
	ok &= test_parse_correct<CompareExpression>(__LINE__, 0, "2 + 2 * 2 + 2 * 2 + 2 * 2 + 2");
	ok &= test_parse_correct<CompareExpression>(__LINE__, 0, "((2) + ((2) * 2) + 2) * 2 + 2 * 2 + 2");
	ok &= test_parse_correct<CompareExpression>(__LINE__, 0, "a  <? b >? c");
	ok &= test_parse_correct<CompareExpression>(__LINE__, 0, "Set<?Int>? x");
	ok &= test_parse_correct<Expression>(__LINE__, 0, "()");
	ok &= test_parse_correct<Expression>(__LINE__, 0, "a  <? b >? c");
	ok &= test_parse_incorrect<Expression>(__LINE__, 0,                 "Set<Int> x");
	ok &= test_parse_correct<VariableDeclarationStatement>(__LINE__, 0, "Set<Int> x = {}"); 
	ok &= test_parse_correct<Expression>(__LINE__, 0, "size<Animal>(animals)");
	ok &= test_parse_correct<Expression>(__LINE__, 0, "size<Animal, Animal>(animals)");
	ok &= test_parse_correct<Expression>(__LINE__, 0, "a<b,c>?d>");
	
	
	ok &= test_parse_correct<And<Token<COLON>, Newline, Indent<Star<Statement>>>>(__LINE__, 0, ":\n\tx");
	ok &= test_parse_correct<And<Newline, Indent<Star<Statement>>>>(__LINE__, 0, "\n\tx");
	ok &= test_parse_correct<Indent<Star<Statement>>>(__LINE__, 0, "\tx");
	ok &= test_parse_correct<Indent<And<IndentToken, Word>>>(__LINE__, 0, "\tx");
	ok &= test_parse_correct<IndentToken, Word>(__LINE__, 1, "\tx");
	ok &= test_parse_correct<IfStatement>(__LINE__, 0, "if a:\n");
	ok &= test_parse_correct<IfStatement>(__LINE__, 0, "if a:\n\tb");
	ok &= test_parse_correct<IfStatement>(__LINE__, 0, "if a:\n\tb\nelse:\n\tc\n");
	ok &= test_parse_correct<ForStatement>(__LINE__, 0, "for a in b:\n\tif a:\n\t\tb\n");
	ok &= test_parse_correct<ForStatement>(__LINE__, 0, "for i in arr:\n");
	ok &= test_parse_correct<ForStatement>(__LINE__, 0, "for i in arr:\n\ti");
	ok &= test_parse_correct<WhileStatement>(__LINE__, 0, "while a:\n");
	ok &= test_parse_correct<ReturnStatement>(__LINE__, 0, "return a, b\n");
	ok &= test_parse_correct<ReturnStatement>(__LINE__, 0, "return a, b, \n");

	ok &= test_parse_correct<Type>(__LINE__, 0, "type A:\n");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type A:\n\tA a\n");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type A:\n\tA a\n\tA a\n\tA a\n\tA a\n");
	ok &= test_parse_correct<Type>(__LINE__, 1, "type A:\n\t\tA a\n\t\tA a\n\t\tA a\n\t\tA a\n");
	ok &= test_parse_correct<Type>(__LINE__, 1, "type A:\n\t\tA a\n");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type A:\n");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type A:\n\tE<F<H,I>>::G method1(K ref k, U ref u, R<U,E<H>,I>::V ref kuv):\n");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type A:\n\tE<F<H,I>>::G method1():\n");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type A:\n\tE<F<H,I>>::G member1");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type A:\n\tE<F<H,I>>::G method1(K ref k, U ref u, R<U,E<H>,I>::V ref kuv):\n\t\tfor i in arr:\n");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type B:");

	ok &= test_parse_correct<File>(__LINE__, 0, "type A:\n\nInt main(Vector<String> ref s):\n");
	ok &= test_parse_correct<File>(__LINE__, 0, "\nint main():\n");
	ok &= test_parse_correct<File>(__LINE__, 0, "Int main(Vector<String> ref args):\n\tCat cat = {}\n");
	ok &= test_parse_correct<File>(__LINE__, 0, "Int main(Vector<String> ref args):\n\tCat cat = {}\n");
	ok &= test_parse_correct<File>(__LINE__, 0, "import OS");
	ok &= test_parse_correct<File>(__LINE__, 0, "type B:");

	ok &= test_parse_correct<FunctionParameter>(__LINE__, 0, "T ref t");
	ok &= test_parse_correct<FunctionParameter>(__LINE__, 0, "T ref! t");

	ok &= test_parse_correct<Function>(__LINE__, 0, "E<F<H,I>>::G method1(K ref k, U ref u, R<U,E<H>,I>::V ref kuv):\n");
	ok &= test_parse_correct<Function>(__LINE__, 1, "E<F<H,I>>::G method1(K ref k, U val u, R<U,E<H>,I>::V val kuv):\n\n\n\t\n\t\n\n\t\tfor i in arr:\n");
	ok &= test_parse_correct<Function>(__LINE__, 0, "E<F<H,I>>::G method1():\n");
	ok &= test_parse_correct<Function>(__LINE__, 1, "E<F<H,I>>::G method1(K ref k, U ref u, R<U,E<H>,I>::V ref kuv):\n");
	ok &= test_parse_correct<Function>(__LINE__, 1, "E<F<H,I>>::G method1(K ref k, U ref u, R<U,E<H>,I>::V ref kuv):\n\t\tfor i in arr:\n");
	ok &= test_parse_correct<Function>(__LINE__, 0, "void a():\n");
	ok &= test_parse_correct<Function>(__LINE__, 1, "E<F<H,I>>::G method1(K ref k, U ref u, R<U,E<H>,I>::V ref kuv):\n\t\tfor i in arr:\n");
	ok &= test_parse_correct<Function>(__LINE__, 1, "E<F<H,I>>::G method1(K ref k, U ref u, R<U,E<H>,I>::V ref kuv):\n");

	ok &= test_parse_correct<IfStatement>(__LINE__, 0, "if a:\n");
	ok &= test_parse_correct<IfStatement>(__LINE__, 0, "if a:\n\tb\n");
	ok &= test_parse_correct<IfStatement>(__LINE__, 0, "if a:\n\tif a:\n\t\tb\n");
	ok &= test_parse_correct<IfStatement>(__LINE__, 1, "if a:\n\t\tb\n");
	ok &= test_parse_correct<ForStatement>(__LINE__, 0, "for a in b:\n\tb\n");
	ok &= test_parse_correct<ForStatement>(__LINE__, 0, "for a in b:\n");

	ok &= test_parse_correct<Template>(__LINE__, 0, "template f<T>:\n");
	ok &= test_parse_correct<Template>(__LINE__, 0, "template f<T>:\n\tInt f():\n\t\treturn 0");
		


	ok &= test_parse_incorrect<VariableDeclarationStatement>(__LINE__, 0, "Set<Int> x");
	ok &= test_parse_incorrect<Type>(__LINE__, 1, "type A:\n\tA a\n\tA a\n\tA a\n\tA a\n");
	ok &= test_parse_incorrect<Type>(__LINE__, 0, "type A:\n\t\tA a\n\t\tA a\n\t\tA a\n\t\tA a\n");
	ok &= test_parse_incorrect<ForStatement>(__LINE__, 0, "for i in arr:\n\t\ti");
	return ok;
}
