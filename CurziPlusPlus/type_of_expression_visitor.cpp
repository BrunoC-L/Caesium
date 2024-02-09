#include "type_of_expression_visitor.hpp"
#include "transpile_expression_visitor.hpp"
#include "type_of_postfix_member_visitor.hpp"
#include "structurizer.hpp"
#include "vec_of_expected_to_expected_of_vec.hpp"
#include "replace_all.hpp"
#include "traverse_type_visitor.hpp"
#include "expression_for_template_visitor.hpp"

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
	throw;
}

R T::operator()(const NodeStructs::UnaryExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::CallExpression& expr) {
	throw;
}

expected<std::reference_wrapper<const NodeStructs::Template>> find_best_template(
	const std::vector<NodeStructs::Template const*>& templates,
	const std::vector<NodeStructs::Expression>& args
) {
	auto n_parameters = templates.at(0)->parameters.size();
	for (unsigned i = 0; i < n_parameters; ++i) {
		const auto& parameter_name = templates.at(0)->parameters.at(i).first;
		for (auto const* a : templates)
			if (parameter_name != a->parameters.at(i).first)
				throw;
	}
	NodeStructs::Template const* best = nullptr;
	for (auto const* a : templates) {
		bool is_base = true;
		for (const auto& p : a->parameters)
			if (p.second.has_value()) {
				is_base = false;
				break;
			}
		if (!is_base)
			continue;
		if (best != nullptr)
			throw; // 2 templates have no default params
		best = a;
	}

	if (best == nullptr)
		throw;

	int best_matches = 0;
	int second_best_matches = 0;
	for (auto const* a : templates) {
		int n_matches = 0;
		for (unsigned i = 0; i < n_parameters; ++i) {
			const auto& [name, opt_value] = a->parameters.at(i);
			if (!opt_value.has_value())
				continue;
			if (opt_value.value() <=> args.at(i) == std::strong_ordering::equal)
				n_matches += 1;
			else {
				n_matches = -1;
				break;
			}
		}
		if (n_matches >= best_matches) {
			second_best_matches = best_matches;
			best_matches = n_matches;
			best = a;
		}
	}
	if (second_best_matches == best_matches && best_matches > 0) {
		std::stringstream ss;
		for (const auto& arg : args)
			ss << expression_for_template_visitor{ {} }(arg) << ", ";
		return error{
			"user error",
			"template collision, example: can't choose between template<T, U=X> and template<T=X, U> for template<X,X>."
			" Template was : `" + templates.at(0)->name + "` and arguments were [" + ss.str() + "]"
		};
	}
	return std::reference_wrapper<const NodeStructs::Template>{*best};
}

R T::operator()(const NodeStructs::TemplateExpression& expr) {
	if (!std::holds_alternative<std::string>(expr.operand.expression.get()))
		throw;

	if (auto it = state.state.named.templates.find(std::get<std::string>(expr.operand.expression.get())); it != state.state.named.templates.end()) {
		auto t = find_best_template(it->second, expr.arguments.args);
		return_if_error(t);
		const auto& tmpl = t.value().get();
		std::vector<std::string> args = expr.arguments.args | LIFT_TRANSFORM(expression_for_template_visitor{ {} }) | to_vec();
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
				auto x = type_of_typename_visitor{ {}, state }(templated_with.at(0));
				return_if_error(x);
				return NodeStructs::UniversalType{ NodeStructs::VectorType{ Box<NodeStructs::UniversalType>{ std::move(x).value() } } };
			}
			if (tmpl.name == "Set") {
				auto x = type_of_typename_visitor{ {}, state }(templated_with.at(0));
				return_if_error(x);
				return NodeStructs::UniversalType{ NodeStructs::SetType{ Box<NodeStructs::UniversalType>{ std::move(x).value() } } };
			}
			if (tmpl.name == "Map") {
				auto x = type_of_typename_visitor{ {}, state }(templated_with.at(0));
				auto y = type_of_typename_visitor{ {}, state }(templated_with.at(1));
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
			throw;
		}

		if (auto it = state.state.named.types.find(tmpl_name); it != state.state.named.types.end()) {
			throw;
		}

		{
			And<IndentToken, Function, Token<END>> f{ 1 };
			auto tokens = Tokenizer(replaced).read();
			tokens_and_iterator g{ tokens, tokens.begin() };
			if (!f.build(g))
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
			if (!t.build(g))
				throw;
			auto* structured_t = new NodeStructs::Type{ getStruct(t.get<Type>()) };
			structured_t->name = tmpl_name;
			state.state.named.types[structured_t->name].push_back(structured_t);
			auto opt_error = traverse_type_visitor{ {}, state }(*structured_t);
			if (opt_error.has_value())
				return opt_error.value();
			return std::pair{ NodeStructs::Reference{}, NodeStructs::UniversalType{ std::reference_wrapper{*structured_t}} };
		}
		throw;
	}
}

R T::operator()(const NodeStructs::ConstructExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::BracketAccessExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::PropertyAccessExpression& expr) {
	auto operand_t = type_of_expression_visitor{ {}, state }(expr.operand);
	return_if_error(operand_t);
	auto t = type_of_postfix_member_visitor{ {}, state, expr.property_name }(operand_t.value().second);
	return_if_error(t);
	return std::pair{ operand_t.value().first, std::move(t).value().second };
}

R T::operator()(const NodeStructs::ParenArguments& expr) {
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
