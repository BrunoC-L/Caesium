#pragma once
#include "core/toCPP.hpp"
#include "utility//as_vec.hpp"

bool test_single_argument() {
	{
		auto templates = as_vec(NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = as_vec(Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>{
				NodeStructs::TemplateParameter{.name = "A" }
			}),
			.templated = "\nInt f():\n"
		});
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		auto res = find_best_template(
			templates,
			as_vec(
				NodeStructs::Expression{ "arg" }
			)
		);
		if (res.has_error() || res.value().tmpl.get() <=> templates.at(0) != std::weak_ordering::equivalent)
			return false;
	}
	{
		auto templates = as_vec(NodeStructs::Template{
			.name = "f1",
			.name_space = std::nullopt,
			.parameters = as_vec(Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>{
				NodeStructs::TemplateParameter{.name = "A" }
			}),
			.templated = "\nInt f1():\n"
			},
			NodeStructs::Template{
				.name = "f2",
				.name_space = std::nullopt,
				.parameters = as_vec(Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>{
					NodeStructs::TemplateParameter{.name = "A" }
					}),
				.templated = "\nInt f2():\n"
			});
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		auto res = find_best_template(
			templates,
			as_vec(NodeStructs::Expression{ "arg" })
		);
		if (!res.has_error())
			return false;
	}
	{
		auto templates = as_vec(NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = as_vec(Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>{
				NodeStructs::VariadicTemplateParameter{.name = "As" }
			}),
			.templated = "\nInt f():\n"
		});
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		auto res = find_best_template(
			templates,
			as_vec(NodeStructs::Expression{ "arg" })
		);
		if (res.has_error() || res.value().tmpl.get() <=> templates.at(0) != std::weak_ordering::equivalent)
			return false;
	}
	{
		auto templates = as_vec(NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = as_vec(Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>{
				NodeStructs::VariadicTemplateParameter{.name = "As" }
				}),
			.templated = "\nInt f():\n"
		},
		NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = as_vec(Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>{
				NodeStructs::TemplateParameterWithDefaultValue{.name = "A", .value = NodeStructs::Expression{ "arg" } }
				}),
			.templated = "\nInt f():\n"
		});
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		auto res = find_best_template(
			templates,
			as_vec(NodeStructs::Expression{ "arg" })
		);
		if (res.has_error() || res.value().tmpl.get() <=> templates.at(1) != std::weak_ordering::equivalent)
			return false;
	}
	{
		auto templates = as_vec(NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = as_vec(Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>{
				NodeStructs::VariadicTemplateParameter{.name = "As" }
				}),
		.templated = "\nInt f():\n"
			},
			NodeStructs::Template{
				.name = "f",
				.name_space = std::nullopt,
				.parameters = as_vec(Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>{
					NodeStructs::TemplateParameter{.name = "A" }
					}),
	.templated = "\nInt f():\n"
			});
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		auto res = find_best_template(
			templates,
			as_vec(NodeStructs::Expression{ "arg" })
		);
		if (res.has_error() || res.value().tmpl.get() <=> templates.at(1) != std::weak_ordering::equivalent)
			return false;
	}
	return true;
}

bool test_full_variadic_vs_split() {
	{
		auto templates = as_vec(NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = as_vec(Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>{
				NodeStructs::VariadicTemplateParameter{.name = "As" }
			}),
			.templated = "\nInt f():\n"
		},
		NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = as_vec(
				Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>{
					NodeStructs::VariadicTemplateParameter{.name = "As" }
				},
				Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>{
					NodeStructs::TemplateParameterWithDefaultValue{.name = "B", .value = NodeStructs::Expression{ "arg" } }
				},
				Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>{
					NodeStructs::VariadicTemplateParameter{.name = "Cs" }
				}
			),
			.templated = "\nInt f():\n"
		});
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		auto res = find_best_template(
			templates,
			as_vec(NodeStructs::Expression{ "arg" })
		);
		if (res.has_error() || res.value().tmpl.get() <=> templates.at(1) != std::weak_ordering::equivalent)
			return false;
	}
	{
		auto templates = as_vec(NodeStructs::Template{
			.name = "f",
			.name_space = std::nullopt,
			.parameters = as_vec(Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>{
				NodeStructs::TemplateParameterWithDefaultValue{.name = "A", .value = NodeStructs::Expression{ "arg" } },
				}),
		.templated = "\nInt f():\n"
			},
			NodeStructs::Template{
				.name = "f",
				.name_space = std::nullopt,
				.parameters = as_vec(
					Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>{
						NodeStructs::VariadicTemplateParameter{.name = "As" }
					},
					Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>{
						NodeStructs::TemplateParameterWithDefaultValue{.name = "B", .value = NodeStructs::Expression{ "arg" } }
					},
					Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>{
						NodeStructs::VariadicTemplateParameter{.name = "Cs" }
					}),
				.templated = "\nInt f():\n"
			});
		if (auto opt_e = validate_templates(templates); opt_e.has_value())
			return false;
		auto res = find_best_template(
			templates,
			as_vec(NodeStructs::Expression{ "arg" })
		);
		if (res.has_error() || res.value().tmpl.get() <=> templates.at(0) != std::weak_ordering::equivalent)
			return false;
	}
	return true;
}

bool test_template_selection() {
	return test_single_argument() && test_full_variadic_vs_split();
}
