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
			.indent = 1
		});
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		auto res = find_best_template(
			templates,
			as_vec(
				NodeStructs::WordTypenameOrExpression{ { "arg" } }
			)
		);
		if (res.has_error() || cmp(res.value().tmpl.get(), templates.at(0)) != std::weak_ordering::equivalent)
			return false;
	}
	{
		auto templates = as_vec(NodeStructs::Template{
			.name = "f1",
			.name_space = std::nullopt,
			.parameters = as_vec(Parameter{
				NodeStructs::TemplateParameter{.name = "A" }
			}),
			.templated = "\nInt f1():\n"
			},
			NodeStructs::Template{
				.name = "f2",
				.name_space = std::nullopt,
				.parameters = as_vec(Parameter{
					NodeStructs::TemplateParameter{.name = "A" }
					}),
				.templated = "\nInt f2():\n",
				.indent = 1
			});
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		auto res = find_best_template(
			templates,
			as_vec(NodeStructs::WordTypenameOrExpression{ { "arg" } })
		);
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
			.indent = 1
		});
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		auto res = find_best_template(
			templates,
			as_vec(NodeStructs::WordTypenameOrExpression{ { "arg" } })
		);
		if (res.has_error() || cmp(res.value().tmpl.get(), templates.at(0)) != std::weak_ordering::equivalent)
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
			.indent = 1
			},
			NodeStructs::Template{
				.name = "f",
				.name_space = std::nullopt,
				.parameters = as_vec(Parameter{
					NodeStructs::TemplateParameterWithDefaultValue{.name = "A", .value = NodeStructs::WordTypenameOrExpression{ { "arg" } } }
				}),
			.templated = "\nInt f():\n",
			.indent = 1
		});
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		auto res = find_best_template(
			templates,
			as_vec(NodeStructs::WordTypenameOrExpression{ { "arg" } })
		);
		if (res.has_error() || cmp(res.value().tmpl.get(), templates.at(1)) != std::weak_ordering::equivalent)
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
			.indent = 1
			},
			NodeStructs::Template{
				.name = "f",
				.name_space = std::nullopt,
				.parameters = as_vec(Parameter{
					NodeStructs::TemplateParameter{.name = "A" }
					}),
			.templated = "\nInt f():\n",
			.indent = 1
			}
		);
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		auto res = find_best_template(
			templates,
			as_vec(NodeStructs::WordTypenameOrExpression{ { "arg" } })
		);
		if (res.has_error() || cmp(res.value().tmpl.get(), templates.at(1)) != std::weak_ordering::equivalent)
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
			.indent = 1
		},
		NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = as_vec(
				Parameter{
					NodeStructs::VariadicTemplateParameter{.name = "As" }
				},
				Parameter{
					NodeStructs::TemplateParameterWithDefaultValue{.name = "B", .value = NodeStructs::WordTypenameOrExpression{ { "arg" } } }
				},
				Parameter{
					NodeStructs::VariadicTemplateParameter{.name = "Cs" }
				}
			),
			.templated = "\nInt f():\n",
			.indent = 1
			});
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		auto res = find_best_template(
			templates,
			as_vec(NodeStructs::WordTypenameOrExpression{ { "arg" } })
		);
		if (res.has_error() || cmp(res.value().tmpl.get(), templates.at(1)) != std::weak_ordering::equivalent)
			return false;
	}
	{
		auto templates = as_vec(NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = as_vec(Parameter{
				NodeStructs::TemplateParameterWithDefaultValue{.name = "A", .value = NodeStructs::WordTypenameOrExpression{ { "arg" } } },
				}),
			.templated = "\nInt f():\n",
			.indent = 1
			},
			NodeStructs::Template{
				.name = "f",
				.name_space = std::nullopt,
				.parameters = as_vec(
					Parameter{
						NodeStructs::VariadicTemplateParameter{.name = "As" }
					},
					Parameter{
						NodeStructs::TemplateParameterWithDefaultValue{.name = "B", .value = NodeStructs::WordTypenameOrExpression{ { "arg" } } }
					},
					Parameter{
						NodeStructs::VariadicTemplateParameter{.name = "Cs" }
					}),
				.templated = "\nInt f():\n",
				.indent = 1
			}
		);
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		auto res = find_best_template(
			templates,
			as_vec(NodeStructs::WordTypenameOrExpression{ { "arg" } })
		);
		if (res.has_error() || cmp(res.value().tmpl.get(), templates.at(0)) != std::weak_ordering::equivalent)
			return false;
	}
	return true;
}

bool test_template_selection() {
	return test_single_argument() && test_full_variadic_vs_split();
}
