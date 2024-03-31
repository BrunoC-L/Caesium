#pragma once
#include <iostream>
#include "core/toCpp.hpp"
#include "core/structurizer.hpp"
#include "utility/colored_text.hpp"
#include "core/structurizer.hpp"

template <bool exp_ok, typename T>
bool test_structurize(int line, int n_indent, std::string program, auto&& expected) {
	auto tokens = Tokenizer(program).read();
	tokens_and_iterator g{ tokens, tokens.begin() };
	T node(n_indent);
	bool nodeBuilt = build(node, g.it);

	bool programReadEntirely = g.it == g.tokens.end();
	while (!programReadEntirely && (g.it->first == NEWLINE || g.it->first == END))
		programReadEntirely = ++g.it == g.tokens.end();

	if (!nodeBuilt || !programReadEntirely) {
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
	using DT = std::remove_cvref_t<decltype(expected)>;
	DT structurized = [&]() -> DT {
			if constexpr (std::is_same_v<DT, NodeStructs::Expression>)
				return getExpressionStruct(node);
			else
				return getStruct(node);
		}();
	bool ok = expected <=> structurized == std::weak_ordering::equivalent;
	if (ok != exp_ok) {
		std::cout << "Comparison " << colored_text_with_bool(exp_ok ? "FAILED" : "WORKED", false) << " for LINE " << line << "\n" << program << "\n\n";
		auto it = g.tokens.begin();
		while (it != g.it) {
			std::cout << it->second << " ";
			++it;
		}
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
	tokens_and_iterator g{ tokens, tokens.begin() };
	Token<STRING> res{ 0 };
	build(res, g.it);
	return res;
}

bool test_structurize_equals() {
	using namespace grammar;
	bool ok = true;

	ok &= test_structurize_equals<Typename>(__LINE__, 0, "a::b::c",
		NodeStructs::Typename{
			NodeStructs::NamespacedTypename{
				NodeStructs::Typename{ NodeStructs::NamespacedTypename{
					NodeStructs::Typename{ NodeStructs::BaseTypename{ "a" } },
					"b"
				} },
				"c"
			}
		});

	ok &= test_structurize_equals<Typename>(__LINE__, 0, "a | b",
		NodeStructs::Typename{
			NodeStructs::UnionTypename{
				std::vector<NodeStructs::Typename>{ { NodeStructs::BaseTypename{ "a" } }, { NodeStructs::BaseTypename{ "b" } } }
			}
		});

	ok &= test_structurize_equals<Typename>(__LINE__, 0, "a | b | c",
		NodeStructs::Typename{
			NodeStructs::UnionTypename{
				std::vector<NodeStructs::Typename>{ { NodeStructs::BaseTypename{ "a" } }, { NodeStructs::BaseTypename{ "b" } }, { NodeStructs::BaseTypename{ "c" } } }
			}
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
				.arguments = { std::vector{ NodeStructs::FunctionArgument{ std::nullopt, { str_parse("\"C:/\"") } } } }
			}
		});

	ok &= test_structurize_equals<Template>(__LINE__, 0, "template f<T>:\n\tInt f(Vector<`T`> ref vec):\n\t\treturn 0",
		NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = { { NodeStructs::TemplateParameter{ .name = "T" } } },
			.templated = "\tInt f(Vector<`T`> ref vec):\n\t\treturn 0"
		});
	return ok;
}
