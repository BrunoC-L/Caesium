#include "type_of_expression_visitor.hpp"
#include "../core/structurizer.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"
#include "../utility/replace_all.hpp"

using T = type_of_expression_visitor;
using R = T::R;

//R T::operator()(const NodeStructs::AssignmentExpression& expr) {
//	throw;
//}

R T::operator()(const NodeStructs::ConditionalExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::OrExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::AndExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::EqualityExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::CompareExpression& expr) {
	return R::value_type{ NodeStructs::Value{}, std::reference_wrapper{ *state.state.named.types.at("Bool").at(0) } };
}

R T::operator()(const NodeStructs::AdditiveExpression& expr) {
	auto type_of_first_term = operator()(expr.expr);
	return_if_error(type_of_first_term);
	return std::pair{
		NodeStructs::Value{}, type_of_first_term.value().second
	};
}

R T::operator()(const NodeStructs::MultiplicativeExpression& expr) {
	auto type_of_first_term = operator()(expr.expr);
	return_if_error(type_of_first_term);
	return std::pair{
		NodeStructs::Value{}, type_of_first_term.value().second
	};
}

R T::operator()(const NodeStructs::UnaryExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::CallExpression& expr) {
	auto operand_t = transpile_expression(state, expr.operand).transform([](auto&& x) { return std::pair{ std::move(x).value_category, std::move(x).type }; });
	//auto operand_t = type_of_expression(state, expr.operand);
	return_if_error(operand_t);
	if (std::holds_alternative<NodeStructs::FunctionType>(operand_t.value().second.value)) {
		throw;
	}
	throw;
}

bool has_argument_and_forced_parameter_mismatch(const std::vector<NodeStructs::Expression>& args, const NodeStructs::Template& tmpl) {
	auto n_parameters = args.size();
	for (auto i = 0; i < n_parameters; ++i)
		if (tmpl.parameters.at(i).second.has_value() && (args.at(i) <=> tmpl.parameters.at(i).second.value() != std::strong_ordering::equal))
			return true;
	return false;
}

auto eliminate_argument_and_forced_parameter_mismatches(const std::vector<NodeStructs::Expression>& args, std::vector<NodeStructs::Template const*>&& templates) {
	for (int i = templates.size() - 1; i >= 0; --i)
		if (has_argument_and_forced_parameter_mismatch(args, *templates.at(i)))
			templates.erase(templates.begin() + i);
	return std::move(templates);
}

bool is_redundant_wrt_to(const NodeStructs::Template& tmpl, const NodeStructs::Template& reference) {
	auto n_parameters = tmpl.parameters.size();
	for (auto i = 0; i < n_parameters; ++i)
		if (!reference.parameters.at(i).second.has_value() && tmpl.parameters.at(i).second.has_value())
			return false;
	return true;
}

auto eliminate_underspecified(const std::vector<NodeStructs::Expression>& args, std::vector<NodeStructs::Template const*>&& templates) {
	auto mask = std::vector<bool>(templates.size(), false);
	for (int i = 0; i < templates.size(); ++i)
		for (int j = 0; j < templates.size(); ++j)
			if (i != j && mask[j] == false && is_redundant_wrt_to(*templates.at(i), *templates.at(j))) {
				mask[i] = true;
				break;
			}
	unsigned index = 0;
	templates.erase(std::remove_if(templates.begin(), templates.end(), [&](NodeStructs::Template const* t) {
		return bool(mask.at(index++));
	}), templates.end());
	return std::move(templates);
}

expected<std::reference_wrapper<const NodeStructs::Template>> find_best_template(
	const std::vector<NodeStructs::Template const*>& templates,
	const std::vector<NodeStructs::Expression>& args
) {
	auto n_parameters = args.size();
	for (unsigned i = 0; i < n_parameters; ++i) {
		const auto& parameter_name = templates.at(0)->parameters.at(i).first;
		for (auto const* a : templates)
			if (a->parameters.size() != n_parameters || parameter_name != a->parameters.at(i).first)
				throw;
	}

	auto candidates = templates;
	candidates = eliminate_argument_and_forced_parameter_mismatches(args, std::move(candidates));
	auto candidates2 = candidates;
	candidates = eliminate_underspecified(args, std::move(candidates));

	if (candidates.size() == 0)
		throw;
	else if (candidates.size() == 1)
		return std::reference_wrapper<const NodeStructs::Template>{ *candidates.at(0) };
	else {
		std::stringstream args_ss;
		bool has_prev_args = false;
		for (const auto& arg : args) {
			if (has_prev_args)
				args_ss << ", ";
			has_prev_args = true;
			args_ss << expression_for_template(arg);
		}
		std::stringstream templates_ss;
		bool has_prev_templates = false;
		for (auto const* a : candidates) {
			if (has_prev_templates)
				templates_ss << ", ";
			has_prev_templates = true;
			std::stringstream parameters_ss;
			bool has_prev_parameters = false;
			for (const auto& parameter : a->parameters) {
				if (has_prev_parameters)
					parameters_ss << ", ";
				has_prev_parameters = true;
				parameters_ss << parameter.first;
				if (parameter.second.has_value())
					parameters_ss << " = " << expression_for_template(parameter.second.value());
			}
			templates_ss << "template " + templates.at(0)->name + "<" + parameters_ss.str() + ">";
		}
		return error{
			"user error",
			"More than one instance of `" + templates.at(0)->name + "`"
			" matched the provided arguments [" + args_ss.str() + "]"
			", contenders were [" + templates_ss.str() + "]"
		};
	}
}

R T::operator()(const NodeStructs::TemplateExpression& expr) {
	if (!std::holds_alternative<std::string>(expr.operand.expression.get()))
		throw;

	if (auto it = state.state.named.templates.find(std::get<std::string>(expr.operand.expression.get())); it != state.state.named.templates.end()) {
		auto t = find_best_template(it->second, expr.arguments.args);
		return_if_error(t);
		const auto& tmpl = t.value().get();
		std::vector<std::string> args = expr.arguments.args | LIFT_TRANSFORM(expression_for_template) | to_vec();
		std::string tmpl_name = template_name(it->first, args);

		size_t max_params = it->second.at(0)->parameters.size();
		for (unsigned i = 1; i < it->second.size(); ++i)
			max_params = std::max(max_params, it->second.at(i)->parameters.size());

		if (tmpl.parameters.size() != expr.arguments.args.size()) {
			std::stringstream ss;
			ss << "invalid number of arguments to template `"
				<< tmpl.name
				<< "`, expected `"
				<< tmpl.parameters.size()
				<< "`, received `"
				<< expr.arguments.args.size()
				<< "`";
			return error{ "user error", ss.str() };
		}

		if (tmpl.templated == "BUILTIN") {
			/*if (tmpl.name == "Vector") {
				auto x = type_of_typename(state, templated_with.at(0));
				return_if_error(x);
				return NodeStructs::UniversalType{ NodeStructs::VectorType{ Box<NodeStructs::UniversalType>{ std::move(x).value() } } };
			}
			if (tmpl.name == "Set") {
				auto x = type_of_typename(state, templated_with.at(0));
				return_if_error(x);
				return NodeStructs::UniversalType{ NodeStructs::SetType{ Box<NodeStructs::UniversalType>{ std::move(x).value() } } };
			}
			if (tmpl.name == "Map") {
				auto x = type_of_typename(state, templated_with.at(0));
				auto y = type_of_typename(state, templated_with.at(1));
				return_if_error(x);
				return_if_error(y);
				return NodeStructs::UniversalType{ NodeStructs::MapType{
					Box<NodeStructs::UniversalType>{ std::move(x).value() },
					Box<NodeStructs::UniversalType>{ std::move(y).value() }
				} };
			}*/
			throw;
		}

		std::string replaced = tmpl.templated;
		for (int i = 0; i < expr.arguments.args.size(); ++i)
			replaced = replace_all(std::move(replaced), tmpl.parameters.at(i).first, args.at(i));

		if (auto it = state.state.named.functions.find(tmpl_name); it != state.state.named.functions.end()) {
			return std::pair{ NodeStructs::Reference{}, NodeStructs::UniversalType{ NodeStructs::FunctionType{ *it->second.back() } } };
		}

		if (auto it = state.state.named.types.find(tmpl_name); it != state.state.named.types.end()) {
			return std::pair{ NodeStructs::Reference{}, NodeStructs::UniversalType{ NodeStructs::TypeType{ *it->second.back() } } };
		}

		{
			And<IndentToken, Function, Token<END>> f{ 1 };
			auto tokens = Tokenizer(replaced).read();
			tokens_and_iterator g{ tokens, tokens.begin() };
			if (!f.build(g.it))
				throw;
			auto* structured_f = new NodeStructs::Function{ getStruct(f.get<Function>()) };
			structured_f->name = tmpl_name;
			state.state.named.functions[structured_f->name].push_back(structured_f);
			state.state.traversed_functions.insert(*structured_f);
			auto transpiled_or_e = transpile(state, *structured_f);
			return_if_error(transpiled_or_e);
			state.state.transpile_in_reverse_order.push_back(std::move(transpiled_or_e).value());
			return std::pair{ NodeStructs::Reference{}, NodeStructs::UniversalType{ NodeStructs::FunctionType{*structured_f}} };
		}
		{
			And<IndentToken, Type, Token<END>> t{ 1 };
			auto tokens = Tokenizer(replaced).read();
			tokens_and_iterator g{ tokens, tokens.begin() };
			if (!t.build(g.it))
				throw;
			auto* structured_t = new NodeStructs::Type{ getStruct(t.get<Type>()) };
			structured_t->name = tmpl_name;
			state.state.named.types[structured_t->name].push_back(structured_t);
			auto opt_error = traverse_type(state, { *structured_t });
			if (opt_error.has_value())
				return opt_error.value();
			return std::pair{ NodeStructs::Reference{}, NodeStructs::UniversalType{ std::reference_wrapper{*structured_t}} };
		}
		throw;
	}
}

R T::operator()(const NodeStructs::ConstructExpression& expr) {
	auto operand_t = operator()(expr.operand);
	return_if_error(operand_t);
	if (std::holds_alternative<NodeStructs::TypeType>(operand_t.value().second.value)) {
		return std::pair{
			NodeStructs::Reference{},
			NodeStructs::UniversalType{
				std::reference_wrapper {
				std::get<NodeStructs::TypeType>(operand_t.value().second.value).type
				}
			}
		};
	}
}

R T::operator()(const NodeStructs::BracketAccessExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::PropertyAccessExpression& expr) {
	auto operand_t = operator()(expr.operand);
	return_if_error(operand_t);
	auto t = type_of_postfix_member(state, expr.property_name, operand_t.value().second);
	return_if_error(t);
	return std::pair{ operand_t.value().first, std::move(t).value().second };
}

R T::operator()(const NodeStructs::ParenArguments& expr) {
	if (expr.args.size() == 0)
		throw;
	if (expr.args.size() == 1)
		return transpile_expression(state, std::get<NodeStructs::Expression>(expr.args.at(0)))
			.transform([](auto&& x) { return std::pair{ std::move(x).value_category, std::move(x).type }; })
			.transform([&](auto&& t) -> R::value_type { return R::value_type{
				argument_category_optional_to_value_category(std::get<0>(expr.args.at(0))), std::move(t).second.value };
			});
	throw;
}

R T::operator()(const NodeStructs::BraceArguments& expr) {
	// man this sucks
	std::vector<std::pair<NodeStructs::ArgumentCategory, NodeStructs::UniversalType>> vec;
	vec.reserve(expr.args.size());
	for (const auto& [arg_cat, arg] : expr.args) {
		auto type_of_expression = operator()(arg);
		return_if_error(type_of_expression);
		if (!arg_cat.has_value())
			throw;
		vec.push_back(std::pair{ arg_cat.value(), std::move(type_of_expression).value().second });
	}
	return std::pair{
		NodeStructs::Reference{},
		NodeStructs::UniversalType{ NodeStructs::AggregateType{ std::move(vec) } }
	};
}

R T::operator()(const std::string& expr) {
	if (auto it = state.state.named.templates.find(expr); it != state.state.named.templates.end())
		return std::pair{
			NodeStructs::Reference{}, // dummy arg
			NodeStructs::UniversalType{ *it->second.back() }
	};

	if (auto it = state.state.variables.find(expr); it != state.state.variables.end())
		return it->second.back();

	if (auto it = state.state.named.types.find(expr); it != state.state.named.types.end())
		return std::pair{
			NodeStructs::Reference{}, // dummy arg
			NodeStructs::UniversalType{ NodeStructs::TypeType{ *it->second.back() } }
		};

	if (auto it = state.state.named.functions.find(expr); it != state.state.named.functions.end())
		return std::pair{
			NodeStructs::Reference{}, // dummy arg
			NodeStructs::UniversalType{ NodeStructs::FunctionType{ *it->second.back() } }
		};

	if (auto it = state.state.named.type_aliases.find(expr); it != state.state.named.type_aliases.end())
		return std::pair{
			NodeStructs::Reference{}, // dummy arg
			it->second
		};


	return error{ "user error", "could not find variable named `" + expr + "`"};
}

R T::operator()(const Token<INTEGER_NUMBER>& expr) {
	return std::pair{
		NodeStructs::Reference{},
		NodeStructs::UniversalType{ stoi(expr.value) }
	};
}

R T::operator()(const Token<FLOATING_POINT_NUMBER>& expr) {
	return std::pair{
		NodeStructs::Reference{},
		NodeStructs::UniversalType{ stod(expr.value) }
	};
}

R T::operator()(const Token<STRING>& expr) {
	throw;
}
