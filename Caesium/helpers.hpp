#pragma once
#include "structured/node_structs.hpp"
#include "core/realised.hpp"
#include "grammar/primitives.hpp"
#include <set>

NodeStructs::Expression make_expression(NodeStructs::Expression::vt expr, rule_info info);
NodeStructs::Typename make_typename(NodeStructs::Typename::vt tn, Optional<NodeStructs::ParameterCategory> cat, rule_info info);

const auto& only(const auto& e) {
	const auto& [res] = e;
	return res;
}

template <typename T, typename... Us>
bool holds(const std::variant<Us...>& v) {
	return std::holds_alternative<T>(v);
}

template <typename T, typename... Us>
bool holds(const Variant<Us...>& v) {
	return holds<T>(v._value);
}

template <typename T>
bool holds(const NodeStructs::Expression& x) {
	return holds<T>(x.expression.get());
}

template <typename T>
bool holds(const Realised::MetaType& x) {
	return holds<T>(x.type.get());
}

template <typename T>
bool holds(const NodeStructs::Typename& x) {
	return holds<T>(x.value.get());
}

template <typename T, typename context>
bool holds(const NodeStructs::Statement<context>& x) {
	return holds<T>(x.statement.get());
}

template <typename T, typename... Us>
const T& get(const std::variant<Us...>& v) { return std::get<T>(v); }

template <typename T, typename... Us>
const T& get(const Variant<Us...>& v) { return std::get<T>(v._value); }

template <typename T, typename... Us>
T&& get(Variant<Us...>&& v) { return std::get<T>(std::move(v)._value); }

template <typename T, typename... Us>
T& get(Variant<Us...>& v) { return std::get<T>(v._value); }

template <typename T>
const T& get(const NodeStructs::Expression& x) { return get<T>(x.expression.get()); }

template <typename T>
T&& get(NodeStructs::Expression&& x) { return get<T>(std::move(x).expression.get()); }

template <typename T>
T& get(NodeStructs::Expression& x) { return get<T>(x.expression.get()); }

template <typename T>
const T& get(const Realised::MetaType& x) { return get<T>(x.type.get()); }

template <typename T>
T&& get(Realised::MetaType&& x) { return get<T>(std::move(x).type.get()); }

template <typename T>
T& get(Realised::MetaType& x) { return get<T>(x.type.get()); }

template <typename T>
const T& get(const NodeStructs::Typename& x) { return get<T>(x.value.get()); }

template <typename T>
T&& get(NodeStructs::Typename&& x) { return get<T>(std::move(x).value.get()); }

template <typename T>
T& get(NodeStructs::Typename& x) { return get<T>(x.value.get()); }

template <typename T, typename context>
const T& get(const NodeStructs::Statement<context>& x) { return get<T>(x.statement.get()); }

template <typename T, typename context>
T&& get(NodeStructs::Statement<context>&& x) { return get<T>(std::move(x).statement.get()); }

template <typename T, typename context>
T& get(NodeStructs::Statement<context>& x) { return get<T>(x.statement.get()); }

const std::string& original_representation(const NodeStructs::WordTypenameOrExpression& tn_or_expr);
const std::string& original_representation(const std::string& e);
const std::string& original_representation(const NodeStructs::Typename& e);
const std::string& original_representation(const NodeStructs::Expression& e);

inline bool primitives_assignable(const Realised::PrimitiveType& parameter, const Realised::PrimitiveType& argument) {
	using vt = std::variant<
		Realised::PrimitiveType::NonValued<std::string>,
		Realised::PrimitiveType::NonValued<double>,
		Realised::PrimitiveType::NonValued<int>,
		Realised::PrimitiveType::NonValued<bool>,
		Realised::PrimitiveType::NonValued<Realised::void_t>,
		Realised::PrimitiveType::NonValued<char>,
		Realised::PrimitiveType::NonValued<Realised::empty_optional_t>,

		Realised::PrimitiveType::Valued<std::string>,
		Realised::PrimitiveType::Valued<double>,
		Realised::PrimitiveType::Valued<int>,
		Realised::PrimitiveType::Valued<bool>,
		Realised::PrimitiveType::Valued<Realised::void_t>,
		Realised::PrimitiveType::Valued<char>,
		Realised::PrimitiveType::Valued<Realised::empty_optional_t>
	>;
	constexpr unsigned diff = 7; // observe how the beginning indices are NonValued<T> and index + 7 is Valued<T>

	const vt& param = parameter.value._value;
	const vt& arg = argument.value._value;

	if (param.index() == arg.index())
		return true;

	if (param.index() + diff == arg.index()) // if param is nonvalued and param + diff matches valued arg, thats ok
		return true;

	return false;
}

#include "cmp.hpp"
#include "copy.hpp"
