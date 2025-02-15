#include "../core/toCPP.hpp"
#include "../utility/replace_all.hpp"
#include <algorithm>

using T = transpile_typename_visitor;
using R = T::R;

R T::operator()(const NodeStructs::BaseTypename& type) {
#define OK(X) if (type.type == X) return X;
	OK("Int");
	OK("Bool");
	OK("String");
	OK("Char");
	OK("Void");
	OK("Floating");
#define OK(X) if (auto it = state.state.global_namespace. X .find(type.type); it != state.state.global_namespace. X .end()) return type.type;
	OK(types);
	OK(functions);
	OK(interfaces);
	OK(namespaces);
	OK(builtins);
	OK(blocks);
	OK(templates);
#undef OK

	if (auto it = state.state.global_namespace.aliases.find(type.type); it != state.state.global_namespace.aliases.end())
		return operator()(it->second);
	if (auto it = state.state.global_namespace.enums.find(type.type); it != state.state.global_namespace.enums.end())
		return "Int";
	return error{
		"user error",
		"undeclared identifier `" + type.type + "`"
	};
}

R T::operator()(const NodeStructs::NamespacedTypename& type) {
	auto type_of_namespace_or_e = type_of_typename(state, variables, type.name_space);
	return_if_error(type_of_namespace_or_e);
	const auto& type_of_namespace = type_of_namespace_or_e.value();

	auto repr_of_typename_or_e = operator()(type.name_space);
	return_if_error(repr_of_typename_or_e);
	const auto& repr_of_typename = repr_of_typename_or_e.value();

	return std::visit(overload(
		[](const auto& e) -> R {
			throw;
		},
		[&](const NodeStructs::EnumType& e) -> R {
			const auto& _enum = e.enum_.get();
			if (auto it = std::find(_enum.values.begin(), _enum.values.end(), type.name_in_name_space); it != _enum.values.end())
				return _enum.name + "__" + type.name_in_name_space;
			throw;
		},
		[&](const NodeStructs::NamespaceType& e) -> R {
			if (auto it = e.name_space.get().types.find(type.name_in_name_space); it != e.name_space.get().types.end())
				return repr_of_typename + "__" + type.name_in_name_space;
			if (auto it = e.name_space.get().aliases.find(type.name_in_name_space); it != e.name_space.get().aliases.end())
				return operator()(it->second);
			if (auto it = e.name_space.get().namespaces.find(type.name_in_name_space); it != e.name_space.get().namespaces.end())
				return repr_of_typename + "__" + type.name_in_name_space;
			if (auto it = e.name_space.get().templates.find(type.name_in_name_space); it != e.name_space.get().templates.end())
				return repr_of_typename + "__" + type.name_in_name_space;
			throw;
		}
	), type_of_namespace.type.get()._value);
}

R T::operator()(const NodeStructs::TemplatedTypename& type) {
	bool is_variant = cmp(type.type.value.get(), NodeStructs::Typename::vt{ NodeStructs::BaseTypename{ "Union" } }) == std::weak_ordering::equivalent;
	bool is_vec_or_set =
		cmp(type.type.value.get(), NodeStructs::Typename::vt{ NodeStructs::BaseTypename{ "Vector" } }) == std::weak_ordering::equivalent
		|| cmp(type.type.value.get(), NodeStructs::Typename::vt{ NodeStructs::BaseTypename{ "Set" } }) == std::weak_ordering::equivalent;
	bool is_map = cmp(type.type.value.get(), NodeStructs::Typename::vt{ NodeStructs::BaseTypename{ "Map" } }) == std::weak_ordering::equivalent;
	if (is_vec_or_set || is_map || is_variant) {
		if (is_vec_or_set && type.templated_with.size() != 1)
			throw;
		if (is_map && type.templated_with.size() != 2)
			throw;
		std::stringstream single_word, with_brackets;
		auto tmpl = type_template_of_typename(state, variables, type.templated_with, type.type);
		return_if_error(tmpl);
		R t = operator()(type.type);
		return_if_error(t);

		single_word << t.value() << "_";
		with_brackets << t.value() << "<";
		bool first = true;
		for (const auto& t : type.templated_with) {
			if (first)
				first = false;
			else {
				single_word << "_";
				with_brackets << ", ";
			}
			auto base = std::visit(overload(
				[&](const std::string s) -> std::string {
					auto test = transpile_expression(state, variables, s);
					if (test.has_value()) {
						if (!std::holds_alternative<type_information>(test.value()))
							throw;
						return std::get<type_information>(test.value()).representation;
					}
					else {
						auto test2 = transpile_typename(state, variables, NodeStructs::BaseTypename{ s });
						if (test2.has_error())
							throw;
						return test2.value();
					}
					throw;
				},
				[&](const NodeStructs::Expression& e) -> std::string {
					auto t = transpile_expression(state, variables, e);
					if (t.has_error())
						throw;
					if (!std::holds_alternative<type_information>(t.value()))
						throw;
					return std::get<type_information>(t.value()).representation;
				},
				[&](const NodeStructs::Typename& t) -> std::string {
					return transpile_typename(state, variables, t).value();
				}
			), t.value._value);
			with_brackets << base;
			single_word << replace_all(std::move(base), "<", "_", ">", "_", " " , "", ",", "_");
		}
		single_word << "_";
		with_brackets << ">";
		state.state.global_namespace.aliases.insert({
			single_word.str(),
			NodeStructs::Typename{
				.value = copy(type),
				.category = NodeStructs::Value{},
				.rule_info = copy(type.type.rule_info)
			}
		});
		if (single_word.str() == "f_Int_") {
			throw;
		}
		state.state.aliases_to_transpile.insert({ single_word.str(), with_brackets.str() });
		return single_word.str();
	}
	else {
		std::stringstream ss;
		auto tmpl = type_template_of_typename(state, variables, type.templated_with, type.type);
		return_if_error(tmpl);
		auto t = operator()(type.type);
		return_if_error(t);
		ss << t.value() << "__";
		bool first = true;
		for (const auto& t : type.templated_with) {
			if (first)
				first = false;
			else
				ss << "__";
			auto a = word_typename_or_expression_for_template(state, variables, t);
			return_if_error(a);
			ss << a.value();
		}
		return ss.str();
	}
}

//template <typename T>
//void swap(T& a, T& b) {
//	T c = std::move(a);
//	new (&a) T(std::move(b));
//	new (&b) T(std::move(c));
//}
 
void swap(NodeStructs::Typename& a, NodeStructs::Typename& b) {
	NodeStructs::Typename c = std::move(a);
	new (&a) NodeStructs::Typename(std::move(b));
	new (&b) NodeStructs::Typename(std::move(c));
}

template<typename T>
void bubble_sort_swap(std::vector<T>& arr) {
	int n = arr.size();
	for (int i = 0; i < n - 1; ++i)
		for (int j = 0; j < n - i - 1; ++j)
			if (arr.at(j) > arr.at(j + 1))
				throw;
				//swap(arr.at(j), arr.at(j + 1));
}

R T::operator()(const NodeStructs::OptionalTypename& type) {
	auto inner_or_e = operator()(type.type);
	return_if_error(inner_or_e);
	auto out = replace_all(copy(inner_or_e.value()), "<", "_", ">", "_", " ", "", ",", "_");
	state.state.global_namespace.aliases.insert({
		out,
		NodeStructs::Typename{
			.value = copy(type),
			.category = NodeStructs::Value{},
			.rule_info = copy(type.type.rule_info)
		}
	});
	state.state.aliases_to_transpile.insert({ "Optional_" + out + "_", "Optional<" + out + ">" });
	return "Optional_" + std::move(out) + "_";
}

R T::operator()(const NodeStructs::UnionTypename& type) {
	std::stringstream ss;
	ss << "Union<";
	auto ts = copy(type.ors);
	if (ts.size() == 0)
		throw;
	bubble_sort_swap(ts); // todo unique
	bool first = true;
	for (const auto& t : ts) {
		if (first)
			first = false;
		else
			ss << ", ";
		auto res = operator()(t);
		return_if_error(res);
		ss << res.value();
	}
	ss << ">";
	auto out = ss.str();
	auto replaced = replace_all(copy(out), "<", "_", ">", "_", " ", "", ",", "_");
	state.state.global_namespace.aliases.insert({
		replaced,
		NodeStructs::Typename{
			.value = copy(type),
			.category = NodeStructs::Value{},
			.rule_info = rule_info{.file_name = "todo:/", .content = "todo??" }
		}
	});
	if (replaced == "f_Int_") {
		throw;
	}
	state.state.aliases_to_transpile.insert({ replaced, std::move(out) });
	return replaced;
}

R T::operator()(const NodeStructs::VariadicExpansionTypename& t) {
	throw;
}
