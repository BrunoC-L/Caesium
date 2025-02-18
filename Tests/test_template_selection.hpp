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
		transpilation_state state(Namespace{ .info = tests_rule_info_stub() });
		state.global_namespace.types.insert({ "SomeType", {} });
		state.global_namespace.types.at("SomeType").push_back(NodeStructs::Type{ .name = "SomeType", .info = tests_rule_info_stub() });
		variables_t variables;
		auto res = find_best_template(
			{ state, 0 },
			variables,
			templates,
			as_vec(
				NodeStructs::WordTypenameOrExpression{ { "SomeType" } }
			)
		);
		if (res.has_error() || cmp(res.value().tmpl.get(), templates.at(0)) != std::strong_ordering::equivalent)
			return false;
	}
	{
		auto templates = as_vec(NodeStructs::Template{
				.name = "f1",
				.name_space = std::nullopt,
				.parameters = as_vec(Parameter{
					NodeStructs::TemplateParameter{.name = "A" }
				}),
				.templated = "\nInt f1():\n",
				.info = tests_rule_info_stub()
			},
			NodeStructs::Template{
				.name = "f2",
				.name_space = std::nullopt,
				.parameters = as_vec(Parameter{
					NodeStructs::TemplateParameter{.name = "A" }
					}),
				.templated = "\nInt f2():\n",
				.indent = 1,
				.info = tests_rule_info_stub()
			});
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		transpilation_state state(Namespace{ .info = tests_rule_info_stub() });
		state.global_namespace.types.insert({ "SomeType", {} });
		state.global_namespace.types.at("SomeType").push_back(NodeStructs::Type{ .name = "SomeType", .info = tests_rule_info_stub() });
		variables_t variables;
		/*variables.insert({ "x", {}}); // todo maybe try non type args
		variables.at("x").push_back(variable_info{
			.value_category = NodeStructs::Value{},
			.type = NodeStructs::CompileTimeType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::Valued<int>{ 0 } } },
		});*/
		auto res = find_best_template(
			{ state, 0 },
			variables,
			templates,
			as_vec(NodeStructs::WordTypenameOrExpression{ { "SomeType" } })
		);
		if (!res.has_error())
			return false; // expect conflict between f1 and f2 they both work
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
		transpilation_state state(Namespace{ .info = tests_rule_info_stub() });
		state.global_namespace.types.insert({ "SomeType", {} });
		state.global_namespace.types.at("SomeType").push_back(NodeStructs::Type{ .name = "SomeType", .info = tests_rule_info_stub() });
		variables_t variables;
		auto res = find_best_template(
			{ state, 0 },
			variables,
			templates,
			as_vec(NodeStructs::WordTypenameOrExpression{ { "SomeType" } })
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
					NodeStructs::TemplateParameterWithDefaultValue{.name = "A", .value = NodeStructs::WordTypenameOrExpression{ { "SomeType" } } }
				}),
			.templated = "\nInt f():\n",
			.indent = 1,
			.info = tests_rule_info_stub()
		});
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		transpilation_state state(Namespace{ .info = tests_rule_info_stub() });
		state.global_namespace.types.insert({ "SomeType", {} });
		state.global_namespace.types.at("SomeType").push_back(NodeStructs::Type{ .name = "SomeType", .info = tests_rule_info_stub() });
		variables_t variables;
		auto res = find_best_template(
			{ state, 0 },
			variables,
			templates,
			as_vec(NodeStructs::WordTypenameOrExpression{ { "SomeType" } })
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
		transpilation_state state(Namespace{ .info = tests_rule_info_stub() });
		state.global_namespace.types.insert({ "SomeType", {} });
		state.global_namespace.types.at("SomeType").push_back(NodeStructs::Type{ .name = "SomeType", .info = tests_rule_info_stub() });
		variables_t variables;
		auto res = find_best_template(
			{ state, 0 },
			variables,
			templates,
			as_vec(NodeStructs::WordTypenameOrExpression{ { "SomeType" } })
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
					NodeStructs::TemplateParameterWithDefaultValue{.name = "B", .value = NodeStructs::WordTypenameOrExpression{ { "SomeType" } } }
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
		transpilation_state state(Namespace{ .info = tests_rule_info_stub() });
		state.global_namespace.types.insert({ "SomeType", {} });
		state.global_namespace.types.at("SomeType").push_back(NodeStructs::Type{ .name = "SomeType", .info = tests_rule_info_stub() });
		variables_t variables;
		auto res = find_best_template(
			{ state, 0 },
			variables,
			templates,
			as_vec(NodeStructs::WordTypenameOrExpression{ { "SomeType" } })
		);
		if (res.has_error() || cmp(res.value().tmpl.get(), templates.at(1)) != std::strong_ordering::equivalent)
			return false;
	}
	{
		auto templates = as_vec(NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = as_vec(Parameter{
				NodeStructs::TemplateParameterWithDefaultValue{.name = "A", .value = NodeStructs::WordTypenameOrExpression{ { "SomeType" } } },
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
						NodeStructs::TemplateParameterWithDefaultValue{.name = "B", .value = NodeStructs::WordTypenameOrExpression{ { "SomeType" } } }
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
		transpilation_state state(Namespace{ .info = tests_rule_info_stub() });
		state.global_namespace.types.insert({ "SomeType", {} });
		state.global_namespace.types.at("SomeType").push_back(NodeStructs::Type{ .name = "SomeType", .info = tests_rule_info_stub() });
		variables_t variables;
		auto res = find_best_template(
			{ state, 0 },
			variables,
			templates,
			as_vec(NodeStructs::WordTypenameOrExpression{ { "SomeType" } })
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
					Parameter{ NodeStructs::TemplateParameterWithDefaultValue{.name = "A", .value = "Int" } },
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
					Parameter{ NodeStructs::TemplateParameterWithDefaultValue{.name = "B", .value = "Int" } }
				),
				.templated = "\nInt f():\n",
				.indent = 1,
				.info = tests_rule_info_stub()
			}
		);
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		transpilation_state state(Namespace{ .info = tests_rule_info_stub() });
		variables_t variables;
		auto res = find_best_template(
			{ state, 0 },
			variables,
			templates,
			as_vec(NodeStructs::WordTypenameOrExpression{ { "Int" } }, NodeStructs::WordTypenameOrExpression{ { "Int" } })
		);
		if (!res.has_error())
			return false;
	}

	return true;
}

bool test_template_selection() {
	return test_single_argument() && test_full_variadic_vs_split() && test_ambiguous();
}
