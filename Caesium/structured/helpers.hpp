#pragma once
#include "node_structs.hpp"
#include "../grammar/primitives.hpp"
#include <set>

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


#define CMP_N(N, T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right);
#define CMP_COPY_N(N, T) CMP_N(N,T)
#include "cmp_copy_all.hpp"

inline std::weak_ordering operator<=>(const NodeStructs::MetaType& left, const NodeStructs::MetaType& right);
inline std::weak_ordering operator<=>(const NodeStructs::PrimitiveType& left, const NodeStructs::PrimitiveType& right);

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
		return std::weak_ordering::equivalent;
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

template <typename T, typename compare>
struct copy_t<std::set<T, compare>> {
	static std::set<T, compare> copy(const std::set<T, compare>& s) {
		std::set<T, compare> res;
		for (const auto& e : s)
			res.insert(::copy(e));
		return res;
	}
};

template <typename K, typename V, typename compare>
struct copy_t<std::map<K, V, compare>> {
	static std::map<K, V, compare> copy(const std::map<K, V, compare>& m) {
		std::map<K, V, compare> res;
		for (const auto& [k, v] : m)
			res.emplace(::copy(k), ::copy(v));
		return res;
	}
};

template <typename T>
struct copy_t<NonCopyableBox<T>> {
	static NonCopyableBox<T> copy(const NonCopyableBox<T>& box) {
		return NonCopyableBox<T>{ ::copy(box.get()) };
	}
};

template <typename T>
struct copy_t<NodeStructs::PrimitiveType::NonValued<T>> {
	static NodeStructs::PrimitiveType::NonValued<T> copy(const NodeStructs::PrimitiveType::NonValued<T>& v) {
		return NodeStructs::PrimitiveType::NonValued<T>{};
	}
};

template <typename T>
struct copy_t<NodeStructs::PrimitiveType::Valued<T>> {
	static NodeStructs::PrimitiveType::Valued<T> copy(const NodeStructs::PrimitiveType::Valued<T>& v) {
		return NodeStructs::PrimitiveType::Valued<T>{ ::copy(v.value) };
	}
};

template <>
struct copy_t<NodeStructs::PrimitiveType> {
	static NodeStructs::PrimitiveType copy(const NodeStructs::PrimitiveType& p) {
		return NodeStructs::PrimitiveType{ ::copy(p.value) };
	}
};

template <typename T>
struct copy_t<std::reference_wrapper<T>> {
	static std::reference_wrapper<T> copy(const std::reference_wrapper<T>& t) {
		return { t.get() };
	}
};

template <typename T, typename U>
struct copy_t<std::pair<T, U>> {
	static std::pair<T, U> copy(const std::pair<T, U>& x) {
		return copy2(x);
	}
};

#define CMP_N(N, T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp##N (left, right); }
#define COPY_N(N,T) template <> struct copy_t<NodeStructs::T> { static NodeStructs::T copy(const NodeStructs::T& e) { return copy##N (e); } };
#define CMP_COPY_N(N, T) CMP_N(N,T) COPY_N(N,T)
#include "cmp_copy_all.hpp"

COPY_N(1, MetaType)
