#pragma once
#include "core/toCPP.hpp"
#include "utility/as_vec.hpp"

using Parameter = Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>;

bool test_single_argument() {
	{
		auto templates = as_vec(NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = as_vec(Parameter{
				NodeStructs::TemplateParameter{.name = "A" }
			}),
			.templated = "\nInt f():\n",
			.indent = 1,
			.info = tests_rule_info_stub()
		});
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		transpilation_state state(NodeStructs::NameSpace{ .info = tests_rule_info_stub() });
		state.global_namespace.types.push_back(NodeStructs::Type{ .name = "SomeType", .info = tests_rule_info_stub() });
		variables_t variables;
		auto res = find_best_template(
			{ state, 0 },
			variables,
			templates,
			as_vec(
				NodeStructs::WordTypenameOrExpression{ std::string{ "SomeType" } }
			)
		);
		if (res.has_error() || cmp(res.value().tmpl.get(), templates.at(0)) != std::strong_ordering::equivalent)
			return false;
	}
	{
		auto template_ = NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = as_vec(Parameter{
				NodeStructs::TemplateParameter{.name = "A" }
			}),
			.templated = "\nVoid f():\n",
			.info = tests_rule_info_stub()
		};
		std::vector<NodeStructs::Template> conflicting_templates{};
		{
			conflicting_templates.push_back(copy(template_));
			conflicting_templates.push_back(copy(template_));
		}

		if (auto opt_e = validate_templates(conflicting_templates); opt_e.has_value())
			return false;
		transpilation_state state(NodeStructs::NameSpace{ .info = tests_rule_info_stub() });
		state.global_namespace.types.push_back(NodeStructs::Type{ .name = "SomeType", .info = tests_rule_info_stub() });
		variables_t variables;

		// todo maybe try non type args
				/*variables.insert({ "x", {}});
				variables.at("x").push_back(variable_info{
					.value_category = NodeStructs::Value{},
					.type = { Realised::CompileTimeType{ Realised::PrimitiveType{ Realised::PrimitiveType::Valued<int>{ 0 } } } },
				});*/

		auto res = find_best_template(
			{ state, 0 },
			variables,
			conflicting_templates,
			as_vec(NodeStructs::WordTypenameOrExpression{ std::string{ "SomeType" } })
		);

		// expect conflict between f1 and f2 they both work
		if (!res.has_error())
			return false;
	}
	{
		auto templates = as_vec(NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = as_vec(Parameter{
				NodeStructs::VariadicTemplateParameter{.name = "As" }
			}),
			.templated = "\nInt f():\n",
			.indent = 1,
			.info = tests_rule_info_stub()
			});
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		transpilation_state state(NodeStructs::NameSpace{ .info = tests_rule_info_stub() });
		state.global_namespace.types.push_back(NodeStructs::Type{ .name = "SomeType", .info = tests_rule_info_stub() });
		variables_t variables;
		auto res = find_best_template(
			{ state, 0 },
			variables,
			templates,
			as_vec(NodeStructs::WordTypenameOrExpression{ std::string{ "SomeType" } })
		);
		if (res.has_error() || cmp(res.value().tmpl.get(), templates.at(0)) != std::strong_ordering::equivalent)
			return false;
	}
	{
		auto templates = as_vec(NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = as_vec(Parameter{
				NodeStructs::VariadicTemplateParameter{.name = "As" }
				}),
			.templated = "\nInt f():\n",
			.indent = 1,
			.info = tests_rule_info_stub()
			},
			NodeStructs::Template{
				.name = "f",
				.name_space = std::nullopt,
				.parameters = as_vec(Parameter{
					NodeStructs::TemplateParameterWithDefaultValue{.name = "A", .value = NodeStructs::WordTypenameOrExpression{ std::string{ "SomeType" } } }
				}),
			.templated = "\nInt f():\n",
			.indent = 1,
			.info = tests_rule_info_stub()
			});
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		transpilation_state state(NodeStructs::NameSpace{ .info = tests_rule_info_stub() });
		state.global_namespace.types.push_back(NodeStructs::Type{ .name = "SomeType", .info = tests_rule_info_stub() });
		variables_t variables;
		auto res = find_best_template(
			{ state, 0 },
			variables,
			templates,
			as_vec(NodeStructs::WordTypenameOrExpression{ std::string{ "SomeType" } })
		);
		if (res.has_error() || cmp(res.value().tmpl.get(), templates.at(1)) != std::strong_ordering::equivalent)
			return false;
	}
	{
		auto templates = as_vec(NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = as_vec(Parameter{
				NodeStructs::VariadicTemplateParameter{.name = "As" }
			}),
			.templated = "\nInt f():\n",
			.indent = 1,
			.info = tests_rule_info_stub()
			},
			NodeStructs::Template{
				.name = "f",
				.name_space = std::nullopt,
				.parameters = as_vec(Parameter{
					NodeStructs::TemplateParameter{.name = "A" }
					}),
			.templated = "\nInt f():\n",
			.indent = 1,
			.info = tests_rule_info_stub()
			}
		);
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		transpilation_state state(NodeStructs::NameSpace{ .info = tests_rule_info_stub() });
		state.global_namespace.types.push_back(NodeStructs::Type{ .name = "SomeType", .info = tests_rule_info_stub() });
		variables_t variables;
		auto res = find_best_template(
			{ state, 0 },
			variables,
			templates,
			as_vec(NodeStructs::WordTypenameOrExpression{ std::string{ "SomeType" } })
		);
		if (res.has_error() || cmp(res.value().tmpl.get(), templates.at(1)) != std::strong_ordering::equivalent)
			return false;
	}
	return true;
}

bool test_full_variadic_vs_split() {
	{
		auto templates = as_vec(NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = as_vec(Parameter{
				NodeStructs::VariadicTemplateParameter{.name = "As" }
			}),
			.templated = "\nInt f():\n",
			.indent = 1,
			.info = tests_rule_info_stub()
			},
			NodeStructs::Template{
				.name = "f",
				.name_space = std::nullopt,
				.parameters = as_vec(
					Parameter{
						NodeStructs::VariadicTemplateParameter{.name = "As" }
					},
					Parameter{
						NodeStructs::TemplateParameterWithDefaultValue{.name = "B", .value = NodeStructs::WordTypenameOrExpression{ std::string{ "SomeType" } } }
					},
					Parameter{
						NodeStructs::VariadicTemplateParameter{.name = "Cs" }
					}
				),
				.templated = "\nInt f():\n",
				.indent = 1,
				.info = tests_rule_info_stub()
			});
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		transpilation_state state(NodeStructs::NameSpace{ .info = tests_rule_info_stub() });
		state.global_namespace.types.push_back(NodeStructs::Type{ .name = "SomeType", .info = tests_rule_info_stub() });
		variables_t variables;
		auto res = find_best_template(
			{ state, 0 },
			variables,
			templates,
			as_vec(NodeStructs::WordTypenameOrExpression{ std::string{ "SomeType" } })
		);
		if (res.has_error() || cmp(res.value().tmpl.get(), templates.at(1)) != std::strong_ordering::equivalent)
			return false;
	}
	{
		auto templates = as_vec(NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = as_vec(Parameter{
				NodeStructs::TemplateParameterWithDefaultValue{.name = "A", .value = NodeStructs::WordTypenameOrExpression{ std::string{ "SomeType" } } },
				}),
			.templated = "\nInt f():\n",
			.indent = 1,
			.info = tests_rule_info_stub()
			},
			NodeStructs::Template{
				.name = "f",
				.name_space = std::nullopt,
				.parameters = as_vec(
					Parameter{
						NodeStructs::VariadicTemplateParameter{.name = "As" }
					},
					Parameter{
						NodeStructs::TemplateParameterWithDefaultValue{.name = "B", .value = NodeStructs::WordTypenameOrExpression{ std::string{ "SomeType" } } }
					},
					Parameter{
						NodeStructs::VariadicTemplateParameter{.name = "Cs" }
					}),
				.templated = "\nInt f():\n",
				.indent = 1,
			.info = tests_rule_info_stub()
			}
		);
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		transpilation_state state(NodeStructs::NameSpace{ .info = tests_rule_info_stub() });
		state.global_namespace.types.push_back(NodeStructs::Type{ .name = "SomeType", .info = tests_rule_info_stub() });
		variables_t variables;
		auto res = find_best_template(
			{ state, 0 },
			variables,
			templates,
			as_vec(NodeStructs::WordTypenameOrExpression{ std::string{ "SomeType" } })
		);
		if (res.has_error() || cmp(res.value().tmpl.get(), templates.at(0)) != std::strong_ordering::equivalent)
			return false;
	}
	return true;
}

bool test_ambiguous() {
	{
		auto templates = as_vec(
			NodeStructs::Template{
				.name = "f",
				.name_space = std::nullopt,
				.parameters = as_vec(
					Parameter{ NodeStructs::TemplateParameter{.name = "A" } },
					Parameter{ NodeStructs::TemplateParameter{.name = "B" } }
				),
				.templated = "\nInt f():\n",
				.indent = 1,
				.info = tests_rule_info_stub()
			},
			NodeStructs::Template{
				.name = "f",
				.name_space = std::nullopt,
				.parameters = as_vec(
					Parameter{ NodeStructs::TemplateParameterWithDefaultValue{.name = "A", .value = { std::string{ "Int" } } } },
					Parameter{ NodeStructs::TemplateParameter{.name = "B" } }
				),
				.templated = "\nInt f():\n",
				.indent = 1,
				.info = tests_rule_info_stub()
			},
			NodeStructs::Template{
				.name = "f",
				.name_space = std::nullopt,
				.parameters = as_vec(
					Parameter{ NodeStructs::TemplateParameter{.name = "A" } },
					Parameter{ NodeStructs::TemplateParameterWithDefaultValue{.name = "B", .value = { std::string{ "Int" } } } }
				),
				.templated = "\nInt f():\n",
				.indent = 1,
				.info = tests_rule_info_stub()
			}
		);
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		transpilation_state state(NodeStructs::NameSpace{ .info = tests_rule_info_stub() });
		variables_t variables;
		auto res = find_best_template(
			{ state, 0 },
			variables,
			templates,
			as_vec(NodeStructs::WordTypenameOrExpression{ std::string{ "Int" } }, NodeStructs::WordTypenameOrExpression{ std::string{ "Int" } })
		);
		if (!res.has_error())
			return false;
	}

	return true;
}

bool test_template_selection() {
	return test_single_argument() && test_full_variadic_vs_split() && test_ambiguous();
}
