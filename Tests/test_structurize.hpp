#pragma once
#include <iostream>
#include "core/toCPP.hpp"
#include "structured/structurizer.hpp"
#include "utility/colored_text.hpp"
#include "utility/as_vec.hpp"

rule_info tests_rule_info_stub() {
	return {
		"tests_rule_info_stub",
		{ 0, 0 },
		{ 0, 0 }
	};
}

template <bool exp_ok, typename T>
bool test_structurize(int line, int n_indent, std::string program, auto&& expected) {
	auto tokens = Tokenizer(program).read();
	Iterator it = { .vec = tokens, .index = 0 , .line = 0, .col = 0, .file_name = test_file_name_stub() };
	T node(n_indent);
	bool nodeBuilt = build(node, it);

	bool programReadEntirely = it.index == it.vec.size();
	while (!programReadEntirely && (it.vec[it.index].first == NEWLINE || it.vec[it.index].first == END))
		programReadEntirely = ++it.index == it.vec.size();

	if (!nodeBuilt || !programReadEntirely) {
		std::cout << "LINE " << line << (line < 100 ? " : " : ": ")
			<< "built: " << colored_text_from_bool(nodeBuilt)
			<< ", entirely: " << colored_text_from_bool(programReadEntirely) << "\n";

		std::cout << program << "\n\n";
		unsigned index = 0;
		while (index != it.index)
			std::cout << it.vec[index++].second << " ";
		std::cout << "\n";
		return false;
	}
	using DT = std::remove_cvref_t<decltype(expected)>;
	DT structurized = [&]() -> DT {
			if constexpr (std::is_same_v<DT, NodeStructs::Expression>)
				return getExpressionStruct(test_file_name_stub(), tokens, node);
			else if constexpr (std::is_same_v<DT, NodeStructs::Function>)
				return getStruct(test_file_name_stub(), tokens, node, std::nullopt);
			else if constexpr (std::is_same_v<DT, NodeStructs::Typename>)
				return getStruct(test_file_name_stub(), tokens, node, tag_allow_value_category_or_empty{});
			else if constexpr (std::is_same_v<DT, NodeStructs::WordTypenameOrExpression>)
				return getStruct(test_file_name_stub(), tokens, node, tag_allow_value_category_or_empty{});
			else
				return getStruct(test_file_name_stub(), tokens, node);
		}();
	bool ok = expected <=> structurized == std::strong_ordering::equivalent;
	if (ok != exp_ok) {
		std::cout << "Comparison " << colored_text_with_bool(exp_ok ? "FAILED" : "WORKED", false) << " for LINE " << line << "\n" << program << "\n\n";
		unsigned index = 0;
		while (index != it.index)
			std::cout << it.vec[index++].second << " ";
		std::cout << "\n";
		return false;
	}
	return ok == exp_ok;
}

template <typename T>
bool test_structurize_not_equals(auto&&... args) {
	return test_structurize<false, T>(std::forward<decltype(args)>(args)...);
}

template <typename T>
bool test_structurize_equals(auto&&... args) {
	return test_structurize<true, T>(std::forward<decltype(args)>(args)...);
}

Token<STRING> str_parse(std::string s) {
	auto tokens = Tokenizer(s).read();
	Iterator it = { .vec = tokens, .index = 0 , .line = 0, .col = 0, .file_name = test_file_name_stub() };
	Token<STRING> res{ 0 };
	build(res, it);
	return res;
}

bool test_structurize_equals() {
	using namespace grammar;
	bool ok = true;

	ok &= test_structurize_equals<Typename>(__LINE__, 0, "a::b::c",
		make_typename(
			NodeStructs::NamespacedTypename{
				make_typename(NodeStructs::NamespacedTypename{
					make_typename(NodeStructs::BaseTypename{ "a" }, NodeStructs::Value{}, tests_rule_info_stub()),
					"b"
				}, NodeStructs::Value{}, tests_rule_info_stub()),
				"c"
			}, NodeStructs::Value{}, tests_rule_info_stub()
		));

	ok &= test_structurize_equals<Typename>(__LINE__, 0, "a | b",
		make_typename(
			NodeStructs::UnionTypename{
				as_vec(
					make_typename(
						NodeStructs::BaseTypename{ "a" }, NodeStructs::Value{}, tests_rule_info_stub()
					),
					make_typename(
						NodeStructs::BaseTypename{ "b" }, NodeStructs::Value{}, tests_rule_info_stub()
					)
				)
			}, NodeStructs::Value{}, tests_rule_info_stub()
		));

	ok &= test_structurize_equals<Typename>(__LINE__, 0, "a | b | c",
		make_typename(
			NodeStructs::UnionTypename{
				as_vec(
					make_typename(NodeStructs::BaseTypename{ "a" }, NodeStructs::Value{}, tests_rule_info_stub()),
					make_typename(NodeStructs::BaseTypename{ "b" }, NodeStructs::Value{}, tests_rule_info_stub()),
					make_typename(NodeStructs::BaseTypename{ "c" }, NodeStructs::Value{}, tests_rule_info_stub())
				)
			}, NodeStructs::Value{}, tests_rule_info_stub()
		));

	ok &= test_structurize_equals<Expression>(__LINE__, 0, "a.b.c",
		make_expression(
			NodeStructs::PropertyAccessExpression{
				make_expression(NodeStructs::PropertyAccessExpression{
					make_expression(std::string{ "a" }, tests_rule_info_stub()),
					"b"
				}, tests_rule_info_stub())
			, "c"
			}, tests_rule_info_stub()));

	ok &= test_structurize_equals<Expression>(__LINE__, 0, "filesystem::entries(\"C:/\")",
		make_expression(
			NodeStructs::CallExpression{
				.operand = make_expression(
					NodeStructs::NamespaceExpression{
						.name_space = make_expression(std::string{ "filesystem" }, tests_rule_info_stub()),
						.name_in_name_space = "entries"
					}, tests_rule_info_stub()
				),
				.arguments = { as_vec(NodeStructs::FunctionArgument{ std::nullopt, make_expression(str_parse("\"C:/\""), tests_rule_info_stub()) }) }
			}, tests_rule_info_stub()
		));

	ok &= test_structurize_equals<Template>(__LINE__, 0, "template f<T>:\n\tInt f(Vector<`T`> ref vec):\n\t\treturn 0",
		NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = as_vec(
				Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>{
					NodeStructs::TemplateParameter{.name = "T" }
				}),
			.templated = "\tInt f(Vector<`T`> ref vec):\n\t\treturn 0",
			.indent = 1,
			.info = tests_rule_info_stub()
		});

	ok &= test_structurize_equals<Function>(__LINE__, 0, "A f(A ref a):\n\tprintln(a)\n",
		NodeStructs::Function{
			.name = "f",
			.name_space = std::nullopt,
			.returnType = make_typename(NodeStructs::BaseTypename{ "A" }, NodeStructs::Value{}, tests_rule_info_stub()),
			.parameters = as_vec(NodeStructs::FunctionParameter{
				.typename_ = make_typename(NodeStructs::BaseTypename{ "A" }, NodeStructs::Reference{}, tests_rule_info_stub()),
				.name = "a",
			}),
			.statements = as_vec(NodeStructs::Statement<function_context>{ Variant<NodeStructs::RunTimeStatement>{ NodeStructs::RunTimeStatement {
				make_expression(
					NodeStructs::CallExpression{
						.operand = make_expression(std::string{ "println" }, tests_rule_info_stub()),
						.arguments = NodeStructs::ParenArguments{
							.args = as_vec(NodeStructs::FunctionArgument{
								.category = std::nullopt,
								.expr = make_expression(std::string{ "a" }, tests_rule_info_stub())
							})
						}
					}, tests_rule_info_stub())
			} } })
		});

	ok &= test_structurize_equals<TypenameOrExpression>(__LINE__, 0, "x",
		NodeStructs::WordTypenameOrExpression{
			.value = { std::string{ "x" } }
		});

	ok &= test_structurize_equals<TypenameOrExpression>(__LINE__, 0, "x.x",
		NodeStructs::WordTypenameOrExpression{
			.value = make_expression(NodeStructs::PropertyAccessExpression{ make_expression(std::string{ "x" }, tests_rule_info_stub()),  std::string{"x"} }, tests_rule_info_stub())
		});

	ok &= test_structurize_equals<TypenameOrExpression>(__LINE__, 0, "x::x",
		NodeStructs::WordTypenameOrExpression{
			.value = make_typename(
				NodeStructs::NamespacedTypename{
					make_typename(NodeStructs::BaseTypename{ "x" }, NodeStructs::Value{}, tests_rule_info_stub()),
					std::string{"x"}
				},
				NodeStructs::Value{}, tests_rule_info_stub()
			)
		});

	return ok;
}
