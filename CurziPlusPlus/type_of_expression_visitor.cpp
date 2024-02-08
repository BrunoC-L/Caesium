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

R T::operator()(const NodeStructs::TemplateExpression& expr) {
	if (!std::holds_alternative<std::string>(expr.operand.expression.get())) {
		throw;
	}
	if (auto it = state.state.named.templates.find(std::get<std::string>(expr.operand.expression.get())); it != state.state.named.templates.end()) {
		if (it->second.size() != 1)
			throw;

		const auto& tmpl = *it->second.back();

		// todo check if already traversed
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
		std::vector<std::string> args = expr.arguments.args | LIFT_TRANSFORM(expression_for_template_visitor{ {}, state }) | to_vec();
		for (int i = 0; i < expr.arguments.args.size(); ++i)
			replaced = replace_all(std::move(replaced), tmpl.parameters.at(i), args.at(i));

		std::string template_name = [&](const std::string& name) {
			std::stringstream ss;
			ss << name;
			for (const auto& arg : args)
				ss << "_" << arg;
			return ss.str();
		}(tmpl.name);

		if (auto it = state.state.named.functions.find(template_name); it != state.state.named.functions.end()) {
			throw;
		}

		if (auto it = state.state.named.types.find(template_name); it != state.state.named.types.end()) {
			throw;
		}

		{
			And<IndentToken, Function, Token<END>> f{ 1 };
			auto tokens = Tokenizer(replaced).read();
			tokens_and_iterator g{ tokens, tokens.begin() };
			if (!f.build(g))
				throw;
			auto* structured_f = new NodeStructs::Function{ getStruct(f.get<Function>()) };
			structured_f->name = template_name;
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
			structured_t->name = template_name;
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
	if (auto it = state.state.variables.find(expr); it != state.state.variables.end())
		return it->second.back();

	if (auto it = state.state.named.types.find(expr); it != state.state.named.types.end())
		return std::pair{
			NodeStructs::Reference{}, // dummy arg
			NodeStructs::UniversalType{ NodeStructs::TypeType{ *it->second.back() } }
		};

	//if (auto it = state.state.named.function_templates.find(expr); it != state.state.named.function_templates.end())
	//	return std::pair{
	//		NodeStructs::Reference{}, // dummy arg
	//		NodeStructs::UniversalType{ NodeStructs::FunctionTemplateType{ *it->second.back() } }
	//	};

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

	if (auto it = state.state.named.templates.find(expr); it != state.state.named.templates.end())
		return std::pair{
			NodeStructs::Reference{}, // dummy arg
			NodeStructs::UniversalType{ *it->second.back() }
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
