#include "test_parse.hpp"

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
	ok &= test_parse_correct<And<Token<LT>, TypenameOrExpression, Token<GT>>>(__LINE__, 0, "<E>");
	ok &= test_parse_correct<And<Token<LT>, CommaStar<TypenameOrExpression>, Token<GT>>>(__LINE__, 0, "<E>");
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
	ok &= test_parse_incorrect<Expression>(__LINE__, 0, "Set<Int> x");
	ok &= test_parse_correct<VariableDeclarationStatement<function_context>>(__LINE__, 0, "Set<Int> x = {}");
	ok &= test_parse_correct<Expression>(__LINE__, 0, "size<Animal>(animals)");
	ok &= test_parse_correct<Expression>(__LINE__, 0, "size<Animal, Animal>(animals)");
	ok &= test_parse_correct<Expression>(__LINE__, 0, "a<b,c>?d>");
	ok &= test_parse_correct<Expression>(__LINE__, 0, "c >=? '0' and c <=? '9'");
	ok &= test_parse_correct<Expression>(__LINE__, 0, "'\\\\'");
	ok &= test_parse_correct<Expression>(__LINE__, 0, "a >? b");
	ok &= test_parse_correct<VariableDeclarationStatement<function_context>>(__LINE__, 0, "Bool c = a >? c");
	ok &= test_parse_correct<CompareOperator>(__LINE__, 0, ">?");

	ok &= test_parse_correct<And<Token<COLON>, Newline, Indent<Star<Statement<function_context>>>>>(__LINE__, 0, ":\n\tx");
	ok &= test_parse_correct<And<Newline, Indent<Star<Statement<function_context>>>>>(__LINE__, 0, "\n\tx");
	ok &= test_parse_correct<Indent<Star<Statement<function_context>>>>(__LINE__, 0, "\tx");
	ok &= test_parse_correct<Indent<And<IndentToken, Word>>>(__LINE__, 0, "\tx");
	ok &= test_parse_correct<IndentToken, Word>(__LINE__, 1, "\tx");
	ok &= test_parse_correct<IfStatement<function_context>>(__LINE__, 0, "if a:\n\tb");
	ok &= test_parse_correct<IfStatement<function_context>>(__LINE__, 0, "if a:\n");
	ok &= test_parse_correct<IfStatement<function_context>>(__LINE__, 0, "if a:\n\tb\nelse:\n\tc\n");
	ok &= test_parse_correct<ForStatement<function_context>>(__LINE__, 0, "for a in b:\n\tif a:\n\t\tb\n");
	ok &= test_parse_correct<ForStatement<function_context>>(__LINE__, 0, "for i in arr:\n");
	ok &= test_parse_correct<ForStatement<function_context>>(__LINE__, 0, "for i in arr:\n\ti");
	ok &= test_parse_correct<WhileStatement<function_context>>(__LINE__, 0, "while a:\n");
	ok &= test_parse_correct<ReturnStatement<function_context>>(__LINE__, 0, "return a, b\n");
	ok &= test_parse_correct<ReturnStatement<function_context>>(__LINE__, 0, "return a, b, \n");
	ok &= test_parse_correct<MatchStatement<function_context>>(__LINE__, 0, "match dir:\n\tfilesystem::directory d:\n");
	ok &= test_parse_correct<MatchStatement<function_context>>(__LINE__, 0, "match dir:\n\tfilesystem::directory d:\n\t\treturn test_transpile_folder(dir)\n\tfilesystem::file f:\n");
	ok &= test_parse_correct<Enum>(__LINE__, 0, "enum TOKENS:\n");
	ok &= test_parse_correct<Enum>(__LINE__, 0, "enum TOKENS:\n\ta\n");
	ok &= test_parse_correct<Enum>(__LINE__, 0, "enum TOKENS:\n\ta//a\n");
	ok &= test_parse_correct<Enum>(__LINE__, 0, "enum TOKENS:\n\ta\n\tb\n");
	ok &= test_parse_correct<Enum>(__LINE__, 0, "enum TOKENS:\n\ta\n\n\tb\n");
	ok &= test_parse_correct<Enum>(__LINE__, 0, "enum TOKENS:\n\ta//a\n\n\tb\n");
	ok &= test_parse_correct<And<Statement<function_context>, Statement<function_context>>>(__LINE__, 0, "a // a\na\n");
	ok &= test_parse_correct<And<Statement<function_context>, Statement<function_context>>>(__LINE__, 0, "a //\na\n");
	ok &= test_parse_correct<And<Statement<function_context>, Statement<function_context>>>(__LINE__, 0, "a // a \na\n");

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

	ok &= test_parse_correct<IfStatement<function_context>>(__LINE__, 0, "if a:\n");
	ok &= test_parse_correct<IfStatement<function_context>>(__LINE__, 0, "if a:\n\tb\n");
	ok &= test_parse_correct<IfStatement<function_context>>(__LINE__, 0, "if a:\n\tif a:\n\t\tb\n");
	ok &= test_parse_correct<IfStatement<function_context>>(__LINE__, 1, "if a:\n\t\tb\n");
	ok &= test_parse_correct<ForStatement<function_context>>(__LINE__, 0, "for a in b:\n\tb\n");
	ok &= test_parse_correct<ForStatement<function_context>>(__LINE__, 0, "for a in b:\n");

	ok &= test_parse_correct<Template>(__LINE__, 0, "template f<T>:\n");
	ok &= test_parse_correct<Template>(__LINE__, 0, "template f<T>:\n\tInt f():\n\t\treturn 0");

	ok &= test_parse_correct<VariableDeclarationStatement<function_context>>(__LINE__, 0, "Int a = {}\n");
	ok &= test_parse_correct<VariableDeclarationStatement<type_context>>(__LINE__, 0, "Int a = {}\n");
	ok &= test_parse_correct<Statement<type_context>>(__LINE__, 0, "#Int a = {}\n");
	ok &= test_parse_correct<TypeElement>(__LINE__, 1, "\t#Int a = {}\n");
	ok &= test_parse_correct<Statement<type_context>>(__LINE__, 1, "\t#if True:\n\t\t#Int a = {}\n");
	ok &= test_parse_correct<IndentToken, Token<POUND>, StatementOpts<type_context>>(__LINE__, 1, "\t#if True:\n\t\t#Int a = {}\n");
	ok &= test_parse_correct<Token<IF>, Expression, ColonIndentCodeBlock<type_context>>(__LINE__, 0, "if True:\n");
	ok &= test_parse_correct<IfStatement<type_context>>(__LINE__, 0, "if True:\n\t#Int a = {}\n");
	ok &= test_parse_correct<Indent<Star<Or<Token<NEWLINE>, Expect<Statement<function_context>>>>>>(__LINE__, 0, "");
	ok &= test_parse_correct<Indent<Star<Or<Token<NEWLINE>, Expect<Statement<type_context>>>>>>(__LINE__, 0, "");
		
	ok &= test_parse_correct<Star<TypeElement>>(__LINE__, 1, "\t#Int a = {}\n");

	ok &= test_parse_correct<Star<TypeElement>>(__LINE__, 2, "\t\t#Int a = {}\n");

	ok &= test_parse_correct<Type>(__LINE__, 0, "type T:\n\t#Int a = {}\n");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type T:\n\t#Int a = {}\n\tInt a\n");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type T:\n\t#if True:\n\t\t#Int a = {}\n");
	ok &= test_parse_correct<And<IndentToken, MemberVariable>>(__LINE__, 2, "\t\tInt a\n");
	ok &= test_parse_correct<Statement<type_context>>(__LINE__, 1, "\t#if True:\n\t\t#Int a = {}\n");
	ok &= test_parse_correct<Statement<type_context>>(__LINE__, 1, "\t#if True:\n\t\tInt a\n");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type T:\n\t#if True:\n\t\tInt a\n");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type T:\n\t#if True:\n\t\tInt a\n\t\tInt a\n");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type T:\n\t#if True:\n\t\tInt a\n\t#if True:\n\t\tInt a\n");
	ok &= test_parse_correct<Type>(__LINE__, 0, "type T:\n\t#if True:\n\t\tInt a\n\t#else:\n\t\tInt b\n");


	ok &= test_parse_incorrect<VariableDeclarationStatement<function_context>>(__LINE__, 0, "Set<Int> x");
	ok &= test_parse_incorrect<Type>(__LINE__, 1, "type A:\n\tA a\n\tA a\n\tA a\n\tA a\n");
	ok &= test_parse_incorrect<Type>(__LINE__, 0, "type A:\n\t\tA a\n\t\tA a\n\t\tA a\n\t\tA a\n");
	ok &= test_parse_incorrect<ForStatement<function_context>>(__LINE__, 0, "for i in arr:\n\t\ti");


	ok &= test_parse_correct<Typename, Token<GT>>(__LINE__, 0, "X>");
	ok &= test_parse_correct<TypeElement>(__LINE__, 0, "Tuple<X> ? a");
	ok &= test_parse_correct<TypeElement>(__LINE__, 0, "Tuple<Tuple<X>?> a");
	ok &= test_parse_correct<TypeElement>(__LINE__, 0, "Tuple<reference_wrapper<DB>?> services");

	return ok;
}