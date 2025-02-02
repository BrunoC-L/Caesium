#pragma once
#include "node_structs.hpp"
#include "primitives.hpp"

NodeStructs::Expression make_expression(NodeStructs::Expression::vt expr, rule_info info);
NodeStructs::Typename make_typename(NodeStructs::Typename::vt tn, Optional<NodeStructs::ParameterCategory> cat, rule_info info);

const auto& only(const auto& e) {
	const auto& [res] = e;
	return res;
}

template <typename T, typename... Us>
bool holds(const Variant<Us...>& v) {
	return std::holds_alternative<T>(v._value);
}

template <typename T>
bool holds(const NodeStructs::Expression& x) {
	return holds<T>(x.expression.get());
}

template <typename T>
bool holds(const NodeStructs::MetaType& x) {
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
const T& get(const NodeStructs::MetaType& x) { return get<T>(x.type.get()); }

template <typename T>
T&& get(NodeStructs::MetaType&& x) { return get<T>(std::move(x).type.get()); }

template <typename T>
T& get(NodeStructs::MetaType& x) { return get<T>(x.type.get()); }

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








template <typename T>
static std::weak_ordering cmp(const T& a, const T& b) {
	if constexpr (is_specialization<T, std::vector>::value) {
		if (auto size_cmp = cmp(a.size(), b.size()); size_cmp != 0)
			return size_cmp;

		for (size_t i = 0; i < a.size(); ++i)
			if (auto v = cmp(a.at(i), b.at(i)); v != std::weak_ordering::equivalent)
				return v;

		return std::weak_ordering::equivalent;
	}
	else if constexpr (is_specialization<T, std::pair>::value) {
		if (auto first = cmp(a.first, b.first); first != 0)
			return first;
		return cmp(a.second, b.second);
	}
	else if constexpr (is_specialization<T, caesium_lib::optional::type>::value) {
		return cmp(a._value, b._value);
	}
	else if constexpr (is_specialization<T, std::optional>::value) {
		return a.has_value() && b.has_value() ? cmp(a.value(), b.value()) : cmp(a.has_value(), b.has_value());
	}
	else if constexpr (is_specialization<T, caesium_lib::variant::type>::value) {
		return cmp(a._value, b._value);
	}
	else if constexpr (is_specialization<T, std::variant>::value) {
		auto index_cmp = cmp(a.index(), b.index());
		if (index_cmp != 0)
			return index_cmp;
		return std::visit(
			[&](const auto& _a) {
				return cmp(_a, std::get<std::remove_cvref_t<decltype(_a)>>(b));
			},
			a
		);
	}
	else if constexpr (is_specialization<T, Box>::value) {
		return cmp(a.get(), b.get());
	}
	else if constexpr (is_specialization<T, NonCopyableBox>::value) {
		return cmp(a.get(), b.get());
	}
	else if constexpr (is_specialization<T, std::reference_wrapper>::value) {
		if constexpr (std::is_same_v<T, std::reference_wrapper<const Namespace>>) {
			return &a.get() <=> &b.get();
		}
		else {
			return cmp(a.get(), b.get());
		}
	}
	else if constexpr (std::is_same_v<T, std::string>)
		return a <=> b;
	else if constexpr (std::is_same_v<T, int>)
		return a <=> b;
	else if constexpr (std::is_same_v<T, char>)
		return a <=> b;
	else if constexpr (std::is_same_v<T, bool>)
		return a <=> b;
	else if constexpr (std::is_same_v<T, NodeStructs::void_t>)
		return a <=> b;
	else if constexpr (std::is_same_v<T, NodeStructs::empty_optional_t>)
		return a <=> b;
	else if constexpr (std::is_same_v<T, rule_info>)
		return std::weak_ordering::equivalent;
	else if constexpr (std::is_same_v<T, double>) {
		std::partial_ordering partial = a <=> b;
		if (partial == partial.greater)
			return std::weak_ordering::greater;
		else if (partial == partial.less)
			return std::weak_ordering::less;
		else if (partial == partial.equivalent)
			return std::weak_ordering::equivalent;
		else
			throw;
	}
	else if constexpr (is_specialization<T, NodeStructs::PrimitiveType::NonValued>::value) {
		return true;
	}
	else if constexpr (is_specialization<T, NodeStructs::PrimitiveType::Valued>::value) {
		return cmp(a.value, b.value);
	}
	else
		return a <=> b;
}

template <typename T>
auto cmp0(const T& x1, const T& x2) {
	return std::weak_ordering::equivalent;
}

template <typename T>
auto cmp1(const T& x1, const T& x2) {
	const auto& [m1] = x1;
	const auto& [n1] = x2;
	if (auto c = cmp(m1, n1); c != std::weak_ordering::equivalent)
		return c;
	return std::weak_ordering::equivalent;
}

template <typename T>
auto cmp2(const T& x1, const T& x2) {
	const auto& [m1, m2] = x1;
	const auto& [n1, n2] = x2;
	if (auto c = cmp(m1, n1); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m2, n2); c != std::weak_ordering::equivalent)
		return c;
	return std::weak_ordering::equivalent;
}

template <typename T>
auto cmp3(const T& x1, const T& x2) {
	const auto& [m1, m2, m3] = x1;
	const auto& [n1, n2, n3] = x2;
	if (auto c = cmp(m1, n1); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m2, n2); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m3, n3); c != std::weak_ordering::equivalent)
		return c;
	return std::weak_ordering::equivalent;
}

template <typename T>
auto cmp4(const T& x1, const T& x2) {
	const auto& [m1, m2, m3, m4] = x1;
	const auto& [n1, n2, n3, n4] = x2;
	if (auto c = cmp(m1, n1); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m2, n2); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m3, n3); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m4, n4); c != std::weak_ordering::equivalent)
		return c;
	return std::weak_ordering::equivalent;
}

template <typename T>
auto cmp5(const T& x1, const T& x2) {
	const auto& [m1, m2, m3, m4, m5] = x1;
	const auto& [n1, n2, n3, n4, n5] = x2;
	if (auto c = cmp(m1, n1); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m2, n2); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m3, n3); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m4, n4); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m5, n5); c != std::weak_ordering::equivalent)
		return c;
	return std::weak_ordering::equivalent;
}

template <typename T>
auto cmp6(const T& x1, const T& x2) {
	const auto& [m1, m2, m3, m4, m5, m6] = x1;
	const auto& [n1, n2, n3, n4, n5, n6] = x2;
	if (auto c = cmp(m1, n1); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m2, n2); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m3, n3); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m4, n4); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m5, n5); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m6, n6); c != std::weak_ordering::equivalent)
		return c;
	return std::weak_ordering::equivalent;
}

template <typename T>
auto cmp12(const T& x1, const T& x2) {
	const auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12] = x1;
	const auto& [n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12] = x2;
	if (auto c = cmp(m1, n1); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m2, n2); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m3, n3); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m4, n4); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m5, n5); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m6, n6); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m7, n7); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m8, n8); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m9, n9); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m10, n10); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m11, n11); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m12, n12); c != std::weak_ordering::equivalent)
		return c;
	return std::weak_ordering::equivalent;
}

inline std::weak_ordering operator<=>(const NodeStructs::MetaType& left, const NodeStructs::MetaType& right) {
	if (holds<NodeStructs::EnumType>(left) &&
		holds<NodeStructs::EnumValueType>(right)) {
		const auto& a = get<NodeStructs::EnumType>(left);
		const auto& b = get<NodeStructs::EnumValueType>(right);
		return cmp(a.enum_.get(), b.enum_.get());
	}
	if (holds<NodeStructs::EnumValueType>(left) &&
		holds<NodeStructs::EnumType>(right)) {
		const auto& a = get<NodeStructs::EnumValueType>(left);
		const auto& b = get<NodeStructs::EnumType>(right);
		return cmp(a.enum_.get(), b.enum_.get());
	}
	return cmp(left.type.get()._value, right.type.get()._value);
}

inline std::weak_ordering operator<=>(const NodeStructs::PrimitiveType& left, const NodeStructs::PrimitiveType& right) {
	const auto& a = left.value._value;
	const auto& b = right.value._value;
	return cmp(a.index(), b.index());
}

inline bool primitives_assignable(const NodeStructs::PrimitiveType& parameter, const NodeStructs::PrimitiveType& argument) {
	using vt = std::variant<
		NodeStructs::PrimitiveType::NonValued<std::string>,
		NodeStructs::PrimitiveType::NonValued<double>,
		NodeStructs::PrimitiveType::NonValued<int>,
		NodeStructs::PrimitiveType::NonValued<bool>,
		NodeStructs::PrimitiveType::NonValued<NodeStructs::void_t>,
		NodeStructs::PrimitiveType::NonValued<char>,
		NodeStructs::PrimitiveType::NonValued<NodeStructs::empty_optional_t>,

		NodeStructs::PrimitiveType::Valued<std::string>,
		NodeStructs::PrimitiveType::Valued<double>,
		NodeStructs::PrimitiveType::Valued<int>,
		NodeStructs::PrimitiveType::Valued<bool>,
		NodeStructs::PrimitiveType::Valued<NodeStructs::void_t>,
		NodeStructs::PrimitiveType::Valued<char>,
		NodeStructs::PrimitiveType::Valued<NodeStructs::empty_optional_t>
	>; 
	constexpr unsigned diff = 7; // observe how the beginning indices are NonValued<T> and index + 7 is Valued<T>

	const vt& param = parameter.value._value;
	const vt& arg = argument.value._value;

	if (param.index() == arg.index())
		return true;

	if (param.index() + diff == arg.index()) // if param is nonvalued and param + diff matches valued arg, thats ok
		return true;
}

template <typename K, typename V>
std::map<K, V> copy(const std::map<K, V>& m) {
	std::map<K, V> res;
	for (const auto& [k, v] : m)
		res.emplace(copy(k), copy(v));
	return res;
}

template <typename T>
NonCopyableBox<T> copy(const NonCopyableBox<T>& box) {
	return NonCopyableBox<T>{ copy(box.get()) };
}

inline NodeStructs::PrimitiveType copy(const NodeStructs::PrimitiveType& p) {
	return std::visit(overload(
		[&](const auto& val) -> NodeStructs::PrimitiveType {
			using raw_t = std::remove_cvref_t<decltype(val)>;
			if constexpr (is_specialization<raw_t, NodeStructs::PrimitiveType::Valued>::value)
				return NodeStructs::PrimitiveType{ raw_t{ copy(val.value) } };
			else
				return NodeStructs::PrimitiveType{ raw_t{} };
		}),
		p.value._value
	);
}

template <typename T>
std::reference_wrapper<T> copy(const std::reference_wrapper<T>& t) {
	return { t.get() };
}

template <typename T, typename U>
std::pair<T, U> copy(const std::pair<T, U>& x) {
	return copy2(x);
}

template <int token>
Token<token> copy(const Token<token>& tk) {
	return tk;
}

template <typename T>
T copy0(const T& x) {
	return {};
}

template <typename T>
T copy1(const T& x) {
	const auto& [a] = x;
	return {
		copy(a),
	};
}

template <typename T>
T copy2(const T& x) {
	const auto& [a, b] = x;
	return {
		copy(a),
		copy(b),
	};
}

template <typename T>
T copy3(const T& x) {
	const auto& [a, b, c] = x;
	return {
		copy(a),
		copy(b),
		copy(c),
	};
}

template <typename T>
T copy4(const T& x) {
	const auto& [a, b, c, d] = x;
	return {
		copy(a),
		copy(b),
		copy(c),
		copy(d),
	};
}

template <typename T>
T copy5(const T& x) {
	const auto& [a, b, c, d, e] = x;
	return {
		copy(a),
		copy(b),
		copy(c),
		copy(d),
		copy(e),
	};
}

template <typename T>
T copy6(const T& x) {
	const auto& [a, b, c, d, e, f] = x;
	return {
		copy(a),
		copy(b),
		copy(c),
		copy(d),
		copy(e),
		copy(f),
	};
}

template <typename T>
T copy12(const T& x) {
	const auto& [a, b, c, d, e, f, g, h, i, j, k, l] = x;
	return {
		copy(a),
		copy(b),
		copy(c),
		copy(d),
		copy(e),
		copy(f),
		copy(g),
		copy(h),
		copy(i),
		copy(j),
		copy(k),
		copy(l),
	};
}

#define CMP0(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp0(left, right); }
#define CMP1(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp1(left, right); }
#define CMP2(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp2(left, right); }
#define CMP3(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp3(left, right); }
#define CMP4(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp4(left, right); }
#define CMP5(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp5(left, right); }
#define CMP6(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp6(left, right); }
#define CMP12(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp12(left, right); }

#define COPY0(T) inline NodeStructs::T copy(const NodeStructs::T& e) { return copy0(e); }
#define COPY1(T) inline NodeStructs::T copy(const NodeStructs::T& e) { return copy1(e); }
#define COPY2(T) inline NodeStructs::T copy(const NodeStructs::T& e) { return copy2(e); }
#define COPY3(T) inline NodeStructs::T copy(const NodeStructs::T& e) { return copy3(e); }
#define COPY4(T) inline NodeStructs::T copy(const NodeStructs::T& e) { return copy4(e); }
#define COPY5(T) inline NodeStructs::T copy(const NodeStructs::T& e) { return copy5(e); }
#define COPY6(T) inline NodeStructs::T copy(const NodeStructs::T& e) { return copy6(e); }
#define COPY12(T) inline NodeStructs::T copy(const NodeStructs::T& e) { return copy12(e); }

#define CMP_COPY_N(N, T) CMP##N(T) COPY##N(T)

COPY1(MetaType)

CMP_COPY_N(3, Typename)
CMP_COPY_N(1, OptionalTypename)
CMP_COPY_N(1, UnionTypename)
CMP_COPY_N(1, VariadicExpansionTypename)
CMP_COPY_N(1, BaseTypename)
CMP_COPY_N(2, NamespacedTypename)
CMP_COPY_N(2, TemplatedTypename)
CMP_COPY_N(1, WordTypenameOrExpression)
CMP_COPY_N(2, Expression)
CMP_COPY_N(0, Reference)
CMP_COPY_N(0, MutableReference)
CMP_COPY_N(0, Move)
CMP_COPY_N(0, Value)
CMP_COPY_N(2, VariableDeclaration)

CMP_COPY_N(2, Statement<function_context>)
CMP_COPY_N(1, Statement<type_context>)
CMP_COPY_N(1, Statement<top_level_context>)
CMP_COPY_N(1, RunTimeStatement<function_context>)
CMP_COPY_N(1, RunTimeStatement<type_context>)
CMP_COPY_N(1, RunTimeStatement<top_level_context>)
CMP_COPY_N(1, CompileTimeStatement<function_context>)
CMP_COPY_N(1, CompileTimeStatement<type_context>)
CMP_COPY_N(1, CompileTimeStatement<top_level_context>)
CMP_COPY_N(3, VariableDeclarationStatement<function_context>)
CMP_COPY_N(3, VariableDeclarationStatement<type_context>)
CMP_COPY_N(3, VariableDeclarationStatement<top_level_context>)
CMP_COPY_N(3, IfStatement<function_context>)
CMP_COPY_N(3, IfStatement<type_context>)
CMP_COPY_N(3, IfStatement<top_level_context>)
CMP_COPY_N(3, ForStatement<function_context>)
CMP_COPY_N(3, ForStatement<type_context>)
CMP_COPY_N(3, ForStatement<top_level_context>)
CMP_COPY_N(2, IForStatement<function_context>)
CMP_COPY_N(2, IForStatement<type_context>)
CMP_COPY_N(2, IForStatement<top_level_context>)
CMP_COPY_N(2, WhileStatement<function_context>)
CMP_COPY_N(2, WhileStatement<type_context>)
CMP_COPY_N(2, WhileStatement<top_level_context>)
CMP_COPY_N(2, MatchCase<function_context>)
CMP_COPY_N(2, MatchCase<type_context>)
CMP_COPY_N(2, MatchCase<top_level_context>)
CMP_COPY_N(2, MatchStatement<function_context>)
CMP_COPY_N(2, MatchStatement<type_context>)
CMP_COPY_N(2, MatchStatement<top_level_context>)
CMP_COPY_N(1, BreakStatement<function_context>)
CMP_COPY_N(1, BreakStatement<type_context>)
CMP_COPY_N(1, BreakStatement<top_level_context>)
CMP_COPY_N(2, ReturnStatement<function_context>)
CMP_COPY_N(2, ReturnStatement<type_context>)
CMP_COPY_N(2, ReturnStatement<top_level_context>)
CMP_COPY_N(2, SwitchCase<function_context>)
CMP_COPY_N(2, SwitchCase<type_context>)
CMP_COPY_N(2, SwitchCase<top_level_context>)
CMP_COPY_N(2, SwitchStatement<function_context>)
CMP_COPY_N(2, SwitchStatement<type_context>)
CMP_COPY_N(2, SwitchStatement<top_level_context>)
CMP_COPY_N(2, Assignment<function_context>)
CMP_COPY_N(2, Assignment<type_context>)
CMP_COPY_N(2, Assignment<top_level_context>)
CMP_COPY_N(1, BlockStatement<function_context>)
CMP_COPY_N(1, BlockStatement<type_context>)
CMP_COPY_N(1, BlockStatement<top_level_context>)

CMP_COPY_N(2, FunctionArgument)
CMP_COPY_N(2, ConditionalExpression)
CMP_COPY_N(2, OrExpression)
CMP_COPY_N(2, AndExpression)
CMP_COPY_N(2, EqualityExpression)
CMP_COPY_N(2, CompareExpression)
CMP_COPY_N(2, AdditiveExpression)
CMP_COPY_N(2, MultiplicativeExpression)
CMP_COPY_N(2, UnaryExpression)
CMP_COPY_N(2, CallExpression)
CMP_COPY_N(2, NamespaceExpression)
CMP_COPY_N(2, TemplateExpression)
CMP_COPY_N(2, ConstructExpression)
CMP_COPY_N(2, BracketAccessExpression)
CMP_COPY_N(3, PropertyAccessAndCallExpression)
CMP_COPY_N(2, PropertyAccessExpression)
CMP_COPY_N(1, ParenArguments)
CMP_COPY_N(1, BraceArguments)
CMP_COPY_N(1, BracketArguments)
CMP_COPY_N(6, Type)
CMP_COPY_N(3, Alias)
CMP_COPY_N(2, MemberVariable)
CMP_COPY_N(2, FunctionType)
CMP_COPY_N(1, InterfaceType)
CMP_COPY_N(1, NamespaceType)
CMP_COPY_N(1, UnionType)
CMP_COPY_N(1, OptionalType)
CMP_COPY_N(2, TemplateType)
CMP_COPY_N(1, Builtin)
CMP_COPY_N(1, EnumType)
CMP_COPY_N(2, EnumValueType)
CMP_COPY_N(2, AggregateType)
CMP_COPY_N(0, Vector)
CMP_COPY_N(0, Set)
CMP_COPY_N(0, Map)
CMP_COPY_N(1, VectorType)
CMP_COPY_N(1, SetType)
CMP_COPY_N(2, MapType)
CMP_COPY_N(0, TypeToken)
CMP_COPY_N(0, TypeList)
CMP_COPY_N(1, TypeListType)
CMP_COPY_N(1, CompileTimeType)
CMP_COPY_N(5, Interface)
CMP_COPY_N(2, Block)
CMP_COPY_N(6, Template)
CMP_COPY_N(3, Enum)
CMP_COPY_N(1, TemplateParameter)
CMP_COPY_N(2, TemplateParameterWithDefaultValue)
CMP_COPY_N(1, VariadicTemplateParameter)
CMP_COPY_N(2, FunctionParameter)
CMP_COPY_N(5, Function)
CMP_COPY_N(12, NameSpace)
CMP_COPY_N(1, Exists)
CMP_COPY_N(3, File)
CMP_COPY_N(1, Import)
