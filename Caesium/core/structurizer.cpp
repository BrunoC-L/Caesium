#pragma once
#include "structurizer.hpp"
#include <iostream>

static std::string word_or_auto(const Or<Token<AUTO>, grammar::Word>& tk) {
	return std::visit(
		overload(
			[](const Token<AUTO>& _auto) -> std::string {
				return "auto";
			},
			[](const grammar::Word& w) -> std::string {
				return w.value;
			}
		), tk.value()
	);
}

NodeStructs::Import getStruct(const grammar::Import& f) {
	return { std::visit(
		overload(overload_default_error,
			[](const grammar::Word& word) {
				return word.value + ".caesium";
			},
			[](const grammar::String& string) {
				return string.value;
		}),
		f.get<Or<grammar::Word, grammar::String>>().value()
	) };
}

template <typename tag_t>
NodeStructs::WordTypenameOrExpression getTypenameOrExpressionStruct(const grammar::TypenameOrExpression& tn_or_expr) {
	return std::visit(overload(
		[&](const grammar::Typename& tn) -> NodeStructs::WordTypenameOrExpression {
			bool has_to_be_interpreted_as_typename_because_of_category_or_optional =
				tn.get<Opt<grammar::ParameterCategory>>().has_value() || tn.get<Opt<Token<QUESTION>>>().has_value();
			if (has_to_be_interpreted_as_typename_because_of_category_or_optional)
				return { getStruct(tn, tag_t{}) };
			else
				return std::visit(overload(
					[&](const Token<AUTO>&) -> NodeStructs::WordTypenameOrExpression { // auto is a typename
						return { getStruct(tn, tag_t{}) };
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
							return { getStruct(tn, tag_t{}) };

						return std::visit(overload(
							[&](const grammar::VariadicExpansionTypename&) -> NodeStructs::WordTypenameOrExpression { // X... is a typename
								return { getStruct(tn, tag_t{}) };
							},
							[&](const grammar::Word& word) -> NodeStructs::WordTypenameOrExpression {
								return { word.value };
							}
						), non_auto_typename.get<Or<grammar::VariadicExpansionTypename, grammar::Word>>().value());
					}
				), tn.get<Or<Token<AUTO>, grammar::NonAutoTypename>>().value());
		},
		[&](const grammar::Expression& expr) -> NodeStructs::WordTypenameOrExpression {
			return { getExpressionStruct(expr) };
		}
	), tn_or_expr.value());
}

NodeStructs::Typename getStruct(const grammar::Typename& t, const auto& exts, NodeStructs::Typename res, size_t i) {
	if (i == exts.size())
		return std::move(res);
	const auto& ext = exts.at(i);
	return std::visit(overload(
		[&](const NodeStructs::VariadicExpansionTypename&) {
			return getStruct(t, exts, std::visit(overload(overload_default_error,
				[&](const grammar::NamespaceTypenameExtension& e) -> NodeStructs::Typename {
					throw;
				},
				[&](const grammar::TemplateTypenameExtension& e) -> NodeStructs::Typename {
					return NodeStructs::Typename{
						NodeStructs::VariadicExpansionTypename {
							NodeStructs::Typename{
								NodeStructs::TemplatedTypename {
									std::get<NodeStructs::VariadicExpansionTypename>(std::move(res).value._value).type,
									e.get<CommaStar<grammar::TypenameOrExpression>>().get<grammar::TypenameOrExpression>()
									| std::views::transform([&](const grammar::TypenameOrExpression& e) { return getStruct(e, tag_expect_empty_category{}); })
									| to_vec()
								}, NodeStructs::Value{}
							}
						}, NodeStructs::Value{}
					};
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
			return getStruct(t, exts, std::visit(overload(overload_default_error,
			[&](const grammar::NamespaceTypenameExtension& e) -> NodeStructs::Typename {
					return { NodeStructs::NamespacedTypename{ std::move(res), e.get<grammar::Word>().value }, NodeStructs::Value{} };
				},
				[&](const grammar::TemplateTypenameExtension& e) -> NodeStructs::Typename {
					return { NodeStructs::TemplatedTypename{
						std::move(res),
						e.get<CommaStar<grammar::TypenameOrExpression>>().get<grammar::TypenameOrExpression>()
						| std::views::transform([&](auto&& e) { return getStruct(e, tag_allow_value_category_or_empty{}); })
						| to_vec()
					}, NodeStructs::Value{} };
				},
				[&](const grammar::UnionTypenameExtension& ext) -> NodeStructs::Typename {
					auto temp = getStruct(ext.get<Alloc<grammar::Typename>>().get(), tag_allow_value_category_or_empty{});
					if (std::holds_alternative<NodeStructs::UnionTypename>(res.value._value)) {
						if (std::holds_alternative<NodeStructs::UnionTypename>(temp.value._value))
							throw;
						std::get<NodeStructs::UnionTypename>(res.value._value).ors.push_back(std::move(temp));
						return std::move(res);
					}
					if (std::holds_alternative<NodeStructs::UnionTypename>(temp.value._value)) {
						auto& ut = std::get<NodeStructs::UnionTypename>(temp.value._value);
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
					return { NodeStructs::UnionTypename{
						std::move(v)
					}, NodeStructs::Value{} };
				},
				[&](const Token<QUESTION>& ext) -> NodeStructs::Typename {
					return { NodeStructs::OptionalTypename{ std::move(res) }, NodeStructs::Value{} };
				}
	), ext.value()), i + 1);
		}
	), res.value._value);
}

template <typename tag>
NodeStructs::Typename get_typename_struct(const grammar::Typename& t) {
	const auto& auto_or_tn = t.get<Or<Token<AUTO>, grammar::NonAutoTypename>>();
	NodeStructs::ParameterCategory value_cat = getStruct(t.get<Opt<grammar::ParameterCategory>>(), tag{});
	const auto& optional = t.get<Opt<Token<QUESTION>>>();

	return std::visit(overload(
		[&](const Token<AUTO>&) -> NodeStructs::Typename {
			return { NodeStructs::BaseTypename{ "auto" }, std::move(value_cat) };
		},
		[&](const grammar::NonAutoTypename& e) -> NodeStructs::Typename {
			NodeStructs::Typename res = std::visit(overload(
				[&](const grammar::Word& w) -> NodeStructs::Typename {
					return { NodeStructs::BaseTypename{ w.value }, NodeStructs::Value{} };
				},
				[&](const grammar::VariadicExpansionTypename& vetn) -> NodeStructs::Typename {
					return NodeStructs::Typename{
						NodeStructs::VariadicExpansionTypename {
							NodeStructs::Typename{ NodeStructs::BaseTypename{ vetn.get<grammar::Word>().value }, NodeStructs::Value{} }
						},
						NodeStructs::Value{}
					};
				}
			), e.get<Or<grammar::VariadicExpansionTypename, grammar::Word>>().value());
			using opts = Or<grammar::NamespaceTypenameExtension, grammar::TemplateTypenameExtension, grammar::UnionTypenameExtension, Token<QUESTION>>;
			auto res2 = getStruct(t, e.get<Star<opts>>().get<opts>(), std::move(res), 0);
			return { std::move(res2.value), std::move(value_cat) };
		}
	), auto_or_tn.value());
}

NodeStructs::Typename getStruct(const grammar::Typename& t, tag_expect_value_category) {
	return get_typename_struct<tag_expect_value_category>(t);
}

NodeStructs::Typename getStruct(const grammar::Typename& t, tag_expect_empty_category) {
	return get_typename_struct<tag_expect_empty_category>(t);
}

NodeStructs::Typename getStruct(const grammar::Typename& t, tag_allow_value_category_or_empty) {
	return get_typename_struct<tag_allow_value_category_or_empty>(t);
}

NodeStructs::ParameterCategory getStruct(const grammar::ParameterCategory& vc) {
	return std::visit(
		overload(overload_default_error,
			/*[](const Token<KEY>&) -> NodeStructs::ValueCategory {
				return NodeStructs::Key{};
			},*/
			[](const Token<VAL>&) -> NodeStructs::ParameterCategory {
				return NodeStructs::Value{};
			},
			[](const Token<REF>&) -> NodeStructs::ParameterCategory {
				return NodeStructs::Reference{};
			},
			[](const And<Token<REF>, Token<NOT>>&) -> NodeStructs::ParameterCategory {
				return NodeStructs::MutableReference{};
			}
		),
		vc.value()
	);
}

NodeStructs::ParameterCategory getStruct(const Opt<grammar::ParameterCategory>& vc, tag_expect_value_category) {
	if (vc.has_value())
		return getStruct(vc.value());
	else
		throw;
}

NodeStructs::ParameterCategory getStruct(const Opt<grammar::ParameterCategory>& vc, tag_expect_empty_category) {
	if (vc.has_value())
		throw;
	return NodeStructs::Value{};
}

NodeStructs::ParameterCategory getStruct(const Opt<grammar::ParameterCategory>& vc, tag_allow_value_category_or_empty) {
	if (vc.has_value())
		return getStruct(vc.value());
	else
		return NodeStructs::Value{};
}

NodeStructs::Function structurize_function(const grammar::Function& f, std::optional<NodeStructs::Typename> name_space) {
	return NodeStructs::Function{
		.name = f.get<grammar::Word>().value,
		.name_space = std::move(name_space),
		.returnType = getStruct(f.get<grammar::Typename>(), tag_allow_value_category_or_empty{}),
		.parameters = f.get<grammar::FunctionParameters>().get<And<Commit<grammar::Typename>, grammar::Word>>()
			| std::views::transform([&](const grammar::FunctionParameter& type_and_name) -> NodeStructs::FunctionParameter {
				auto res = NodeStructs::FunctionParameter{
					getStruct(type_and_name.get<Commit<grammar::Typename>>(), tag_expect_value_category{}),
					type_and_name.get<grammar::Word>().value
				};
				if (!res.typename_.category._value.has_value())
					throw;
				return res;
			})
			| to_vec(),
		.statements = getStatements(f.get<grammar::ColonIndentCodeBlock>())
	};
}

Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter> getTemplateParameter(
	const And<grammar::Word, Token<DOTS>>& parameter
) {
	return NodeStructs::VariadicTemplateParameter{ parameter.get<grammar::Word>().value };
}

Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter> getTemplateParameter(
	const And<grammar::Word, Opt<And<Token<EQUAL>, grammar::Expression>>>& parameter_and_optional_value
) {
	if (parameter_and_optional_value.get<Opt<And<Token<EQUAL>, grammar::Expression>>>().has_value())
		return NodeStructs::TemplateParameterWithDefaultValue{
			parameter_and_optional_value.get<grammar::Word>().value,
			getExpressionStruct(parameter_and_optional_value.get<Opt<And<Token<EQUAL>, grammar::Expression>>>().value().get<grammar::Expression>())
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
	const parameter_t& parameter
) {
	return std::visit([](const auto& e) { return getTemplateParameter(e); }, parameter.value());
}

NodeStructs::Template getStruct(const grammar::Template& t) {
	return getStruct(t, std::nullopt);
}

NodeStructs::Template getStruct(const grammar::Template& t, std::optional<NodeStructs::Typename> name_space) {
	using parameters_t = CommaStar<parameter_t>;
	return {
		.name = t.get<grammar::Word>().value,
		.name_space = std::move(name_space),
		.parameters = t.get<parameters_t>().get<parameter_t>()
			| std::views::transform([&](auto&& e) { return getTemplateParameter(e); })
			| to_vec(),
		.templated = t.get<TemplateBody>().value,
		.indent = t.n_indent + 1
	};
}

NodeStructs::MemberVariable getStruct(const grammar::MemberVariable& f) {
	return NodeStructs::MemberVariable{
		.type = getStruct(f.get<grammar::Typename>(), tag_allow_value_category_or_empty{}),
		.name = f.get<grammar::Word>().value,
	};
}

NodeStructs::Alias getStruct(const grammar::Alias& f, std::optional<NodeStructs::Typename> name_space) {
	return NodeStructs::Alias{
		.aliasFrom = f.get<grammar::Word>().value,
		.aliasTo = getStruct(f.get<grammar::Typename>(), tag_allow_value_category_or_empty{}),
		.name_space = std::move(name_space)
	};
}

NodeStructs::Type getStruct(const grammar::Type& t, std::optional<NodeStructs::Typename> name_space) {
	auto elems = t.get<Indent<Star<And<IndentToken, grammar::TypeElement>>>>().get_view<grammar::TypeElement>()
		| std::views::transform([&](auto&& e) { return e.value(); });
	return {
		.name = t.get<grammar::Word>().value,
		.name_space = copy(name_space),
		.aliases = elems
			| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::Alias>(e); })
			| std::views::transform([&](auto&& e) { return std::get<grammar::Alias>(e); })
			| std::views::transform([&](auto&& t) { return getStruct(t, copy(name_space)); })
			| to_vec(),
		.member_variables = elems
			| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::MemberVariable>(e); })
			| std::views::transform([&](auto&& e) { return std::get<grammar::MemberVariable>(e); })
			| std::views::transform([&](auto&& t) { return getStruct(t); })
			| to_vec()
	};
}

NodeStructs::Interface getStruct(const grammar::Interface& interface, std::optional<NodeStructs::Typename> name_space) {
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
			| std::views::transform([&](auto&& t) { return getStruct(t, copy(name_space)); })
			| to_vec(),
		.member_variables = elems
			| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::MemberVariable>(e); })
			| std::views::transform([&](auto&& e) { return std::get<grammar::MemberVariable>(e); })
			| std::views::transform([&](auto&& t) { return getStruct(t); })
			| to_vec()
	};
}

NodeStructs::Enum getStruct(const grammar::Enum& e, std::optional<NodeStructs::Typename> name_space) {
	using value_t = Indent<And<IndentToken, grammar::Word, grammar::Newline>>;
	std::vector<std::string> values = {};
	auto vec = e.get<Star<value_t>>().get<value_t>();
	values.reserve(vec.size());
	for (const value_t& v : vec)
		values.push_back(v.get<grammar::Word>().value);
	return {
		e.get<grammar::Word>().value,
		std::move(values),
		copy(name_space)
	};
}

bool uses_auto(const NodeStructs::Function& fn);

NodeStructs::NameSpace getStruct(const grammar::NameSpace& ns, std::optional<NodeStructs::Typename> name_space);

NodeStructs::NameSpace getNamespaceStruct(const auto& indent_named_range, std::optional<NodeStructs::Typename> name_space, std::optional<std::string> name) {
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
					NodeStructs::Typename{ NodeStructs::NamespacedTypename{
						std::move(name_space).value(),
						name.value(),
					}, NodeStructs::Value{} } :
					NodeStructs::Typename{ NodeStructs::BaseTypename{ name.value() }, NodeStructs::Value{} }
			};
		else
			return std::move(name_space);
	}();

	for (const And<IndentToken, grammar::Named>& named : indent_named_range) {
		std::visit(
			overload(
				[&](const grammar::Type& e) {
					types.push_back(getStruct(e, copy(composed_ns)));
				},
				[&](const grammar::Function& e) {
					functions.push_back(structurize_function(e, copy(composed_ns)));
				},
				[&](const grammar::Interface& e) {
					interfaces.push_back(getStruct(e, copy(composed_ns)));
				},
				[&](const grammar::Template& e) {
					templates.push_back(getStruct(e, copy(composed_ns)));
				},
				[&](const grammar::Alias& e) {
					aliases.push_back(getStruct(e, copy(composed_ns)));
				},
				[&](const grammar::NameSpace& e) {
					namespaces.push_back(getStruct(e, copy(composed_ns)));
				},
				[&](const grammar::Enum& e) {
					enums.push_back(getStruct(e, copy(composed_ns)));
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

NodeStructs::NameSpace getStruct(const grammar::NameSpace& ns, std::optional<NodeStructs::Typename> name_space) {
	return getNamespaceStruct(
		ns.get<Indent<Star<Or<Token<NEWLINE>, And<IndentToken, grammar::Named>>>>>().get<And<IndentToken, grammar::Named>>(),
		std::move(name_space),
		ns.get<grammar::Word>().value
	);
}

NodeStructs::Exists getStruct(const grammar::Exists& e) {
	return NodeStructs::Exists{
		getNamespaceStruct(
			e.get<Indent<Star<Or<Token<NEWLINE>, And<IndentToken, grammar::Named>>>>>().get<And<IndentToken, grammar::Named>>(),
			std::nullopt,
			std::nullopt
		)
	};
}

NodeStructs::File getStruct(const grammar::File& f, std::string_view fileName) {
	using T = Star<Or<Token<NEWLINE>, grammar::Named, grammar::Exists>>;
	auto t = f.get<T>().get_view<grammar::Named>()
		| std::views::transform([&](auto&& e) { return e.value(); })
		;

	auto functions = t
		| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::Function>(e); })
		| std::views::transform([&](auto&& e) { return std::get<grammar::Function>(e); })
		| std::views::transform([&](auto&& fn) { return structurize_function(fn, std::nullopt); })
		| to_vec();

	return NodeStructs::File{
		.imports = f.get<Star<grammar::Import>>().get<grammar::Import>()
			| std::views::transform([&](auto&& e) { return getStruct(e); })
			| to_vec(),
		.exists = f.get<T>().get_view<grammar::Exists>()
			| std::views::transform([&](auto&& e) { return getStruct(e); })
			| to_vec(),
		.content = NodeStructs::NameSpace{
			.name = std::string{ fileName },
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
				| std::views::transform([&](auto&& e) { return getStruct(e, std::nullopt); })
				| to_vec(),
			.interfaces = t
				| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::Interface>(e); })
				| std::views::transform([&](auto&& e) { return std::get<grammar::Interface>(e); })
				| std::views::transform([&](auto&& e) { return getStruct(e, std::nullopt); })
				| to_vec(),
			.templates = t
				| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::Template>(e); })
				| std::views::transform([&](auto&& e) { return std::get<grammar::Template>(e); })
				| std::views::transform([&](auto&& e) { return getStruct(e, std::nullopt); })
				| to_vec(),
			.aliases = t
				| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::Alias>(e); })
				| std::views::transform([&](auto&& e) { return std::get<grammar::Alias>(e); })
				| std::views::transform([&](auto&& e) { return getStruct(e, std::nullopt); })
				| to_vec(),
			.enums = t
				| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::Enum>(e); })
				| std::views::transform([&](auto&& e) { return std::get<grammar::Enum>(e); })
				| std::views::transform([&](auto&& e) { return getStruct(e, std::nullopt); })
				| to_vec(),
			.namespaces = t
				| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::NameSpace>(e); })
				| std::views::transform([&](auto&& e) { return std::get<grammar::NameSpace>(e); })
				| std::views::transform([&](auto&& e) { return getStruct(e, std::nullopt); })
				| to_vec()
		}
	};
}

NodeStructs::ArgumentCategory getStruct(const Or<Token<MOVE>, And<Token<REF>, Token<NOT>>, Token<REF>>& t) {
	return std::visit(overload(overload_default_error,
		[](const Token<MOVE>&) -> NodeStructs::ArgumentCategory {
			return NodeStructs::Move{};
		},
		[](const And<Token<REF>, Token<NOT>>&) -> NodeStructs::ArgumentCategory {
			return NodeStructs::MutableReference{};
		},
		[](const Token<REF>&) -> NodeStructs::ArgumentCategory {
			return NodeStructs::Reference{};
		}),
		t.value()
	);
}

NodeStructs::FunctionArgument getStruct(const grammar::FunctionArgument& arg) {
	using call_t = Or<Token<MOVE>, And<Token<REF>, Token<NOT>>, Token<REF>>;
	if (arg.get<Opt<call_t>>().has_value())
		return NodeStructs::FunctionArgument{
			getStruct(arg.get<Opt<call_t>>().value()),
			getExpressionStruct(arg.get<grammar::Expression>())
	};
	else
		return NodeStructs::FunctionArgument{
			{},
			getExpressionStruct(arg.get<grammar::Expression>())
	};
}

NodeStructs::ParenArguments getStruct(const grammar::ParenArguments& args) {
	return {
		args.get<CommaStar<grammar::FunctionArgument>>().get_view<grammar::FunctionArgument>()
			| std::views::transform([&](auto&& e) { return getStruct(e); })
			| to_vec()
	};
}

NodeStructs::BracketArguments getStruct(const grammar::BracketArguments& args) {
	return {
		args.get<CommaStar<grammar::FunctionArgument>>().get_view<grammar::FunctionArgument>()
			| std::views::transform([&](auto&& e) { return getStruct(e); })
			| to_vec()
	};
}

NodeStructs::BraceArguments getStruct(const grammar::BraceArguments& args) {
	return {
		args.get<CommaStar<grammar::FunctionArgument>>().get_view<grammar::FunctionArgument>()
			| std::views::transform([&](auto&& e) { return getStruct(e); })
			| to_vec()
	};
}

NodeStructs::Expression getExpressionStruct(const grammar::BraceArguments&) {
	throw;
	/*NodeStructs::BraceArguments res;
	for (const auto& arg : statement.get<CommaStar<FunctionArgument>>().get<FunctionArgument>())
		res.args.push_back(getExpressionStruct(arg));
	return res;*/
}

NodeStructs::Expression getExpressionStruct(const grammar::ParenExpression& statement) {
	return std::visit(overload(overload_default_error,
		[](const grammar::Construct& e) -> NodeStructs::Expression {
			return make_expression({ NodeStructs::ConstructExpression{
				.operand = getStruct(e.get<grammar::Typename>(), tag_expect_empty_category{} ),
				.arguments = e.get<grammar::BraceArguments>().get<CommaStar<grammar::FunctionArgument>>().get<grammar::FunctionArgument>()
				| std::views::transform([&](auto&& e) { return getStruct(e); })
				| to_vec()
			} });
		},
		[](const grammar::ParenArguments& e) -> NodeStructs::Expression {
			return make_expression({ NodeStructs::ParenArguments{
				.args = e.get<CommaStar<grammar::FunctionArgument>>().get<grammar::FunctionArgument>()
				| std::views::transform([&](auto&& e) { return getStruct(e); })
				| to_vec()
			} });
		},
		[](const grammar::BracketArguments& /*e*/) -> NodeStructs::Expression {
			//const auto& args = e.get<CommaStar<FunctionArgument>>().get<FunctionArgument>();
			throw;
			//auto res = getExpressionStruct(e);
			//return make_expression({ std::move(res) });
		},
		[](const grammar::BraceArguments& e) -> NodeStructs::Expression {
			const auto& args = e.get<CommaStar<grammar::FunctionArgument>>().get<grammar::FunctionArgument>();
			return make_expression({ NodeStructs::BraceArguments{
				args
				| std::views::transform([&](auto&& e) { return getStruct(e); })
				| to_vec()
			} });
			//auto res = getExpressionStruct(e);
			//return make_expression({ std::move(res) });
		},
		[](const grammar::Word& e) {
			return make_expression(e.value);
		},
		[](const Token<INTEGER_NUMBER>& e) {
			return make_expression(e);
		},
		[](const Token<FLOATING_POINT_NUMBER>& e) {
			return make_expression(e);
		},
		[](const Token<STRING>& e) {
			return make_expression(e);
		}
	),
		statement.value()
	);
}

NodeStructs::Expression getPostfixExpressionStruct(NodeStructs::Expression&& expr, const grammar::Postfix& postfix) {
	return std::visit(
		overload(overload_default_error,
			[&](const And<Token<DOT>, grammar::Word, grammar::ParenArguments>& e) {
				return make_expression({ NodeStructs::PropertyAccessAndCallExpression{ std::move(expr), e.get<grammar::Word>().value, getStruct(e.get<grammar::ParenArguments>()) } });
			},
			[&](const And<Token<DOT>, grammar::Word>& e) {
				return make_expression({ NodeStructs::PropertyAccessExpression{ std::move(expr), e.get<grammar::Word>().value } });
			},
			[&](const And<Token<NS>, grammar::Word>& e) {
				return make_expression({ NodeStructs::NamespaceExpression{ std::move(expr), e.get<grammar::Word>().value } });
			},
			[&](const grammar::ParenArguments& args) {
				return make_expression({ NodeStructs::CallExpression{ std::move(expr), getStruct(args) } });
			},
			[&](const grammar::BracketArguments& args) {
				return make_expression({ NodeStructs::BracketAccessExpression{ std::move(expr), getStruct(args) } });
			},
			/*[&](const BraceArguments& args) {
				return make_expression({ NodeStructs::ConstructExpression{ std::move(expr), getStruct(args) } });
			},*/
			[&](const grammar::TemplateTypenameExtension& args) {
				return make_expression({ NodeStructs::TemplateExpression{
					std::move(expr), 
					args.get<CommaStar<grammar::TypenameOrExpression>>().get<grammar::TypenameOrExpression>()
					| std::views::transform([&](auto&& e) { return getStruct(e, tag_allow_value_category_or_empty{}); })
					| to_vec()
				} });
			}
		),
		postfix.value()
	);
}

NodeStructs::Expression getExpressionStruct(const grammar::PostfixExpression& statement, NodeStructs::Expression cur, const auto& vec, size_t i) {
	if (i == vec.size())
		return std::move(cur);
	return getExpressionStruct(statement, getPostfixExpressionStruct(std::move(cur), vec.at(i)), vec, i + 1);
}

NodeStructs::Expression getExpressionStruct(const grammar::PostfixExpression& statement) {
	const auto& postfixes = statement.get<Star<grammar::Postfix>>().get<grammar::Postfix>();
	auto expr = getExpressionStruct(statement.get<grammar::ParenExpression>());
	return getExpressionStruct(statement, std::move(expr), postfixes, 0);
}

NodeStructs::Expression getExpressionStruct(const grammar::UnaryExpression& statement) {
	const auto& prefixes = statement.get().get<Star<grammar::unary_operators>>().get<grammar::unary_operators>();
	if (prefixes.size() == 0)
		return getExpressionStruct(statement.get().get<grammar::PostfixExpression>());
	else {
		return make_expression({ NodeStructs::UnaryExpression {
			.unary_operators = prefixes | std::views::transform(
				[](const auto& e) {
					return std::visit(
						[](const auto& token) {
							return NodeStructs::UnaryExpression::op_types{ token };
						},
						e.value()
					);
				})
			| to_vec(),
			.expr = getExpressionStruct(statement.get().get<grammar::PostfixExpression>())
		} });
	}
}

NodeStructs::Expression getExpressionStruct(const grammar::MultiplicativeExpression& statement) {
	using operators = Or<Token<ASTERISK>, Token<SLASH>, Token<PERCENT>>;
	using VT = Variant<Token<ASTERISK>, Token<SLASH>, Token<PERCENT>>;
	const auto& multiplications = statement.get<Star<And<operators, grammar::UnaryExpression>>>().get<And<operators, grammar::UnaryExpression>>();
	if (multiplications.size() == 0)
		return getExpressionStruct(statement.get<grammar::UnaryExpression>());
	else
		return make_expression({ NodeStructs::MultiplicativeExpression{
			getExpressionStruct(statement.get<grammar::UnaryExpression>()),
			multiplications
				| std::views::transform([&](auto&& op_exp) {
					return std::pair{
						VT{ op_exp.get<operators>().value() },
						getExpressionStruct(op_exp.get<grammar::UnaryExpression>())
					};
				})
				| to_vec()
		} });
}

NodeStructs::Expression getExpressionStruct(const grammar::AdditiveExpression& statement) {
	using operators = Or<Token<PLUS>, Token<DASH>>;
	using VT = Variant<Token<PLUS>, Token<DASH>>;
	const auto& additions = statement.get<Star<And<operators, grammar::MultiplicativeExpression>>>().get<And<operators, grammar::MultiplicativeExpression>>();
	if (additions.size() == 0)
		return getExpressionStruct(statement.get<grammar::MultiplicativeExpression>());
	else
		return make_expression({ NodeStructs::AdditiveExpression{
			getExpressionStruct(statement.get<grammar::MultiplicativeExpression>()),
			additions
				| std::views::transform([&](auto&& op_exp) {
					return std::pair{
						VT{ op_exp.get<operators>().value() },
						getExpressionStruct(op_exp.get<grammar::MultiplicativeExpression>())
					};
				})
				| to_vec()
		} });
}

NodeStructs::Expression getExpressionStruct(const grammar::CompareExpression& statement) {
	using op_add = And<grammar::CompareOperator, grammar::AdditiveExpression>;
	using VT = Variant<Token<LT>, Token<LTE>, Token<GT>, Token<GTE>>;
	const auto& comparisons = statement.get<Star<op_add>>().get<op_add>();
	if (comparisons.size() == 0)
		return getExpressionStruct(statement.get<grammar::AdditiveExpression>());
	else
		return make_expression({ NodeStructs::CompareExpression{
			getExpressionStruct(statement.get<grammar::AdditiveExpression>()),
			comparisons
				| std::views::transform([&](const op_add& op_exp) {
					return std::pair{
						VT{ op_exp.get<grammar::CompareOperator>().get<Or<Token<LT>, Token<LTE>, Token<GT>, Token<GTE>>>().value() },
						getExpressionStruct(op_exp.get<grammar::AdditiveExpression>())
					};
				})
				| to_vec()
		} });
}

NodeStructs::Expression getExpressionStruct(const grammar::EqualityExpression& statement) {
	using operators = Or<Token<EQUALEQUAL>, Token<NEQUAL>>;
	using VT = Variant<Token<EQUALEQUAL>, Token<NEQUAL>>;
	const auto& equals = statement.get<Star<And<operators, grammar::CompareExpression>>>().get<And<operators, grammar::CompareExpression>>();
	if (equals.size() == 0)
		return getExpressionStruct(statement.get<grammar::CompareExpression>());
	else
		return make_expression({ NodeStructs::EqualityExpression{
			getExpressionStruct(statement.get<grammar::CompareExpression>()),
			equals
				| std::views::transform([&](auto&& op_exp) {
					return std::pair{
						VT{ op_exp.get<operators>().value() },
						getExpressionStruct(op_exp.get<grammar::CompareExpression>())
					};
				})
				| to_vec()
		} });
}

NodeStructs::Expression getExpressionStruct(const grammar::AndExpression& statement) {
	const auto& ands = statement.get<Star<And<Token<AND>, grammar::EqualityExpression>>>().get<grammar::EqualityExpression>();
	if (ands.size() == 0)
		return getExpressionStruct(statement.get<grammar::EqualityExpression>());
	else
		return make_expression({ NodeStructs::AndExpression{
			getExpressionStruct(statement.get<grammar::EqualityExpression>()),
			ands
				| std::views::transform([&](auto& e) { return getExpressionStruct(e); })
				| to_vec()
		} });
}

NodeStructs::Expression getExpressionStruct(const grammar::OrExpression& statement) {
	const auto& ors = statement.get<Star<And<Token<OR>, grammar::AndExpression>>>().get<grammar::AndExpression>();
	if (ors.size() == 0)
		return getExpressionStruct(statement.get<grammar::AndExpression>());
	else
		return make_expression({ NodeStructs::OrExpression{
			getExpressionStruct(statement.get<grammar::AndExpression>()),
			ors
				| std::views::transform([&](auto& e) { return getExpressionStruct(e); })
				| to_vec()
		} });
}

NodeStructs::Expression getExpressionStruct(const grammar::ConditionalExpression& statement) {
	const auto& ifElseExpr = statement.get<Opt<And<
		Token<IF>,
		grammar::OrExpression,
		Token<ELSE>,
		grammar::OrExpression
		>>>();
	if (ifElseExpr.has_value())
		return make_expression({ NodeStructs::ConditionalExpression{
			getExpressionStruct(statement.get<grammar::OrExpression>()),
			std::pair{
				getExpressionStruct(ifElseExpr.value().get<grammar::OrExpression, 0>()),
				getExpressionStruct(ifElseExpr.value().get<grammar::OrExpression, 1>())
			}
		} });
	else
		return getExpressionStruct(statement.get<grammar::OrExpression>());
}

NodeStructs::Expression getExpressionStruct(const grammar::ExpressionStatement& statement) {
	return getExpressionStruct(statement.get<grammar::Expression>());
}

NodeStructs::Expression getStatementStruct(const grammar::ExpressionStatement& statement) {
	return getExpressionStruct(statement);
}

auto getStatementStruct(const Expect<grammar::Statement>& statement) {
	const grammar::Statement& st = statement;
	return getStatementStruct(st);
}

NodeStructs::VariableDeclarationStatement getStatementStruct(const grammar::VariableDeclarationStatement& statement) {
	return {
		getStruct(statement.get<grammar::Typename>(), tag_allow_value_category_or_empty{}),
		statement.get<grammar::Word>().value, getExpressionStruct(statement.get<grammar::Expression>())
	};
}

std::vector<NodeStructs::Statement> getStatements(const grammar::ColonIndentCodeBlock& code) {
	return code.get<Indent<Star<Or<Token<NEWLINE>, Expect<grammar::Statement>>>>>().get<Or<Token<NEWLINE>, Expect<grammar::Statement>>>()
		| std::views::transform(
			[&](const Or<Token<NEWLINE>, Expect<grammar::Statement>>& e)
			-> const std::variant<Token<NEWLINE>, Expect<grammar::Statement>>& {
				return e.value();
			})
		| std::views::filter([&](auto&& e) { return std::holds_alternative<Expect<grammar::Statement>>(e); })
		| std::views::transform([&](auto&& e) { return std::get<Expect<grammar::Statement>>(e); })
		| std::views::transform([&](const Expect<grammar::Statement>& e) { return getStatementStruct(e); })
		| to_vec();
}

NodeStructs::BlockStatement getStatementStruct(const grammar::BlockStatement& statement) {
	// wtf even is this right now it doesn't make sense
	throw;
	//return { getStruct(statement.get<grammar::Typename>()) };
}

NodeStructs::IfStatement getStatementStruct(const grammar::IfStatement& statement) {
	using T = Variant<NonCopyableBox<NodeStructs::IfStatement>, std::vector<NodeStructs::Statement>>;
	return {
		getExpressionStruct(statement.get<grammar::Expression>()),
		getStatements(statement.get<grammar::ColonIndentCodeBlock>()),
		statement.get<Opt<Alloc<grammar::ElseStatement>>>().node.transform([](const auto& e) -> T {
			return std::visit(
				overload(overload_default_error,
					[&](const Alloc<grammar::IfStatement>& e) -> T {
						return getStatementStruct(e.get());
					},
					[&](const grammar::ColonIndentCodeBlock& e) -> T {
						return getStatements(e);
					}
				),
				e.get()
				.get<Or<Alloc<grammar::IfStatement>, grammar::ColonIndentCodeBlock>>()
				.value()
			);
		})
	};
}

NodeStructs::ForStatement getForStatementStruct(const auto& statement) {
	return {
		.collection = getExpressionStruct(statement.get<grammar::Expression>()),
		.iterators = statement.get<CommaPlus<Or<grammar::VariableDeclaration, grammar::Word>>>().get<Or<grammar::VariableDeclaration, grammar::Word>>()
			| std::views::transform([](const Or<grammar::VariableDeclaration, grammar::Word>& or_node) {
				return std::visit(overload(
					[](const grammar::Word& e) -> Variant<NodeStructs::VariableDeclaration, std::string> {
						return { e.value };
					},
					[](const grammar::VariableDeclaration& e) -> Variant<NodeStructs::VariableDeclaration, std::string> {
						return NodeStructs::VariableDeclaration{
							getStruct(e.get<grammar::Typename>(), tag_allow_value_category_or_empty{}),
							e.get<grammar::Word>().value
						};
					}
				), or_node.value());
			})
			| to_vec(),
		.statements = getStatements(statement.get<grammar::ColonIndentCodeBlock>())
	};
}

NodeStructs::ForStatement getStatementStruct(const grammar::ForStatement& statement) {
	return getForStatementStruct(statement);
}

NodeStructs::IForStatement getStatementStruct(const grammar::IForStatement& statement) {
	return {
		statement.get<grammar::Word>().value,
		getForStatementStruct(statement)
	};
}

NodeStructs::WhileStatement getStatementStruct(const grammar::WhileStatement& statement) {
	return {
		getExpressionStruct(statement.get<grammar::Expression>()),
		getStatements(statement.get<grammar::ColonIndentCodeBlock>())
	};
}

NodeStructs::MatchCase getCase(const auto& typenames, const auto& statements) {
	return {
		typenames
		| std::views::transform([&](auto&& variable_declaration) {
			return std::pair{
				getStruct(variable_declaration.get<grammar::Typename>(), tag_allow_value_category_or_empty{}),
				variable_declaration.get<grammar::Word>().value
			}; })
		| to_vec(),
		statements
		| std::views::transform([&](auto&& e) { return e.value(); })
		| std::views::filter([&](auto&& e) { return std::holds_alternative<Expect<grammar::Statement>>(e); })
		| std::views::transform([&](auto&& e) { return std::get<Expect<grammar::Statement>>(e); })
		| std::views::transform([&](auto&& e) { return getStatementStruct(e); })
		| to_vec()
	};
}

std::vector<NodeStructs::MatchCase> getCases(const Indent<Plus<And<IndentToken, CommaPlus<grammar::VariableDeclaration>, grammar::ColonIndentCodeBlock>>>& cases) {
	std::vector<NodeStructs::MatchCase> res;
	res.reserve(cases.nodes.size());
	for (const auto& and_node : cases.nodes) {
		const auto& [_, typenames, statements] = and_node.value;
		res.push_back(getCase(
			typenames.nodes,
			statements.get<Indent<Star<Or<Token<NEWLINE>, Expect<grammar::Statement>>>>>().get_view<Or<Token<NEWLINE>, Expect<grammar::Statement>>>()
		));
	}
	return res;
}

NodeStructs::MatchStatement getStatementStruct(const grammar::MatchStatement& statement) {
	return {
		statement.get<CommaPlus<grammar::Expression>>().get_view<grammar::Expression>()
			| std::views::transform([&](auto&& e) { return getExpressionStruct(e); })
			| to_vec(),
		getCases(statement.get<Indent<Plus<And<
			IndentToken,
			CommaPlus<grammar::VariableDeclaration>,
			grammar::ColonIndentCodeBlock
		>>>>())
	};
}

NodeStructs::BreakStatement getStatementStruct(const grammar::BreakStatement& statement) {
	return {
		statement.get<Opt<And<Token<IF>, grammar::Expression>>>().has_value()
			? getExpressionStruct(statement.get<Opt<And<Token<IF>, grammar::Expression>>>().value().get<grammar::Expression>())
			: std::optional<NodeStructs::Expression>{}
	};
}

std::vector<NodeStructs::Expression> getExpressions(const std::vector<grammar::Expression>& vec) {
	return vec
		| std::views::transform([&](auto&& e) { return getExpressionStruct(e); })
		| to_vec();
}

NodeStructs::ReturnStatement getStatementStruct(const grammar::ReturnStatement& statement) {
	std::vector<NodeStructs::FunctionArgument> returns = statement.get<CommaStar<grammar::FunctionArgument>>().get<grammar::FunctionArgument>()
		| std::views::transform([](auto&& e) { return getStruct(e); })
		| to_vec();
	return {
		std::move(returns),
		statement.get<Opt<And<Token<IF>, grammar::Expression>>>().has_value()
			? getExpressionStruct(statement.get<Opt<And<Token<IF>, grammar::Expression>>>().value().get<grammar::Expression>())
			: std::optional<NodeStructs::Expression>{}
	};
}

NodeStructs::SwitchStatement getStatementStruct(const grammar::SwitchStatement& statement) {
	return NodeStructs::SwitchStatement{
		.expr = getExpressionStruct(statement.get<grammar::Expression>()),
		.cases = statement.get<Indent<Star<And<
			IndentToken,
			grammar::Expression,
			grammar::ColonIndentCodeBlock
		>>>>().get<And<IndentToken, grammar::Expression, grammar::ColonIndentCodeBlock>>()
		| std::views::transform([](const And<IndentToken, grammar::Expression, grammar::ColonIndentCodeBlock>& switch_case) {
			return NodeStructs::SwitchCase{
				getExpressionStruct(switch_case.get<grammar::Expression>()),
				getStatements(switch_case.get<grammar::ColonIndentCodeBlock>())
			};
		})
		| to_vec()
	};
}

NodeStructs::Assignment getStatementStruct(const grammar::Assignment& statement) {
	return { getExpressionStruct(statement.get<grammar::Expression, 0>()), getExpressionStruct(statement.get<grammar::Expression, 1>()) };
}

NodeStructs::Statement getStatementStruct(const grammar::Statement& statement) {
	return NodeStructs::Statement{
		std::visit(
			[](const auto& statement) -> NodeStructs::Statement::VT {
				return { getStatementStruct(statement) };
			},
			statement.get<grammar::StatementOpts>().get().value()
		),
		statement.get<Opt<Token<POUND>>>().has_value()
	};
}

NodeStructs::WordTypenameOrExpression getStruct(const grammar::TypenameOrExpression& t, tag_expect_value_category) {
	return getTypenameOrExpressionStruct<tag_expect_value_category>(t);
}

NodeStructs::WordTypenameOrExpression getStruct(const grammar::TypenameOrExpression& t, tag_expect_empty_category) {
	return getTypenameOrExpressionStruct<tag_expect_empty_category>(t);
}

NodeStructs::WordTypenameOrExpression getStruct(const grammar::TypenameOrExpression& t, tag_allow_value_category_or_empty) {
	return getTypenameOrExpressionStruct<tag_allow_value_category_or_empty>(t);
}
