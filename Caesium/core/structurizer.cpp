#pragma once
#include "structurizer.hpp"
#include <iostream>

static std::string word_or_auto(const Or<Token<AUTO>, grammar::Word>& tk) {
	return std::visit(
		overload(
			[&](const Token<AUTO>& _auto) -> std::string {
				return "auto";
			},
			[&](const grammar::Word& w) -> std::string {
				return w.value;
			}
		), tk.value()
	);
}

NodeStructs::Import getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Import& f) {
	return { std::visit(
		overload(overload_default_error,
			[&](const grammar::Word& word) {
				return word.value + ".caesium";
			},
			[&](const grammar::String& string) {
				return string.value;
		}),
		f.get<Or<grammar::Word, grammar::String>>().value()
	) };
}

template <typename tag_t>
NodeStructs::WordTypenameOrExpression getTypenameOrExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::TypenameOrExpression& tn_or_expr) {
	return std::visit(overload(
		[&](const grammar::Typename& tn) -> NodeStructs::WordTypenameOrExpression {
			bool has_to_be_interpreted_as_typename_because_of_category_or_optional =
				tn.get<Opt<grammar::ParameterCategory>>().has_value() || tn.get<Opt<Token<QUESTION>>>().has_value();
			if (has_to_be_interpreted_as_typename_because_of_category_or_optional)
				return { getStruct(file_name, vec, tn, tag_t{}) };
			else
				return std::visit(overload(
					[&](const Token<AUTO>&) -> NodeStructs::WordTypenameOrExpression { // auto is a typename
						return { getStruct(file_name, vec, tn, tag_t{}) };
					},
					[&](const grammar::NonAutoTypename& non_auto_typename) -> NodeStructs::WordTypenameOrExpression {
						bool has_to_be_interpreted_as_typename_because_of_extensions =
							non_auto_typename.get<Star<Or<
							grammar::NamespaceTypenameExtension,
							grammar::TemplateTypenameExtension,
							grammar::UnionTypenameExtension,
							Token<QUESTION>
							>>>().nodes.size() > 0;
						if (has_to_be_interpreted_as_typename_because_of_extensions)
							return { getStruct(file_name, vec, tn, tag_t{}) };

						return std::visit(overload(
							[&](const grammar::VariadicExpansionTypename&) -> NodeStructs::WordTypenameOrExpression { // X... is a typename
								return { getStruct(file_name, vec, tn, tag_t{}) };
							},
							[&](const grammar::Word& word) -> NodeStructs::WordTypenameOrExpression {
								return { word.value };
							}
						), non_auto_typename.get<Or<grammar::VariadicExpansionTypename, grammar::Word>>().value());
					}
			), tn.get<Or<Token<AUTO>, grammar::NonAutoTypename>>().value());
		},
		[&](const grammar::Expression& expr) -> NodeStructs::WordTypenameOrExpression {
			return { getExpressionStruct(file_name, vec, expr) };
		}
	), tn_or_expr.value());
}

NodeStructs::Typename getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Typename& t,
	const auto& exts,
	NodeStructs::Typename res,
	size_t i
) {
	if (i == exts.size())
		return std::move(res);
	const auto& ext = exts.at(i);
	return std::visit(overload(
		[&](const NodeStructs::VariadicExpansionTypename&) {
			return getStruct(file_name, vec, t, exts, std::visit(overload(overload_default_error,
				[&](const grammar::NamespaceTypenameExtension& e) -> NodeStructs::Typename {
					throw;
				},
				[&](const grammar::TemplateTypenameExtension& e) -> NodeStructs::Typename {
					return make_typename(
						NodeStructs::VariadicExpansionTypename{
							make_typename(
								NodeStructs::TemplatedTypename{
									get<NodeStructs::VariadicExpansionTypename>(std::move(res)).type,
									e.get<CommaStar<grammar::TypenameOrExpression>>().get<grammar::TypenameOrExpression>()
									| std::views::transform([&](const grammar::TypenameOrExpression& e) { return getStruct(file_name, vec, e, tag_expect_empty_category{}); })
									| to_vec()
								}, NodeStructs::Value{}, rule_info_from_rule(file_name, vec, e)
							)
						}, NodeStructs::Value{}, rule_info_from_rule(file_name, vec, e)
					);
				},
				[&](const grammar::UnionTypenameExtension& ext) -> NodeStructs::Typename {
					// variadics dont support unions
					throw;
				},
				[&](const Token<QUESTION>& ext) -> NodeStructs::Typename {
					throw;
				}
			), ext.value()), i + 1);
		},
		[&](const auto&) { // just checking its not a variadic expansion
			return getStruct(file_name, vec, t, exts, std::visit(overload(overload_default_error,
				[&](const grammar::NamespaceTypenameExtension& e) -> NodeStructs::Typename {
					return make_typename(NodeStructs::NamespacedTypename{ std::move(res), e.get<grammar::Word>().value }, NodeStructs::Value{}, rule_info_from_rule(file_name, vec, e));
				},
				[&](const grammar::TemplateTypenameExtension& e) -> NodeStructs::Typename {
					return make_typename(NodeStructs::TemplatedTypename{
						std::move(res),
						e.get<CommaStar<grammar::TypenameOrExpression>>().get<grammar::TypenameOrExpression>()
						| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e, tag_allow_value_category_or_empty{}); })
						| to_vec()
						}, NodeStructs::Value{}, rule_info_from_rule(file_name, vec, e));
				},
				[&](const grammar::UnionTypenameExtension& ext) -> NodeStructs::Typename {
					auto temp = getStruct(file_name, vec, ext.get<Alloc<grammar::Typename>>().get(), tag_allow_value_category_or_empty{});
					if (holds<NodeStructs::UnionTypename>(res)) {
						if (holds<NodeStructs::UnionTypename>(temp))
							throw;
						get<NodeStructs::UnionTypename>(res).ors.push_back(std::move(temp));
						return std::move(res);
					}
					if (holds<NodeStructs::UnionTypename>(temp)) {
						auto& ut = get<NodeStructs::UnionTypename>(temp);
						std::vector<NodeStructs::Typename> v;
						v.reserve(ut.ors.size());
						v.push_back(std::move(res));
						for (auto& o : ut.ors)
							v.push_back(std::move(o));
						std::swap(ut.ors, v);
						return temp;
					}
					std::vector<NodeStructs::Typename> v;
					v.push_back(std::move(res));
					v.push_back(std::move(temp));
					return make_typename(NodeStructs::UnionTypename{
						std::move(v)
					}, NodeStructs::Value{}, rule_info_from_rule(file_name, vec, ext));
				},
				[&](const Token<QUESTION>& ext) -> NodeStructs::Typename {
					return make_typename(NodeStructs::OptionalTypename{ std::move(res) }, NodeStructs::Value{}, rule_info_from_rule(file_name, vec, ext));
				}
			), ext.value()), i + 1);
		}
	), res.value.get()._value);
}

template <typename tag>
NodeStructs::Typename get_typename_struct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Typename& t) {
	const auto& auto_or_tn = t.get<Or<Token<AUTO>, grammar::NonAutoTypename>>();
	NodeStructs::ParameterCategory value_cat = getStruct(file_name, vec, t.get<Opt<grammar::ParameterCategory>>(), tag{});
	const auto& optional = t.get<Opt<Token<QUESTION>>>();

	return std::visit(overload(
		[&](const Token<AUTO>& a) -> NodeStructs::Typename {
			return make_typename(NodeStructs::BaseTypename{ "auto" }, std::move(value_cat), rule_info_from_rule(file_name, vec, a));
		},
		[&](const grammar::NonAutoTypename& e) -> NodeStructs::Typename {
			NodeStructs::Typename res = std::visit(overload(
				[&](const grammar::Word& w) -> NodeStructs::Typename {
					return make_typename(NodeStructs::BaseTypename{ w.value }, NodeStructs::Value{}, rule_info_from_rule(file_name, vec, w));
				},
				[&](const grammar::VariadicExpansionTypename& vetn) -> NodeStructs::Typename {
					return make_typename(
						NodeStructs::VariadicExpansionTypename{
							make_typename(NodeStructs::BaseTypename{ vetn.get<grammar::Word>().value }, NodeStructs::Value{}, rule_info_from_rule(file_name, vec, vetn.get<grammar::Word>()))
						},
						NodeStructs::Value{}, rule_info_from_rule(file_name, vec, vetn)
					);
				}
			), e.get<Or<grammar::VariadicExpansionTypename, grammar::Word>>().value());
			using opts = Or<grammar::NamespaceTypenameExtension, grammar::TemplateTypenameExtension, grammar::UnionTypenameExtension, Token<QUESTION>>;
			NodeStructs::Typename::vt res2 = getStruct(file_name, vec, t, e.get<Star<opts>>().get<opts>(), std::move(res), 0).value;
			return make_typename(std::move(res2), std::move(value_cat), rule_info_from_rule(file_name, vec, e));
		}
	), auto_or_tn.value());
}

NodeStructs::Typename getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Typename& t, tag_expect_value_category) {
	return get_typename_struct<tag_expect_value_category>(file_name, vec, t);
}

NodeStructs::Typename getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Typename& t, tag_expect_empty_category) {
	return get_typename_struct<tag_expect_empty_category>(file_name, vec, t);
}

NodeStructs::Typename getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Typename& t, tag_allow_value_category_or_empty) {
	return get_typename_struct<tag_allow_value_category_or_empty>(file_name, vec, t);
}

NodeStructs::ParameterCategory getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::ParameterCategory& vc) {
	return std::visit(
		overload(overload_default_error,
			/*[&](const Token<KEY>&) -> NodeStructs::ValueCategory {
				return NodeStructs::Key{};
			},*/
			[&](const Token<VAL>&) -> NodeStructs::ParameterCategory {
				return NodeStructs::Value{};
			},
			[&](const Token<REF>&) -> NodeStructs::ParameterCategory {
				return NodeStructs::Reference{};
			},
			[&](const And<Token<REF>, Token<NOT>>&) -> NodeStructs::ParameterCategory {
				return NodeStructs::MutableReference{};
			}
		),
		vc.value()
	);
}

NodeStructs::ParameterCategory getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const Opt<grammar::ParameterCategory>& vc, tag_expect_value_category) {
	if (vc.has_value())
		return getStruct(file_name, vec, vc.value());
	else
		throw;
}

NodeStructs::ParameterCategory getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const Opt<grammar::ParameterCategory>& vc, tag_expect_empty_category) {
	if (vc.has_value())
		throw;
	return NodeStructs::Value{};
}

NodeStructs::ParameterCategory getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const Opt<grammar::ParameterCategory>& vc, tag_allow_value_category_or_empty) {
	if (vc.has_value())
		return getStruct(file_name, vec, vc.value());
	else
		return NodeStructs::Value{};
}

NodeStructs::Function getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Function& f, std::optional<NodeStructs::Typename> name_space) {
	return NodeStructs::Function{
		.name = f.get<grammar::Word>().value,
		.name_space = std::move(name_space),
		.returnType = getStruct(file_name, vec, f.get<grammar::Typename>(), tag_allow_value_category_or_empty{}),
		.parameters = f.get<grammar::FunctionParameters>().get<And<Commit<grammar::Typename>, grammar::Word>>()
			| std::views::transform([&](const grammar::FunctionParameter& type_and_name) -> NodeStructs::FunctionParameter {
				auto res = NodeStructs::FunctionParameter{
					getStruct(file_name, vec, type_and_name.get<Commit<grammar::Typename>>(), tag_expect_value_category{}),
					type_and_name.get<grammar::Word>().value
				};
				if (!res.typename_.category._value.has_value())
					throw;
				return res;
			})
			| to_vec(),
		.statements = getStatements(file_name, vec, f.get<grammar::ColonIndentCodeBlock>())
	};
}

Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter> getTemplateParameter(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const And<grammar::Word, Token<DOTS>>& parameter
) {
	return NodeStructs::VariadicTemplateParameter{ parameter.get<grammar::Word>().value };
}

Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter> getTemplateParameter(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const And<grammar::Word, Opt<And<Token<EQUAL>, grammar::Expression>>>& parameter_and_optional_value
) {
	if (parameter_and_optional_value.get<Opt<And<Token<EQUAL>, grammar::Expression>>>().has_value())
		return NodeStructs::TemplateParameterWithDefaultValue{
			parameter_and_optional_value.get<grammar::Word>().value,
			getExpressionStruct(file_name, vec, parameter_and_optional_value.get<Opt<And<Token<EQUAL>, grammar::Expression>>>().value().get<grammar::Expression>())
	};
	else
		return NodeStructs::TemplateParameter{
			parameter_and_optional_value.get<grammar::Word>().value,
	};
}
using parameter_t = Or<
	And<
	grammar::Word,
	Token<DOTS>
	>,
	And<
	grammar::Word,
	Opt<And<
	Token<EQUAL>,
	grammar::Expression
	>>
	>
	>;

Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter> getTemplateParameter(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const parameter_t& parameter
) {
	return std::visit([&](const auto& e) { return getTemplateParameter(file_name, vec, e); }, parameter.value());
}

NodeStructs::Template getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Template& t) {
	return getStruct(file_name, vec, t, std::nullopt);
}

NodeStructs::Template getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Template& t, std::optional<NodeStructs::Typename> name_space) {
	using parameters_t = CommaStar<parameter_t>;
	return {
		.name = t.get<grammar::Word>().value,
		.name_space = std::move(name_space),
		.parameters = t.get<parameters_t>().get<parameter_t>()
			| std::views::transform([&](auto&& e) { return getTemplateParameter(file_name, vec, e); })
			| to_vec(),
		.templated = t.get<TemplateBody>().value,
		.indent = t.n_indent + 1
	};
}

NodeStructs::MemberVariable getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::MemberVariable& f) {
	return NodeStructs::MemberVariable{
		.type = getStruct(file_name, vec, f.get<grammar::Typename>(), tag_allow_value_category_or_empty{}),
		.name = f.get<grammar::Word>().value,
	};
}

NodeStructs::Alias getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Alias& f, std::optional<NodeStructs::Typename> name_space) {
	return NodeStructs::Alias{
		.aliasFrom = f.get<grammar::Word>().value,
		.aliasTo = getStruct(file_name, vec, f.get<grammar::Typename>(), tag_allow_value_category_or_empty{}),
		.name_space = std::move(name_space)
	};
}

NodeStructs::Type getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Type& t, std::optional<NodeStructs::Typename> name_space) {
	auto elems = t.get<Indent<Star<And<IndentToken, grammar::TypeElement>>>>().get_view<grammar::TypeElement>()
		| std::views::transform([&](auto&& e) { return e.value(); });
	return {
		.name = t.get<grammar::Word>().value,
		.name_space = copy(name_space),
		.aliases = elems
			| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::Alias>(e); })
			| std::views::transform([&](auto&& e) { return std::get<grammar::Alias>(e); })
			| std::views::transform([&](auto&& t) { return getStruct(file_name, vec, t, copy(name_space)); })
			| to_vec(),
		.member_variables = elems
			| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::MemberVariable>(e); })
			| std::views::transform([&](auto&& e) { return std::get<grammar::MemberVariable>(e); })
			| std::views::transform([&](auto&& t) { return getStruct(file_name, vec, t); })
			| to_vec()
	};
}

NodeStructs::Interface getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Interface& interface, std::optional<NodeStructs::Typename> name_space) {
	using Member = Or<
		grammar::Alias,
		grammar::MemberVariable
	>;
	using Members = Indent<Star<And<IndentToken, Member>>>;
	auto elems = interface.get<Members>().get_view<Member>()
		| std::views::transform([&](auto&& e) { return e.value(); });
	return {
		.name = interface.get<grammar::Word>().value,
		.name_space = copy(name_space),
		.aliases = elems
			| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::Alias>(e); })
			| std::views::transform([&](auto&& e) { return std::get<grammar::Alias>(e); })
			| std::views::transform([&](auto&& t) { return getStruct(file_name, vec, t, copy(name_space)); })
			| to_vec(),
		.member_variables = elems
			| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::MemberVariable>(e); })
			| std::views::transform([&](auto&& e) { return std::get<grammar::MemberVariable>(e); })
			| std::views::transform([&](auto&& t) { return getStruct(file_name, vec, t); })
			| to_vec()
	};
}

NodeStructs::Enum getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Enum& e, std::optional<NodeStructs::Typename> name_space) {
	using value_t = Indent<And<IndentToken, grammar::Word, grammar::Newline>>;
	std::vector<std::string> values = {};
	auto enum_values = e.get<Star<value_t>>().get<value_t>();
	values.reserve(enum_values.size());
	for (const value_t& v : enum_values)
		values.push_back(v.get<grammar::Word>().value);
	return {
		e.get<grammar::Word>().value,
		std::move(values),
		copy(name_space)
	};
}

bool uses_auto(const NodeStructs::Function& fn);

NodeStructs::NameSpace getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::NameSpace& ns, std::optional<NodeStructs::Typename> name_space);

NodeStructs::NameSpace getNamespaceStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const auto& indent_named_range, std::optional<NodeStructs::Typename> name_space, std::optional<std::string> name) {
	std::vector<NodeStructs::Type> types;
	std::vector<NodeStructs::Function> functions;
	std::vector<NodeStructs::Interface> interfaces;
	std::vector<NodeStructs::Template> templates;
	std::vector<NodeStructs::Alias> aliases;
	std::vector<NodeStructs::NameSpace> namespaces;
	std::vector<NodeStructs::Enum> enums;

	std::optional<NodeStructs::Typename> composed_ns = [&]() {
		if (name.has_value())
			return std::optional<NodeStructs::Typename>{
				name_space.has_value() ?
					make_typename(NodeStructs::NamespacedTypename{
						std::move(name_space).value(),
						name.value(),
					}, NodeStructs::Value{}, rule_info_stub_no_throw())
					: make_typename(NodeStructs::BaseTypename{ name.value() }, NodeStructs::Value{}, rule_info_stub_no_throw())
				};
		else
			return std::move(name_space);
		}();

	for (const And<IndentToken, grammar::Named>& named : indent_named_range) {
		std::visit(
			overload(
				[&](const grammar::Type& e) {
					types.push_back(getStruct(file_name, vec, e, copy(composed_ns)));
				},
				[&](const grammar::Function& e) {
					functions.push_back(getStruct(file_name, vec, e, copy(composed_ns)));
				},
				[&](const grammar::Interface& e) {
					interfaces.push_back(getStruct(file_name, vec, e, copy(composed_ns)));
				},
				[&](const grammar::Template& e) {
					templates.push_back(getStruct(file_name, vec, e, copy(composed_ns)));
				},
				[&](const grammar::Alias& e) {
					aliases.push_back(getStruct(file_name, vec, e, copy(composed_ns)));
				},
				[&](const grammar::NameSpace& e) {
					namespaces.push_back(getStruct(file_name, vec, e, copy(composed_ns)));
				},
				[&](const grammar::Enum& e) {
					enums.push_back(getStruct(file_name, vec, e, copy(composed_ns)));
				}
			), named.get<grammar::Named>().value()
					);
	}

	return {
		.name = name.has_value() ? std::move(name).value() : "UNNAMED_NAMESPACE",
		.name_space = copy(name_space),
		.functions = functions
		| std::views::filter([&](auto&& f) { return !uses_auto(f); })
		| std::views::transform([&](auto&& fn) { return copy(fn); })
		| to_vec(),
		.functions_using_auto = functions
		| std::views::filter([&](auto&& f) { return uses_auto(f); })
		| std::views::transform([&](auto&& fn) { return copy(fn); })
		| to_vec(),
		.types = std::move(types),
		.interfaces = std::move(interfaces),
		.templates = std::move(templates),
		.blocks = {},
		.aliases = std::move(aliases),
		.enums = std::move(enums),
		.namespaces = std::move(namespaces),
	};
}

NodeStructs::NameSpace getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::NameSpace& ns, std::optional<NodeStructs::Typename> name_space) {
	return getNamespaceStruct(
		file_name,
		vec,
		ns.get<Indent<Star<Or<Token<NEWLINE>, And<IndentToken, grammar::Named>>>>>().get<And<IndentToken, grammar::Named>>(),
		std::move(name_space),
		ns.get<grammar::Word>().value
	);
}

NodeStructs::Exists getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Exists& e) {
	return NodeStructs::Exists{
		getNamespaceStruct(
			file_name,
			vec,
			e.get<Indent<Star<Or<Token<NEWLINE>, And<IndentToken, grammar::Named>>>>>().get<And<IndentToken, grammar::Named>>(),
			std::nullopt,
			std::nullopt
		)
	};
}

NodeStructs::File getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::File& f) {
	using T = Star<Or<Token<NEWLINE>, grammar::Named, grammar::Exists>>;
	auto t = f.get<T>().get_view<grammar::Named>()
		| std::views::transform([&](auto&& e) { return e.value(); })
		;

	auto functions = t
		| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::Function>(e); })
		| std::views::transform([&](auto&& e) { return std::get<grammar::Function>(e); })
		| std::views::transform([&](auto&& fn) { return getStruct(file_name, vec, fn, std::nullopt); })
		| to_vec();

	return NodeStructs::File{
		.imports = f.get<Star<grammar::Import>>().get<grammar::Import>()
			| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e); })
			| to_vec(),
		.exists = f.get<T>().get_view<grammar::Exists>()
			| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e); })
			| to_vec(),
		.content = NodeStructs::NameSpace{
			.name = std::string{ file_name },
			.functions = functions
				| std::views::filter([&](auto&& f) { return !uses_auto(f); })
				| std::views::transform([&](auto&& fn) { return copy(fn); })
				| to_vec(),
			.functions_using_auto = functions
				| std::views::filter([&](auto&& f) { return uses_auto(f); })
				| std::views::transform([&](auto&& fn) { return copy(fn); })
				| to_vec(),
			.types = t
				| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::Type>(e); })
				| std::views::transform([&](auto&& e) { return std::get<grammar::Type>(e); })
				| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e, std::nullopt); })
				| to_vec(),
			.interfaces = t
				| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::Interface>(e); })
				| std::views::transform([&](auto&& e) { return std::get<grammar::Interface>(e); })
				| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e, std::nullopt); })
				| to_vec(),
			.templates = t
				| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::Template>(e); })
				| std::views::transform([&](auto&& e) { return std::get<grammar::Template>(e); })
				| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e, std::nullopt); })
				| to_vec(),
			.aliases = t
				| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::Alias>(e); })
				| std::views::transform([&](auto&& e) { return std::get<grammar::Alias>(e); })
				| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e, std::nullopt); })
				| to_vec(),
			.enums = t
				| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::Enum>(e); })
				| std::views::transform([&](auto&& e) { return std::get<grammar::Enum>(e); })
				| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e, std::nullopt); })
				| to_vec(),
			.namespaces = t
				| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::NameSpace>(e); })
				| std::views::transform([&](auto&& e) { return std::get<grammar::NameSpace>(e); })
				| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e, std::nullopt); })
				| to_vec()
		}
	};
}

NodeStructs::ArgumentCategory getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const Or<Token<MOVE>, And<Token<REF>, Token<NOT>>, Token<REF>>& t) {
	return std::visit(overload(overload_default_error,
		[&](const Token<MOVE>&) -> NodeStructs::ArgumentCategory {
			return NodeStructs::Move{};
		},
		[&](const And<Token<REF>, Token<NOT>>&) -> NodeStructs::ArgumentCategory {
			return NodeStructs::MutableReference{};
		},
		[&](const Token<REF>&) -> NodeStructs::ArgumentCategory {
			return NodeStructs::Reference{};
		}),
		t.value()
	);
}

NodeStructs::FunctionArgument getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::FunctionArgument& arg) {
	using call_t = Or<Token<MOVE>, And<Token<REF>, Token<NOT>>, Token<REF>>;
	if (arg.get<Opt<call_t>>().has_value())
		return NodeStructs::FunctionArgument{
			getStruct(file_name, vec, arg.get<Opt<call_t>>().value()),
			getExpressionStruct(file_name, vec, arg.get<grammar::Expression>())
	};
	else
		return NodeStructs::FunctionArgument{
			{},
			getExpressionStruct(file_name, vec, arg.get<grammar::Expression>())
	};
}

NodeStructs::ParenArguments getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::ParenArguments& args) {
	return {
		args.get<CommaStar<grammar::FunctionArgument>>().get_view<grammar::FunctionArgument>()
			| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e); })
			| to_vec()
	};
}

NodeStructs::BracketArguments getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::BracketArguments& args) {
	return {
		args.get<CommaStar<grammar::FunctionArgument>>().get_view<grammar::FunctionArgument>()
			| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e); })
			| to_vec()
	};
}

NodeStructs::BraceArguments getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::BraceArguments& args) {
	return {
		args.get<CommaStar<grammar::FunctionArgument>>().get_view<grammar::FunctionArgument>()
			| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e); })
			| to_vec()
	};
}

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::BraceArguments&) {
	throw;
	/*NodeStructs::BraceArguments res;
	for (const auto& arg : statement.get<CommaStar<FunctionArgument>>().get<FunctionArgument>())
		res.args.push_back(getExpressionStruct(arg));
	return res;*/
}

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::ParenExpression& statement) {
	return std::visit(overload(overload_default_error,
		[&](const grammar::Construct& e) -> NodeStructs::Expression {
			return make_expression(NodeStructs::ConstructExpression{
				.operand = getStruct(file_name, vec, e.get<grammar::Typename>(), tag_expect_empty_category{}),
				.arguments = e.get<grammar::BraceArguments>().get<CommaStar<grammar::FunctionArgument>>().get<grammar::FunctionArgument>()
				| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e); })
				| to_vec()
			}, rule_info_from_rule(file_name, vec, e));
		},
		[&](const grammar::ParenArguments& e) -> NodeStructs::Expression {
			return make_expression(NodeStructs::ParenArguments{
				.args = e.get<CommaStar<grammar::FunctionArgument>>().get<grammar::FunctionArgument>()
				| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e); })
				| to_vec()
			}, rule_info_from_rule(file_name, vec, e));
		},
		[&](const grammar::BracketArguments& /*e*/) -> NodeStructs::Expression {
			//const auto& args = e.get<CommaStar<FunctionArgument>>().get<FunctionArgument>();
			throw;
			//auto res = getExpressionStruct(e);
			//return make_expression({ std::move(res) });
		},
		[&](const grammar::BraceArguments& e) -> NodeStructs::Expression {
			const auto& args = e.get<CommaStar<grammar::FunctionArgument>>().get<grammar::FunctionArgument>();
			return make_expression(NodeStructs::BraceArguments{
				args
				| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e); })
				| to_vec()
			}, rule_info_from_rule(file_name, vec, e));
			//auto res = getExpressionStruct(e);
			//return make_expression({ std::move(res) });
		},
		[&](const grammar::Word& e) {
			return make_expression(e.value, rule_info_from_rule(file_name, vec, e));
		},
		[&](const Token<INTEGER_NUMBER>& e) {
			return make_expression(e, rule_info_from_rule(file_name, vec, e));
		},
		[&](const Token<FLOATING_POINT_NUMBER>& e) {
			return make_expression(e, rule_info_from_rule(file_name, vec, e));
		},
		[&](const Token<STRING>& e) {
			return make_expression(e, rule_info_from_rule(file_name, vec, e));
		}
	),
		statement.value()
	);
}

NodeStructs::Expression getPostfixExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, NodeStructs::Expression&& expr, const grammar::Postfix& postfix) {
	return std::visit(
		overload(overload_default_error,
			[&](const And<Token<DOT>, grammar::Word>& e) {
				return make_expression(
					NodeStructs::PropertyAccessExpression{ std::move(expr), e.get<grammar::Word>().value },
					rule_info_from_rule(file_name, vec, e)
				);
			},
			[&](const And<Token<NS>, grammar::Word>& e) {
				return make_expression(NodeStructs::NamespaceExpression{ std::move(expr), e.get<grammar::Word>().value },
				rule_info_from_rule(file_name, vec, e)
				);
			},
			[&](const And<Token<DOT>, grammar::Word, grammar::ParenArguments>& e) {
				return make_expression(
					NodeStructs::PropertyAccessAndCallExpression{
						std::move(expr),
						e.get<grammar::Word>().value,
						getStruct(file_name, vec, e.get<grammar::ParenArguments>())
					}, rule_info_from_rule(file_name, vec, e)
				);
			},
			[&](const grammar::ParenArguments& args) {
				auto args_ = getStruct(file_name, vec, args);
				return make_expression(NodeStructs::CallExpression{ std::move(expr), std::move(args_) }, rule_info_from_rule(file_name, vec, args));
			},
			[&](const grammar::BracketArguments& args) {
				auto args_ = getStruct(file_name, vec, args);
				return make_expression(NodeStructs::BracketAccessExpression{ std::move(expr), std::move(args_) }, rule_info_from_rule(file_name, vec, args));
			},
			[&](const grammar::TemplateTypenameExtension& args) {
				return make_expression(NodeStructs::TemplateExpression{
					std::move(expr),
					args.get<CommaStar<grammar::TypenameOrExpression>>().get<grammar::TypenameOrExpression>()
					| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e, tag_allow_value_category_or_empty{}); })
					| to_vec()
				}, rule_info_from_rule(file_name, vec, args));
			}
		),
		postfix.value()
	);
}

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::PostfixExpression& expr, NodeStructs::Expression cur, const auto& expressions, size_t i) {
	if (i == expressions.size())
		return std::move(cur);
	return getExpressionStruct(file_name, vec, expr, getPostfixExpressionStruct(file_name, vec, std::move(cur), expressions.at(i)), expressions, i + 1);
}

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::PostfixExpression& expr) {
	const auto& postfixes = expr.get<Star<grammar::Postfix>>().get<grammar::Postfix>();
	auto base = getExpressionStruct(file_name, vec, expr.get<grammar::ParenExpression>());
	return getExpressionStruct(file_name, vec, expr, std::move(base), postfixes, 0);
}

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::UnaryExpression& expr) {
	const auto& prefixes = expr.get().get<Star<grammar::unary_operators>>().get<grammar::unary_operators>();
	if (prefixes.size() == 0)
		return getExpressionStruct(file_name, vec, expr.get().get<grammar::PostfixExpression>());
	else {
		return make_expression(NodeStructs::UnaryExpression {
			.unary_operators = prefixes | std::views::transform(
				[&](const auto& e) {
					return std::visit(
						[&](const auto& token) {
							return NodeStructs::UnaryExpression::op_types{ token };
						},
						e.value()
					);
				})
			| to_vec(),
			.expr = getExpressionStruct(file_name, vec, expr.get().get<grammar::PostfixExpression>())
		}, rule_info_from_rule(file_name, vec, expr));
	}
}

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::MultiplicativeExpression& expr) {
	using operators = Or<Token<ASTERISK>, Token<SLASH>, Token<PERCENT>>;
	using VT = Variant<Token<ASTERISK>, Token<SLASH>, Token<PERCENT>>;
	const auto& multiplications = expr.get<Star<And<operators, grammar::UnaryExpression>>>().get<And<operators, grammar::UnaryExpression>>();
	if (multiplications.size() == 0)
		return getExpressionStruct(file_name, vec, expr.get<grammar::UnaryExpression>());
	else
		return make_expression(NodeStructs::MultiplicativeExpression{
			getExpressionStruct(file_name, vec, expr.get<grammar::UnaryExpression>()),
			multiplications
				| std::views::transform([&](auto&& op_exp) {
					return std::pair{
						VT{ op_exp.get<operators>().value() },
						getExpressionStruct(file_name, vec, op_exp.get<grammar::UnaryExpression>())
					};
				})
				| to_vec()
		}, rule_info_from_rule(file_name, vec, expr));
}

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::AdditiveExpression& expr) {
	using operators = Or<Token<PLUS>, Token<DASH>>;
	using VT = Variant<Token<PLUS>, Token<DASH>>;
	const auto& additions = expr.get<Star<And<operators, grammar::MultiplicativeExpression>>>().get<And<operators, grammar::MultiplicativeExpression>>();
	if (additions.size() == 0)
		return getExpressionStruct(file_name, vec, expr.get<grammar::MultiplicativeExpression>());
	else
		return make_expression(NodeStructs::AdditiveExpression{
			getExpressionStruct(file_name, vec, expr.get<grammar::MultiplicativeExpression>()),
			additions
				| std::views::transform([&](auto&& op_exp) {
					return std::pair{
						VT{ op_exp.get<operators>().value() },
						getExpressionStruct(file_name, vec, op_exp.get<grammar::MultiplicativeExpression>())
					};
				})
				| to_vec()
		}, rule_info_from_rule(file_name, vec, expr));
}

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::CompareExpression& expr) {
	using op_add = And<grammar::CompareOperator, grammar::AdditiveExpression>;
	using VT = Variant<Token<LT>, Token<LTE>, Token<GT>, Token<GTE>>;
	const auto& comparisons = expr.get<Star<op_add>>().get<op_add>();
	if (comparisons.size() == 0)
		return getExpressionStruct(file_name, vec, expr.get<grammar::AdditiveExpression>());
	else
		return make_expression(NodeStructs::CompareExpression{
			getExpressionStruct(file_name, vec, expr.get<grammar::AdditiveExpression>()),
			comparisons
				| std::views::transform([&](const op_add& op_exp) {
					return std::pair{
						VT{ op_exp.get<grammar::CompareOperator>().get<Or<Token<LT>, Token<LTE>, Token<GT>, Token<GTE>>>().value() },
						getExpressionStruct(file_name, vec, op_exp.get<grammar::AdditiveExpression>())
					};
				})
				| to_vec()
		}, rule_info_from_rule(file_name, vec, expr));
}

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::EqualityExpression& expr) {
	using operators = Or<Token<EQUALEQUAL>, Token<NEQUAL>>;
	using VT = Variant<Token<EQUALEQUAL>, Token<NEQUAL>>;
	const auto& equals = expr.get<Star<And<operators, grammar::CompareExpression>>>().get<And<operators, grammar::CompareExpression>>();
	if (equals.size() == 0)
		return getExpressionStruct(file_name, vec, expr.get<grammar::CompareExpression>());
	else
		return make_expression(NodeStructs::EqualityExpression{
			getExpressionStruct(file_name, vec, expr.get<grammar::CompareExpression>()),
			equals
				| std::views::transform([&](auto&& op_exp) {
					return std::pair{
						VT{ op_exp.get<operators>().value() },
						getExpressionStruct(file_name, vec, op_exp.get<grammar::CompareExpression>())
					};
				})
				| to_vec()
		}, rule_info_from_rule(file_name, vec, expr));
}

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::AndExpression& expr) {
	const auto& ands = expr.get<Star<And<Token<AND>, grammar::EqualityExpression>>>().get<grammar::EqualityExpression>();
	if (ands.size() == 0)
		return getExpressionStruct(file_name, vec, expr.get<grammar::EqualityExpression>());
	else
		return make_expression(NodeStructs::AndExpression{
			getExpressionStruct(file_name, vec, expr.get<grammar::EqualityExpression>()),
			ands
				| std::views::transform([&](auto& e) { return getExpressionStruct(file_name, vec, e); })
				| to_vec()
		}, rule_info_from_rule(file_name, vec, expr));
}

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::OrExpression& expr) {
	const auto& ors = expr.get<Star<And<Token<OR>, grammar::AndExpression>>>().get<grammar::AndExpression>();
	if (ors.size() == 0)
		return getExpressionStruct(file_name, vec, expr.get<grammar::AndExpression>());
	else
		return make_expression(NodeStructs::OrExpression{
			getExpressionStruct(file_name, vec, expr.get<grammar::AndExpression>()),
			ors
				| std::views::transform([&](auto& e) { return getExpressionStruct(file_name, vec, e); })
				| to_vec()
		}, rule_info_from_rule(file_name, vec, expr));
}

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::ConditionalExpression& expr) {
	const auto& ifElseExpr = expr.get<Opt<And<
		Token<IF>,
		grammar::OrExpression,
		Token<ELSE>,
		grammar::OrExpression
		>>>();
	if (ifElseExpr.has_value())
		return make_expression(NodeStructs::ConditionalExpression{
			getExpressionStruct(file_name, vec, expr.get<grammar::OrExpression>()),
			std::pair{
				getExpressionStruct(file_name, vec, ifElseExpr.value().get<grammar::OrExpression, 0>()),
				getExpressionStruct(file_name, vec, ifElseExpr.value().get<grammar::OrExpression, 1>())
			}
		}, rule_info_from_rule(file_name, vec, expr));
	else
		return getExpressionStruct(file_name, vec, expr.get<grammar::OrExpression>());
}

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::ExpressionStatement& statement) {
	return getExpressionStruct(file_name, vec, statement.get<grammar::Expression>());
}

NodeStructs::Expression getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::ExpressionStatement& statement) {
	return getExpressionStruct(file_name, vec, statement);
}

auto getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const Expect<grammar::Statement>& statement) {
	const grammar::Statement& st = statement;
	return getStatementStruct(file_name, vec, st);
}

NodeStructs::VariableDeclarationStatement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::VariableDeclarationStatement& statement) {
	return {
		getStruct(file_name, vec, statement.get<grammar::Typename>(), tag_allow_value_category_or_empty{}),
		statement.get<grammar::Word>().value, getExpressionStruct(file_name, vec, statement.get<grammar::Expression>())
	};
}

std::vector<NodeStructs::Statement> getStatements(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::ColonIndentCodeBlock& code) {
	return code.get<Indent<Star<Or<Token<NEWLINE>, Expect<grammar::Statement>>>>>().get<Or<Token<NEWLINE>, Expect<grammar::Statement>>>()
		| std::views::transform(
			[&](const Or<Token<NEWLINE>, Expect<grammar::Statement>>& e)
			-> const std::variant<Token<NEWLINE>, Expect<grammar::Statement>>&{
				return e.value();
			})
		| std::views::filter([&](auto&& e) { return std::holds_alternative<Expect<grammar::Statement>>(e); })
				| std::views::transform([&](auto&& e) { return std::get<Expect<grammar::Statement>>(e); })
				| std::views::transform([&](const Expect<grammar::Statement>& e) { return getStatementStruct(file_name, vec, e); })
				| to_vec();
}

NodeStructs::BlockStatement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::BlockStatement& statement) {
	// wtf even is this right now it doesn't make sense
	throw;
	//return { getStruct(statement.get<grammar::Typename>()) };
}

NodeStructs::IfStatement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::IfStatement& statement) {
	using T = Variant<NonCopyableBox<NodeStructs::IfStatement>, std::vector<NodeStructs::Statement>>;
	return {
		getExpressionStruct(file_name, vec, statement.get<grammar::Expression>()),
		getStatements(file_name, vec, statement.get<grammar::ColonIndentCodeBlock>()),
		statement.get<Opt<Alloc<grammar::ElseStatement>>>().node.transform([&](const auto& e) -> T {
			return std::visit(
				overload(overload_default_error,
					[&](const Alloc<grammar::IfStatement>& e) -> T {
						return getStatementStruct(file_name, vec, e.get());
					},
					[&](const grammar::ColonIndentCodeBlock& e) -> T {
						return getStatements(file_name, vec, e);
					}
				),
				e.get()
				.get<Or<Alloc<grammar::IfStatement>, grammar::ColonIndentCodeBlock>>()
				.value()
			);
		})
	};
}

NodeStructs::ForStatement getForStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const auto& statement) {
	return {
		.collection = getExpressionStruct(file_name, vec, statement.get<grammar::Expression>()),
		.iterators = statement.get<CommaPlus<Or<grammar::VariableDeclaration, grammar::Word>>>().get<Or<grammar::VariableDeclaration, grammar::Word>>()
			| std::views::transform([&](const Or<grammar::VariableDeclaration, grammar::Word>& or_node) {
				return std::visit(overload(
					[&](const grammar::Word& e) -> Variant<NodeStructs::VariableDeclaration, std::string> {
						return { e.value };
					},
					[&](const grammar::VariableDeclaration& e) -> Variant<NodeStructs::VariableDeclaration, std::string> {
						return NodeStructs::VariableDeclaration{
							getStruct(file_name, vec, e.get<grammar::Typename>(), tag_allow_value_category_or_empty{}),
							e.get<grammar::Word>().value
						};
					}
				), or_node.value());
			})
			| to_vec(),
		.statements = getStatements(file_name, vec, statement.get<grammar::ColonIndentCodeBlock>())
	};
}

NodeStructs::ForStatement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::ForStatement& statement) {
	return getForStatementStruct(file_name, vec, statement);
}

NodeStructs::IForStatement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::IForStatement& statement) {
	return {
		statement.get<grammar::Word>().value,
		getForStatementStruct(file_name, vec, statement)
	};
}

NodeStructs::WhileStatement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::WhileStatement& statement) {
	return {
		getExpressionStruct(file_name, vec, statement.get<grammar::Expression>()),
		getStatements(file_name, vec, statement.get<grammar::ColonIndentCodeBlock>())
	};
}

NodeStructs::MatchCase getCase(const std::string& file_name, const std::vector<TokenValue>& vec, const auto& typenames, const auto& statements) {
	return {
		typenames
		| std::views::transform([&](auto&& variable_declaration) {
			return std::pair{
				getStruct(file_name, vec, variable_declaration.get<grammar::Typename>(), tag_allow_value_category_or_empty{}),
				variable_declaration.get<grammar::Word>().value
			}; })
		| to_vec(),
		statements
		| std::views::transform([&](auto&& e) { return e.value(); })
		| std::views::filter([&](auto&& e) { return std::holds_alternative<Expect<grammar::Statement>>(e); })
		| std::views::transform([&](auto&& e) { return std::get<Expect<grammar::Statement>>(e); })
		| std::views::transform([&](auto&& e) { return getStatementStruct(file_name, vec, e); })
		| to_vec()
	};
}

std::vector<NodeStructs::MatchCase> getCases(const std::string& file_name, const std::vector<TokenValue>& vec, const Indent<Plus<And<IndentToken, CommaPlus<grammar::VariableDeclaration>, grammar::ColonIndentCodeBlock>>>& cases) {
	std::vector<NodeStructs::MatchCase> res;
	res.reserve(cases.nodes.size());
	for (const auto& and_node : cases.nodes) {
		const auto& [_, typenames, statements] = and_node.value;
		res.push_back(getCase(
			file_name,
			vec,
			typenames.nodes,
			statements.get<Indent<Star<Or<Token<NEWLINE>, Expect<grammar::Statement>>>>>().get_view<Or<Token<NEWLINE>, Expect<grammar::Statement>>>()
		));
	}
	return res;
}

NodeStructs::MatchStatement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::MatchStatement& statement) {
	return {
		statement.get<CommaPlus<grammar::Expression>>().get_view<grammar::Expression>()
			| std::views::transform([&](auto&& e) { return getExpressionStruct(file_name, vec, e); })
			| to_vec(),
		getCases(file_name, vec, statement.get<Indent<Plus<And<
			IndentToken,
			CommaPlus<grammar::VariableDeclaration>,
			grammar::ColonIndentCodeBlock
		>>>>())
	};
}

NodeStructs::BreakStatement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::BreakStatement& statement) {
	return {
		statement.get<Opt<And<Token<IF>, grammar::Expression>>>().has_value()
			? getExpressionStruct(file_name, vec, statement.get<Opt<And<Token<IF>, grammar::Expression>>>().value().get<grammar::Expression>())
			: std::optional<NodeStructs::Expression>{}
	};
}

std::vector<NodeStructs::Expression> getExpressions(
	const std::string& file_name, const std::vector<TokenValue>& vec, const std::vector<grammar::Expression>& expressions
) {
	return expressions
		| std::views::transform([&](auto&& e) { return getExpressionStruct(file_name, vec, e); })
		| to_vec();
}

NodeStructs::ReturnStatement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::ReturnStatement& statement) {
	std::vector<NodeStructs::FunctionArgument> returns = statement.get<CommaStar<grammar::FunctionArgument>>().get<grammar::FunctionArgument>()
		| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e); })
		| to_vec();
	return {
		std::move(returns),
		statement.get<Opt<And<Token<IF>, grammar::Expression>>>().has_value()
			? getExpressionStruct(file_name, vec, statement.get<Opt<And<Token<IF>, grammar::Expression>>>().value().get<grammar::Expression>())
			: std::optional<NodeStructs::Expression>{}
	};
}

NodeStructs::SwitchStatement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::SwitchStatement& statement) {
	return NodeStructs::SwitchStatement{
		.expr = getExpressionStruct(file_name, vec, statement.get<grammar::Expression>()),
		.cases = statement.get<Indent<Star<And<
			IndentToken,
			grammar::Expression,
			grammar::ColonIndentCodeBlock
		>>>>().get<And<IndentToken, grammar::Expression, grammar::ColonIndentCodeBlock>>()
		| std::views::transform([&](const And<IndentToken, grammar::Expression, grammar::ColonIndentCodeBlock>& switch_case) {
			return NodeStructs::SwitchCase{
				getExpressionStruct(file_name, vec, switch_case.get<grammar::Expression>()),
				getStatements(file_name, vec, switch_case.get<grammar::ColonIndentCodeBlock>())
			};
		})
		| to_vec()
	};
}

NodeStructs::Assignment getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Assignment& statement) {
	return { getExpressionStruct(file_name, vec, statement.get<grammar::Expression, 0>()), getExpressionStruct(file_name, vec, statement.get<grammar::Expression, 1>()) };
}

NodeStructs::Statement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Statement& statement) {
	return NodeStructs::Statement{
		std::visit(
			[&](const auto& statement) -> NodeStructs::Statement::vt {
				return { getStatementStruct(file_name, vec, statement) };
			},
			statement.get<grammar::StatementOpts>().get().value()
		),
		statement.get<Opt<Token<POUND>>>().has_value()
	};
}

NodeStructs::WordTypenameOrExpression getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::TypenameOrExpression& t, tag_expect_value_category) {
	return getTypenameOrExpressionStruct<tag_expect_value_category>(file_name, vec, t);
}

NodeStructs::WordTypenameOrExpression getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::TypenameOrExpression& t, tag_expect_empty_category) {
	return getTypenameOrExpressionStruct<tag_expect_empty_category>(file_name, vec, t);
}

NodeStructs::WordTypenameOrExpression getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::TypenameOrExpression& t, tag_allow_value_category_or_empty) {
	return getTypenameOrExpressionStruct<tag_allow_value_category_or_empty>(file_name, vec, t);
}

std::string accumulate_content(const std::vector<TokenValue>& vec, const unsigned& beg, const unsigned& end) {
	std::stringstream ss;
	auto it = beg;
	while (it != end)
		ss << vec[it++].second;
	return ss.str();
}
