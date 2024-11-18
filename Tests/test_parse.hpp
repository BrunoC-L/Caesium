#pragma once
#include <iostream>
#include "core/toCpp.hpp"
#include "core/structurizer.hpp"
#include "utility/colored_text.hpp"

template <bool expected_to_built, typename... Ts>
bool test_parse(int line, int n_indent, std::string program) {
	auto tokens = Tokenizer(program).read();
	Iterator it = { tokens, 0 };
	And node = [&]() {
		if constexpr (expected_to_built)
			return And<Ts...>(n_indent);
		else
			// if it is expected to fail, try to parse END to make sure it has to get the entire input
			// otherwise they might succeed by skipping tokens at the end with Star or Opts
			return And<Ts..., Token<END>>(n_indent);
	}();
	try {
		bool nodeBuilt = build(node, it);

		bool programReadEntirely = it.index == it.vec.size();
		while (!programReadEntirely && (it.vec[it.index].first == NEWLINE || it.vec[it.index].first == END))
			programReadEntirely = ++it.index == it.vec.size();

		if ((nodeBuilt && programReadEntirely) != expected_to_built) {
			std::cout << "LINE " << line << (line < 100 ? " : " : ": ")
				<< "built: " << colored_text_from_bool(nodeBuilt)
				<< ", entirely: " << colored_text_from_bool(programReadEntirely) << "\n";

			std::cout << program << "\n\n";
			auto index = it.index;
			while (index != it.index)
				std::cout << it.vec[index++].second << " ";
			std::cout << "\n";
			return false;
		}
	}
	catch (const parse_error& e) {
		size_t line = 1;
		{
			auto index = it.index;
			while (index != e.beg_offset) {
				for (const char& c : it.vec[index].second)
					if (c == '\n')
						++line;
				++index;
			}
		}
		std::stringstream ss;
		ss << "Unable to parse in test "
			<< e.name_of_rule
			<< "\nContent was: \n";
		auto index = e.beg_offset;
		while (index != it.index)
			ss << it.vec[index++].second;
		ss << "\n";
		std::cout << ss.str();
		throw std::runtime_error(ss.str());
	}
	return true;
}

template <typename... Ts>
bool test_parse_correct(auto&&... es) {
	return test_parse<true, Ts...>(std::forward<decltype(es)>(es)...);
}

template <typename... Ts>
bool test_parse_incorrect(auto&&... es) {
	return test_parse<false, Ts...>(std::forward<decltype(es)>(es)...);
}

bool test_parse() {
	using namespace grammar;
	bool ok = true;

	ok &= test_parse_correct<Token<END>>(__LINE__, 0, "");
	ok &= test_parse_incorrect<Token<END>>(__LINE__, 0, "\n");
	ok &= test_parse_correct<Newline>(__LINE__, 0, "\n");
	ok &= test_parse_correct<Newline>(__LINE__, 0, "\t\n");
	ok &= test_parse_correct<Newline>(__LINE__, 0, " \n");
	ok &= test_parse_correct<Newline>(__LINE__, 0, "\n");
	ok &= test_parse_correct<Newline>(__LINE__, 0, " // a \n");

	ok &= test_parse_correct<Import>(__LINE__, 0, "import a");

	ok &= test_parse_correct<Typename>(__LINE__, 0, "E");
	ok &= test_parse_correct<And<TypenameOrExpression>>(__LINE__, 0, "E");
	ok &= test_parse_correct<And<TypenameOrExpression, Token<GT>>>(__LINE__, 0, "E>");
	ok &= test_parse_correct<And<Token<LT>, TypenameOrExpression, Token<GT>>>(__LINE__, 0, "<E>");
	ok &= test_parse_correct<And<Token<LT>, Alloc<TypenameOrExpression>, Token<GT>>>(__LINE__, 0, "<E>");
	ok &= test_parse_correct<And<Token<LT>, CommaStar<Alloc<TypenameOrExpression>>, Token<GT>>>(__LINE__, 0, "<E>");
	ok &= test_parse_correct<TemplateTypenameExtension>(__LINE__, 0, "<E>");
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
	ok &= test_parse_correct<Typename>(__LINE__, 0, "A?");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "std::reference_wrapper?");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "std::reference_wrapper<DB> ?");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "Tuple<std::reference_wrapper<DB> ?>");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "DB ?");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "DB ref");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "DB ? ref");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "DB ref ?");

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
	ok &= test_parse_correct<Expression>(__LINE__, 0, "x.x");
	ok &= test_parse_correct<Expression>(__LINE__, 0, "a  <? b >? c");
	ok &= test_parse_incorrect<Expression>(__LINE__, 0,                 "Set<Int> x");
	ok &= test_parse_correct<VariableDeclarationStatement>(__LINE__, 0, "Set<Int> x = {}");
	ok &= test_parse_correct<Expression>(__LINE__, 0, "size<Animal>(animals)");
	ok &= test_parse_correct<Expression>(__LINE__, 0, "size<Animal, Animal>(animals)");
	ok &= test_parse_correct<Expression>(__LINE__, 0, "a<b,c>?d>");
	ok &= test_parse_correct<Expression>(__LINE__, 0, "c >=? '0' and c <=? '9'");
	ok &= test_parse_correct<Expression>(__LINE__, 0, "'\\\\'");
	ok &= test_parse_correct<Expression>(__LINE__, 0, "a >? b");
	ok &= test_parse_correct<VariableDeclarationStatement>(__LINE__, 0, "Bool c = a >? c");
	ok &= test_parse_correct<CompareOperator>(__LINE__, 0, ">?");

	ok &= test_parse_correct<And<Token<COLON>, Newline, Indent<Star<Statement>>>>(__LINE__, 0, ":\n\tx");
	ok &= test_parse_correct<And<Newline, Indent<Star<Statement>>>>(__LINE__, 0, "\n\tx");
	ok &= test_parse_correct<Indent<Star<Statement>>>(__LINE__, 0, "\tx");
	ok &= test_parse_correct<Indent<And<IndentToken, Word>>>(__LINE__, 0, "\tx");
	ok &= test_parse_correct<IndentToken, Word>(__LINE__, 1, "\tx");
	ok &= test_parse_correct<IfStatement>(__LINE__, 0, "if a:\n\tb");
	ok &= test_parse_correct<IfStatement>(__LINE__, 0, "if a:\n");
	ok &= test_parse_correct<IfStatement>(__LINE__, 0, "if a:\n\tb\nelse:\n\tc\n");
	ok &= test_parse_correct<ForStatement>(__LINE__, 0, "for a in b:\n\tif a:\n\t\tb\n");
	ok &= test_parse_correct<ForStatement>(__LINE__, 0, "for i in arr:\n");
	ok &= test_parse_correct<ForStatement>(__LINE__, 0, "for i in arr:\n\ti");
	ok &= test_parse_correct<WhileStatement>(__LINE__, 0, "while a:\n");
	ok &= test_parse_correct<ReturnStatement>(__LINE__, 0, "return a, b\n");
	ok &= test_parse_correct<ReturnStatement>(__LINE__, 0, "return a, b, \n");
	ok &= test_parse_correct<MatchStatement>(__LINE__, 0, "match dir:\n\tfilesystem::directory d:\n");
	ok &= test_parse_correct<MatchStatement>(__LINE__, 0, "match dir:\n\tfilesystem::directory d:\n\t\treturn test_transpile_folder(dir)\n\tfilesystem::file f:\n");
	ok &= test_parse_correct<Enum>(__LINE__, 0, "enum TOKENS:\n");
	ok &= test_parse_correct<Enum>(__LINE__, 0, "enum TOKENS:\n\ta\n"); 
	ok &= test_parse_correct<Enum>(__LINE__, 0, "enum TOKENS:\n\ta//a\n");
	ok &= test_parse_correct<Enum>(__LINE__, 0, "enum TOKENS:\n\ta\n\tb\n");
	ok &= test_parse_correct<Enum>(__LINE__, 0, "enum TOKENS:\n\ta\n\n\tb\n");
	ok &= test_parse_correct<Enum>(__LINE__, 0, "enum TOKENS:\n\ta//a\n\n\tb\n");
	ok &= test_parse_correct<And<Statement, Statement>>(__LINE__, 0, "a // a\na\n");
	ok &= test_parse_correct<And<Statement, Statement>>(__LINE__, 0, "a //\na\n");
	ok &= test_parse_correct<And<Statement, Statement>>(__LINE__, 0, "a // a \na\n");

	ok &= test_parse_correct<Type>(__LINE__, 0, "type A:\n");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type A:\n\tA a\n");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type A:\n\tA a\n\tA a\n\tA a\n\tA a\n");
	ok &= test_parse_correct<Type>(__LINE__, 1, "type A:\n\t\tA a\n\t\tA a\n\t\tA a\n\t\tA a\n");
	ok &= test_parse_correct<Type>(__LINE__, 1, "type A:\n\t\tA a\n");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type A:\n");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type A:\n\tE<F<H,I>>::G member1");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type B:");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type Token:\n\tString value\n\tInt n_indent\n");
	ok &= test_parse_correct<Type>(__LINE__, 1, "type ServerServiceProvider:\n\t\tT services");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type ServerServiceProvider:\n\tTuple<std::reference_wrapper<DB>> services");
	ok &= test_parse_correct<Type>(__LINE__, 1, "type ServerServiceProvider:\n\t\tTuple<std::reference_wrapper<DB>> services");
	ok &= test_parse_correct<Type>(__LINE__, 1, "type ServerServiceProvider:\n\t\tstd::reference_wrapper<DB> services");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "std::reference_wrapper");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "std::reference_wrapper<DB>");
	ok &= test_parse_correct<Typename>(__LINE__, 0, "std::reference_wrapper<DB> ?");
	ok &= test_parse_incorrect<Typename>(__LINE__, 0, "std::reference_wrapper<DB> x");
	ok &= test_parse_incorrect<Typename>(__LINE__, 0, "std::reference_wrapper<DB> ? services");
	ok &= test_parse_correct<Expression>(__LINE__, 0, "DB>? services");
	ok &= test_parse_incorrect<Expression>(__LINE__, 0, "DB> ? services");
	ok &= test_parse_correct<MemberVariable>(__LINE__, 0, "std::reference_wrapper<DB> ? services");
	ok &= test_parse_correct<And<Typename, Word>>(__LINE__, 0, "std::reference_wrapper<DB> ? services");
	ok &= test_parse_correct<Type>(__LINE__, 1, "type ServerServiceProvider:\n\t\tstd::reference_wrapper<DB> ? services");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type ServerServiceProvider:\n\tTuple<std::reference_wrapper<DB> ?> services");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type ServerServiceProvider2 :\n\tTuple<DB ref ?> services");
	

	ok &= test_parse_correct<Exists>(__LINE__, 0, "exists:\n");
	ok &= test_parse_correct<Exists>(__LINE__, 0, "exists:\n\tsf:\n");
	ok &= test_parse_correct<Exists>(__LINE__, 0, "exists:\n\tsf:\n\t\ttype sf_time:\n");
	ok &= test_parse_correct<NameSpace>(__LINE__, 0, "sf:\n");

	ok &= test_parse_correct<File>(__LINE__, 0, "type A:\n\nInt main(Vector<String> ref s):\n");
	ok &= test_parse_correct<File>(__LINE__, 0, "\nint main():\n");
	ok &= test_parse_correct<File>(__LINE__, 0, "Int main(Vector<String> ref args):\n\tCat cat = {}\n");
	ok &= test_parse_correct<File>(__LINE__, 0, "Int main(Vector<String> ref args):\n\tCat cat = {}\n");
	ok &= test_parse_correct<File>(__LINE__, 0, "import OS");
	ok &= test_parse_correct<File>(__LINE__, 0, "type B:");
	ok &= test_parse_correct<File>(__LINE__, 0, "type ServerServiceProvider:\n\tTuple<std::reference_wrapper<DB> ?> services");
	ok &= test_parse_correct<File>(__LINE__, 0, "type ServerServiceProvider:\n\tTuple<std::reference_wrapper<DB>?> services");
	ok &= test_parse_correct<File>(__LINE__, 0, "type ServerServiceProvider2 :\n\tTuple<DB ref ?> services");


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


	ok &= test_parse_correct<Typename, Token<GT>>(__LINE__, 0, "X>");
	ok &= test_parse_correct<TypeElement>(__LINE__, 0, "Tuple<X> ? a");
	ok &= test_parse_correct<TypeElement>(__LINE__, 0, "Tuple<Tuple<X>?> a");
	ok &= test_parse_correct<TypeElement>(__LINE__, 0, "Tuple<reference_wrapper<DB>?> services");
	return ok;    

}
