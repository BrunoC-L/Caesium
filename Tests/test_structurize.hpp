#pragma once
#include <iostream>
#include "core/toCpp.hpp"
#include "core/structurizer.hpp"
#include "utility/colored_text.hpp"
#include "utility/as_vec.hpp"

template <bool exp_ok, typename T>
bool test_structurize(int line, int n_indent, std::string program, auto&& expected) {
	auto tokens = Tokenizer(program).read();
	Iterator it = { tokens, 0 };
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
		auto index = it.index;
		while (index != index)
			std::cout << it.vec[index++].second << " ";
		std::cout << "\n";
		return false;
	}
	using DT = std::remove_cvref_t<decltype(expected)>;
	DT structurized = [&]() -> DT {
			if constexpr (std::is_same_v<DT, NodeStructs::Expression>)
				return getExpressionStruct(node);
			else if constexpr (std::is_same_v<DT, NodeStructs::Function>)
				return structurize_function(node, std::nullopt);
			else if constexpr (std::is_same_v<DT, NodeStructs::Typename>)
				return getStruct(node, tag_allow_value_category_or_empty{});
			else if constexpr (std::is_same_v<DT, NodeStructs::WordTypenameOrExpression>)
				return getStruct(node, tag_allow_value_category_or_empty{});
			else
				return getStruct(node);
		}();
	bool ok = expected <=> structurized == std::weak_ordering::equivalent;
	if (ok != exp_ok) {
		std::cout << "Comparison " << colored_text_with_bool(exp_ok ? "FAILED" : "WORKED", false) << " for LINE " << line << "\n" << program << "\n\n";
		auto index = it.index;
		while (index != index)
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
	Iterator it = { tokens, 0 };
	Token<STRING> res{ 0 };
	build(res, it);
	return res;
}

bool test_structurize_equals() {
	using namespace grammar;
	bool ok = true;

	ok &= test_structurize_equals<Typename>(__LINE__, 0, "a::b::c",
		NodeStructs::Typename{
			NodeStructs::NamespacedTypename{
				NodeStructs::Typename{ NodeStructs::NamespacedTypename{
					NodeStructs::Typename{ NodeStructs::BaseTypename{ "a" }, NodeStructs::Value{} },
					"b"
				}, NodeStructs::Value{} },
				"c"
			}, NodeStructs::Value{}
		});

	ok &= test_structurize_equals<Typename>(__LINE__, 0, "a | b",
		NodeStructs::Typename{
			NodeStructs::UnionTypename{
				as_vec(
					NodeStructs::Typename{
						NodeStructs::BaseTypename{ "a" }, NodeStructs::Value{}
					},
					NodeStructs::Typename{
						NodeStructs::BaseTypename{ "b" }, NodeStructs::Value{}
					}
				)
			}, NodeStructs::Value{}
		});

	ok &= test_structurize_equals<Typename>(__LINE__, 0, "a | b | c",
		NodeStructs::Typename{
			NodeStructs::UnionTypename{
				as_vec(NodeStructs::Typename{ NodeStructs::BaseTypename{ "a" }, NodeStructs::Value{} }, NodeStructs::Typename{ NodeStructs::BaseTypename{ "b" }, NodeStructs::Value{} }, NodeStructs::Typename{ NodeStructs::BaseTypename{ "c" }, NodeStructs::Value{} })
			}, NodeStructs::Value{}
		});

	ok &= test_structurize_equals<Expression>(__LINE__, 0, "a.b.c",
		NodeStructs::Expression{
			NodeStructs::PropertyAccessExpression{
				NodeStructs::Expression{ NodeStructs::PropertyAccessExpression{
					NodeStructs::Expression{ std::string{ "a" } },
					"b"
				} },
				"c"
			}
		});

	ok &= test_structurize_equals<Expression>(__LINE__, 0, "filesystem::entries(\"C:/\")",
		NodeStructs::Expression{
			NodeStructs::CallExpression{
				.operand = NodeStructs::Expression{
					NodeStructs::NamespaceExpression{
						.name_space = NodeStructs::Expression{ std::string{ "filesystem" } },
						.name_in_name_space = "entries"
					}
				},
				.arguments = { as_vec(NodeStructs::FunctionArgument{ std::nullopt, { str_parse("\"C:/\"") } }) }
			}
		});

	ok &= test_structurize_equals<Template>(__LINE__, 0, "template f<T>:\n\tInt f(Vector<`T`> ref vec):\n\t\treturn 0",
		NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = as_vec(
				Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>{
					NodeStructs::TemplateParameter{.name = "T" }
				}),
			.templated = "\tInt f(Vector<`T`> ref vec):\n\t\treturn 0",
			.indent = 1
		});

	ok &= test_structurize_equals<Function>(__LINE__, 0, "A f(A ref a):\n\tprintln(a)\n",
		NodeStructs::Function{
			.name = "f",
			.name_space = std::nullopt,
			.returnType = { NodeStructs::BaseTypename{ "A" }, NodeStructs::Value{} },
			.parameters = as_vec(NodeStructs::FunctionParameter{
				.typename_ = { NodeStructs::BaseTypename{ "A" }, NodeStructs::Reference{} },
				.name = "a",
			}),
			.statements = as_vec(NodeStructs::Statement{
				NodeStructs::Expression{
					NodeStructs::CallExpression{
						.operand = "println",
						.arguments = NodeStructs::ParenArguments{
							.args = as_vec(NodeStructs::FunctionArgument{
								.category = std::nullopt,
								.expr = "a"
							})
						}
					}
				}
			})
		});

	ok &= test_structurize_equals<TypenameOrExpression>(__LINE__, 0, "x",
		NodeStructs::WordTypenameOrExpression{
			.value = { std::string{ "x" } }
		});

	ok &= test_structurize_equals<TypenameOrExpression>(__LINE__, 0, "x.x",
		NodeStructs::WordTypenameOrExpression{
			.value = NodeStructs::Expression{ NodeStructs::PropertyAccessExpression{ { std::string{ "x" } },  std::string{"x"} } }
		});

	ok &= test_structurize_equals<TypenameOrExpression>(__LINE__, 0, "x::x",
		NodeStructs::WordTypenameOrExpression{
			.value = NodeStructs::Typename{
				NodeStructs::NamespacedTypename{
					NodeStructs::Typename{ NodeStructs::BaseTypename{ std::string{ "x" } }, NodeStructs::Value{} },
					std::string{"x"}
				},
				NodeStructs::Value{}
			}
		});

	return ok;
}
