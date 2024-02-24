#include "../core/toCPP.hpp"
//#include "transpile_expression_visitor.hpp"
#include "../utility/replace_all.hpp"
#include "../core/structurizer.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"

#include <algorithm>

using T = transpile_expression_visitor;
using R = T::R;

//R T::operator()(const NodeStructs::AssignmentExpression& expr) {
//	std::stringstream ss;
//	ss << operator()(expr.expr).value();
//	for (const auto& e : expr.assignments)
//		ss << " " << symbol_variant_as_text(e.first) << " " << operator()(e.second).value();
//	return ss.str();
//}

R T::operator()(const NodeStructs::ConditionalExpression& expr) {
	if (expr.ifElseExprs.has_value()) {
		// x if first else second
		//                  ([&] () { if (first) return x; else return second; }());

		auto condition_expr_info = operator()(expr.ifElseExprs.value().first);
		return_if_error(condition_expr_info);

		auto if_expr_info = operator()(expr.expr);
		return_if_error(if_expr_info);

		auto else_expr_info = operator()(expr.ifElseExprs.value().second);
		return_if_error(else_expr_info);

		return whole_expression_information{
			.expression = expr,
			.value_category = if_expr_info.value().value_category,
			.type = if_expr_info.value().type,
			.representation= std::string("([&] () { if (") +
			condition_expr_info.value().representation +
			") return " +
			if_expr_info.value().representation +
			"; else return " +
			else_expr_info.value().representation +
			"; }())"
		};
	}
	else
		return operator()(expr.expr);
}

R T::operator()(const NodeStructs::OrExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	ss << base.value().representation;
	for (const auto& e : expr.ors) {
		auto repr = operator()(e);
		return_if_error(repr);
		ss << " || " << repr.value().representation;
	}
	return whole_expression_information{
		.expression = expr,
		.value_category = NodeStructs::Value{},
		.type = *state.state.named.types.at("Bool").back(),
		.representation = ss.str()
	};
}

R T::operator()(const NodeStructs::AndExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	ss << base.value().representation;
	for (const auto& e : expr.ands) {
		auto repr = operator()(e);
		return_if_error(repr);
		ss << " && " << repr.value().representation;
	}
	return whole_expression_information{
		.expression = expr,
		.value_category = NodeStructs::Value{},
		.type = *state.state.named.types.at("Bool").back(),
		.representation = ss.str()
	};
}

R T::operator()(const NodeStructs::EqualityExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	ss << base.value().representation;
	for (const auto& [op, e] : expr.equals) {
		auto repr = operator()(e);
		return_if_error(repr);
		ss << " " << symbol_variant_as_text(op) << " " << repr.value().representation;
	}
	return whole_expression_information{
		.expression = expr,
		.value_category = NodeStructs::Value{},
		.type = *state.state.named.types.at("Bool").back(),
		.representation = ss.str()
	};
}

R T::operator()(const NodeStructs::CompareExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	ss << base.value().representation;
	for (const auto& [op, e] : expr.comparisons) {
		auto repr = operator()(e);
		return_if_error(repr);
		ss << " " << symbol_variant_as_text(op) << " " << repr.value().representation;
	}
	return whole_expression_information{
		.expression = expr,
		.value_category = NodeStructs::Value{},
		.type = *state.state.named.types.at("Bool").back(),
		.representation = ss.str()
	};
}

R T::operator()(const NodeStructs::AdditiveExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	ss << base.value().representation;
	for (const auto& [op, e] : expr.adds) {
		auto repr = operator()(e);
		return_if_error(repr);
		ss << " " << symbol_variant_as_text(op) << " " << repr.value().representation;
	}
	return whole_expression_information{
		.expression = expr,
		.value_category = NodeStructs::Value{},
		.type = *state.state.named.types.at("Int").back(),
		.representation = ss.str()
	};
}

R T::operator()(const NodeStructs::MultiplicativeExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	ss << base.value().representation;
	for (const auto& [op, e] : expr.muls) {
		auto repr = operator()(e);
		return_if_error(repr);
		ss << " " << symbol_variant_as_text(op) << " " << repr.value().representation;
	}
	return whole_expression_information{
		.expression = expr,
		.value_category = NodeStructs::Value{},
		.type = *state.state.named.types.at("Int").back(),
		.representation = ss.str()
	};
}

R T::operator()(const NodeStructs::UnaryExpression& expr) {
	std::stringstream ss;
	for (const auto& op : expr.unary_operators)
		ss << std::visit([&](const auto& token_expr) { return symbol_as_text(token_expr); }, op);
	auto repr = operator()(expr.expr);
	return_if_error(repr);
	ss << repr.value().representation;
	return whole_expression_information{
		.expression = expr,
		.value_category = NodeStructs::Value{},
		.type = *state.state.named.types.at("Int").back(),
		.representation = ss.str()
	};
}

R T::operator()(const NodeStructs::CallExpression& expr) {
	auto temp1 = transpile_expression(state, expr.operand);
	return_if_error(temp1);
	auto temp2 = type_of_function_like_call_with_args(state, expr.arguments.args, temp1.value().type);
	return_if_error(temp2);
	return whole_expression_information{
		.expression = expr,
		.value_category = temp2.value().first,
		.type = std::move(temp2).value().second,
		.representation = transpile_call_expression_with_args(state, expr.arguments.args, expr.operand).value()
	};
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
			return whole_expression_information{
				.expression = expr,
				.value_category = NodeStructs::Value{},
				.type = NodeStructs::FunctionType{ *it->second.back() },
				.representation = template_name(tmpl.name, expr.arguments.args)
			};
		}

		if (auto it = state.state.named.types.find(tmpl_name); it != state.state.named.types.end()) {
			return whole_expression_information{
				.expression = expr,
				.value_category = NodeStructs::Value{},
				.type = NodeStructs::TypeType{ *it->second.back() },
				.representation = template_name(tmpl.name, expr.arguments.args)
			};
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
			return whole_expression_information{
				.expression = expr,
				.value_category = NodeStructs::Value{},
				.type = NodeStructs::FunctionType{ *structured_f },
				.representation = template_name(tmpl.name, expr.arguments.args)
			};
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
			auto opt_error = traverse_type(state, *structured_t);
			if (opt_error.has_value())
				return opt_error.value();
			return whole_expression_information{
				.expression = expr,
				.value_category = NodeStructs::Value{},
				.type = NodeStructs::UniversalType{ std::reference_wrapper{*structured_t}},
				.representation = template_name(tmpl.name, expr.arguments.args)
			};
		}
		throw;
	}

	throw;
}

R T::operator()(const NodeStructs::ConstructExpression& expr) {
	auto operand_info = operator()(expr.operand);
	return_if_error(operand_info);

	if (std::holds_alternative<NodeStructs::TypeType>(operand_info.value().type.value)) {
		const auto& e = std::get<NodeStructs::TypeType>(operand_info.value().type.value);

		auto args_repr = transpile_args(state, expr.arguments.args);
		return_if_error(args_repr);

		return whole_expression_information{
			.expression = expr,
			.value_category = NodeStructs::Value{},
			.type = e.type,
			.representation = operand_info.value().representation + "{" + args_repr.value() + "}"
		};
	}
	throw;
}

R T::operator()(const NodeStructs::BracketAccessExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::PropertyAccessAndCallExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::PropertyAccessExpression& expr) {
	auto operand_info = transpile_expression(state, expr.operand);
	return_if_error(operand_info);

	auto t = type_of_postfix_member(state, expr.property_name, operand_info.value().type);
	return_if_error(t);

	if (std::holds_alternative<NodeStructs::InterfaceType>(operand_info.value().type.value)) {
		return whole_expression_information{
			.expression = expr,
			.value_category = operand_info.value().value_category,
			.type = t.value().second,
			.representation = "std::visit(overload([&](auto&& XX){ return XX." + expr.property_name + "; }), " + operand_info.value().representation + ")"
		};
	}
	else {
		return whole_expression_information{
			.expression = expr,
			.value_category = operand_info.value().value_category,
			.type = t.value().second,
			.representation = operand_info.value().representation + "." + expr.property_name
		};
	}
}

R T::operator()(const NodeStructs::ParenArguments& expr) {
	auto transpiled_args = expr.args
		| LIFT_TRANSFORM_X(arg, std::pair{
			std::get<0>(arg),
			operator()(std::get<1>(arg))
			};)
		| to_vec();
	for (const auto& arg : transpiled_args)
		return_if_error(arg.second);

	auto x = transpiled_args
		| LIFT_TRANSFORM_X(cat_and_info, std::pair{
			cat_and_info.first.has_value() ? cat_and_info.first.value() : NodeStructs::ArgumentCategory{ NodeStructs::Move{} },
			cat_and_info.second.value().type
			})
		| to_vec();

	return whole_expression_information{
		.expression = expr,
		.value_category = NodeStructs::Value{},
		.type = NodeStructs::AggregateType{ x },
		.representation = "(" + transpile_args(state, expr.args).value() + ")"
	};
}

R T::operator()(const NodeStructs::BraceArguments& expr) {
	auto transpiled_args = expr.args
		| LIFT_TRANSFORM_X(arg, std::pair{
			std::get<0>(arg),
			operator()(std::get<1>(arg))
		};)
		| to_vec();
	for (const auto& arg : transpiled_args)
		return_if_error(arg.second);

	auto x = transpiled_args
		| LIFT_TRANSFORM_X(cat_and_info, std::pair{
			cat_and_info.first.has_value() ? cat_and_info.first.value() : NodeStructs::ArgumentCategory{ NodeStructs::Move{} },
			cat_and_info.second.value().type
		})
		| to_vec();

	return whole_expression_information{
		.expression = expr,
		.value_category = NodeStructs::Value{},
		.type = NodeStructs::AggregateType{ x },
		.representation = "{" + transpile_args(state, expr.args).value() + "}"
	};
}

R T::operator()(const std::string& expr) {
	if (auto it = state.state.variables.find(expr); it != state.state.variables.end()) {
		return whole_expression_information{
			.expression = expr,
			.value_category = it->second.back().first,
			.type = it->second.back().second,
			.representation = expr
		};
	}
	if (auto it = state.state.named.functions.find(expr); it != state.state.named.functions.end()) {
		const auto& fn = *it->second.back();
		if (!state.state.traversed_functions.contains(fn)) {
			state.state.traversed_functions.insert(fn);
			auto t = transpile(state.unindented(), fn);
			return_if_error(t);
			state.state.transpile_in_reverse_order.push_back(std::move(t).value());
		}
		return whole_expression_information{
			.expression = expr,
			.value_category = NodeStructs::Value{},
			.type = NodeStructs::FunctionType{ fn },
			.representation = expr
		};
	}
	if (auto it = state.state.named.types.find(expr); it != state.state.named.types.end()) {
		const auto& T = *it->second.back();
		if (std::optional<error> err = traverse_type(state, T); err.has_value())
			return err.value();
		return whole_expression_information{
			.expression = expr,
			.value_category = NodeStructs::Value{},
			.type = NodeStructs::TypeType{ T },
			.representation = expr
		};
	}
	if (auto it = state.state.named.type_aliases.find(expr); it != state.state.named.type_aliases.end()) {
		const auto& alias = it->second;
		if (std::optional<error> err = traverse_type(state, alias); err.has_value())
			return err.value();
		return whole_expression_information{
			.expression = expr,
			.value_category = NodeStructs::Value{},
			.type = alias,
			.representation = transpile_typename(state, state.state.named.type_aliases_typenames.at(expr)).value()
		};
	}
	if (auto it = state.state.named.templates.find(expr); it != state.state.named.templates.end()) {
		const auto& tmpl = *it->second.back();
		return whole_expression_information{
			.expression = expr,
			.value_category = NodeStructs::Value{},
			.type = NodeStructs::Template{ tmpl },
			.representation = expr
		};
	}
	return error{ "user error", "Undeclared variable `" + expr + "`" };
}

R T::operator()(const Token<INTEGER_NUMBER>& expr) {
	return whole_expression_information{
		.expression = expr,
		.value_category = NodeStructs::Value{},
		.type = *state.state.named.types.at("Int").back(),
		.representation = expr.value
	};
}

R T::operator()(const Token<FLOATING_POINT_NUMBER>& expr) {
	return whole_expression_information{
		.expression = expr,
		.value_category = NodeStructs::Value{},
		.type = *state.state.named.types.at("Float").back(),
		.representation = expr.value
	};
}

R T::operator()(const Token<STRING>& expr) {
	return whole_expression_information{
		.expression = expr,
		.value_category = NodeStructs::Value{},
		.type = *state.state.named.types.at("String").back(),
		.representation = expr.value
	};
}
