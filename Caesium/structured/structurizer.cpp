#include "structurizer.hpp"
#include <iostream>

NodeStructs::Import getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Import& f
) {
	return { std::visit(
		overload(overload_default_error,
			[&](const grammar::Word& word) {
				return word.value + ".caesium";
			},
			[&](const grammar::String& string) {
				return string.value;
		}),
		f.template get<Or<grammar::Word, grammar::String>>().value()
	) };
}

template <typename tag_t>
NodeStructs::WordTypenameOrExpression getTypenameOrExpressionStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::TypenameOrExpression& tn_or_expr
) {
	return std::visit(overload(
		[&](const grammar::Typename& tn) -> NodeStructs::WordTypenameOrExpression {
			bool has_to_be_interpreted_as_typename_because_of_category_or_optional =
				tn.template get<Opt<grammar::ParameterCategory>>().has_value() || tn.template get<Opt<Token<QUESTION>>>().has_value();
			if (has_to_be_interpreted_as_typename_because_of_category_or_optional)
				return { getStruct(file_name, vec, tn, tag_t{}) };
			else
				return std::visit(overload(
					[&](const Token<AUTO>&) -> NodeStructs::WordTypenameOrExpression { // auto is a typename
						return { getStruct(file_name, vec, tn, tag_t{}) };
					},
					[&](const grammar::NonAutoTypename& non_auto_typename) -> NodeStructs::WordTypenameOrExpression {
						bool has_to_be_interpreted_as_typename_because_of_extensions =
							non_auto_typename.template get<Star<Or<
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
						), non_auto_typename.template get<Or<grammar::VariadicExpansionTypename, grammar::Word>>().value());
					}
			), tn.template get<Or<Token<AUTO>, grammar::NonAutoTypename>>().value());
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
		return res;
	const auto& ext = exts.at(i);
	return std::visit(overload(
		[&](const NodeStructs::VariadicExpansionTypename&) {
			return getStruct(file_name, vec, t, exts, std::visit(overload(overload_default_error,
				[&](const grammar::NamespaceTypenameExtension& e) -> NodeStructs::Typename {
					NOT_IMPLEMENTED;
				},
				[&](const grammar::TemplateTypenameExtension& e) -> NodeStructs::Typename {
					return make_typename(
						NodeStructs::VariadicExpansionTypename{
							make_typename(
								NodeStructs::TemplatedTypename{
									get<NodeStructs::VariadicExpansionTypename>(std::move(res)).type,
									e.template get<CommaStar<grammar::TypenameOrExpression>>().template get<grammar::TypenameOrExpression>()
									| std::views::transform([&](const grammar::TypenameOrExpression& e) { return getStruct(file_name, vec, e, tag_expect_empty_category{}); })
									| to_vec()
								}, NodeStructs::Value{}, rule_info_from_rule(file_name, vec, e)
							)
						}, NodeStructs::Value{}, rule_info_from_rule(file_name, vec, e)
					);
				},
				[&](const grammar::UnionTypenameExtension& ext) -> NodeStructs::Typename {
					// variadics dont support unions
					NOT_IMPLEMENTED;
				},
				[&](const Token<QUESTION>& ext) -> NodeStructs::Typename {
					NOT_IMPLEMENTED;
				}
			), ext.value()), i + 1);
		},
		[&](const auto&) { // just checking its not a variadic expansion
			return getStruct(file_name, vec, t, exts, std::visit(overload(overload_default_error,
				[&](const grammar::NamespaceTypenameExtension& e) -> NodeStructs::Typename {
					return make_typename(NodeStructs::NamespacedTypename{ std::move(res), e.template get<grammar::Word>().value }, NodeStructs::Value{}, rule_info_from_rule(file_name, vec, e));
				},
				[&](const grammar::TemplateTypenameExtension& e) -> NodeStructs::Typename {
					return make_typename(NodeStructs::TemplatedTypename{
						std::move(res),
						e.template get<CommaStar<grammar::TypenameOrExpression>>().template get<grammar::TypenameOrExpression>()
						| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e, tag_allow_value_category_or_empty{}); })
						| to_vec()
						}, NodeStructs::Value{}, rule_info_from_rule(file_name, vec, e));
				},
				[&](const grammar::UnionTypenameExtension& ext) -> NodeStructs::Typename {
					auto temp = getStruct(file_name, vec, ext.template get<Alloc<grammar::Typename>>().get(), tag_allow_value_category_or_empty{});
					if (holds<NodeStructs::UnionTypename>(res)) {
						if (holds<NodeStructs::UnionTypename>(temp))
							NOT_IMPLEMENTED;
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
NodeStructs::Typename get_typename_struct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Typename& t
) {
	const auto& auto_or_tn = t.template get<Or<Token<AUTO>, grammar::NonAutoTypename>>();
	NodeStructs::ParameterCategory value_cat = getStruct(file_name, vec, t.template get<Opt<grammar::ParameterCategory>>(), tag{});
	// const auto& optional = t.template get<Opt<Token<QUESTION>>>();

	return std::visit(overload(
		[&](const Token<AUTO>& a) -> NodeStructs::Typename {
			return make_typename(copy(auto_tn_base), std::move(value_cat), rule_info_from_rule(file_name, vec, a));
		},
		[&](const grammar::NonAutoTypename& e) -> NodeStructs::Typename {
			NodeStructs::Typename res = std::visit(overload(
				[&](const grammar::Word& w) -> NodeStructs::Typename {
					return make_typename(NodeStructs::BaseTypename{ w.value }, NodeStructs::Value{}, rule_info_from_rule(file_name, vec, w));
				},
				[&](const grammar::VariadicExpansionTypename& vetn) -> NodeStructs::Typename {
					return make_typename(
						NodeStructs::VariadicExpansionTypename{
							make_typename(NodeStructs::BaseTypename{ vetn.template get<grammar::Word>().value }, NodeStructs::Value{}, rule_info_from_rule(file_name, vec, vetn.template get<grammar::Word>()))
						},
						NodeStructs::Value{}, rule_info_from_rule(file_name, vec, vetn)
					);
				}
			), e.template get<Or<grammar::VariadicExpansionTypename, grammar::Word>>().value());
			using opts = Or<grammar::NamespaceTypenameExtension, grammar::TemplateTypenameExtension, grammar::UnionTypenameExtension, Token<QUESTION>>;
			NodeStructs::Typename::vt res2 = getStruct(file_name, vec, t, e.template get<Star<opts>>().template get<opts>(), std::move(res), 0).value;
			return make_typename(std::move(res2), std::move(value_cat), rule_info_from_rule(file_name, vec, e));
		}
	), auto_or_tn.value());
}

NodeStructs::Typename getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Typename& t, tag_expect_value_category
) {
	return get_typename_struct<tag_expect_value_category>(file_name, vec, t);
}

NodeStructs::Typename getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Typename& t, tag_expect_empty_category
) {
	return get_typename_struct<tag_expect_empty_category>(file_name, vec, t);
}

NodeStructs::Typename getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Typename& t, tag_allow_value_category_or_empty
) {
	return get_typename_struct<tag_allow_value_category_or_empty>(file_name, vec, t);
}

NodeStructs::ParameterCategory getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::ParameterCategory& vc
) {
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

NodeStructs::ParameterCategory getStruct(const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const Opt<grammar::ParameterCategory>& vc, tag_expect_value_category
) {
	if (vc.has_value())
		return getStruct(file_name, vec, vc.value());
	else
		NOT_IMPLEMENTED;
}

NodeStructs::ParameterCategory getStruct(const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const Opt<grammar::ParameterCategory>& vc, tag_expect_empty_category
) {
	if (vc.has_value())
		NOT_IMPLEMENTED;
	return NodeStructs::Value{};
}

NodeStructs::ParameterCategory getStruct(const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const Opt<grammar::ParameterCategory>& vc, tag_allow_value_category_or_empty
) {
	if (vc.has_value())
		return getStruct(file_name, vec, vc.value());
	else
		return NodeStructs::Value{};
}

NodeStructs::Function getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Function& f, std::optional<NodeStructs::Typename> name_space
) {
	return NodeStructs::Function{
		.name = f.template get<grammar::Word>().value,
		.name_space = std::move(name_space),
		.returnType = getStruct(file_name, vec, f.template get<grammar::Typename>(), tag_allow_value_category_or_empty{}),
		.parameters = f.template get<grammar::FunctionParameters>().template get<And<Commit<grammar::Typename>, grammar::Word>>()
			| std::views::transform([&](const grammar::FunctionParameter& type_and_name) -> NodeStructs::FunctionParameter {
				auto res = NodeStructs::FunctionParameter{
					getStruct(file_name, vec, type_and_name.template get<Commit<grammar::Typename>>(), tag_expect_value_category{}),
					type_and_name.template get<grammar::Word>().value
				};
				if (!res.typename_.category._value.has_value())
					NOT_IMPLEMENTED;
				return res;
			})
			| to_vec(),
		.statements = getStatements(file_name, vec, f.template get<grammar::ColonIndentCodeBlock<function_context>>()),
		.info = rule_info_from_rule(file_name, vec, f)
	};
}

Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter> getTemplateParameter(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const And<grammar::Word, Token<DOTS>>& parameter
) {
	return NodeStructs::VariadicTemplateParameter{ parameter.template get<grammar::Word>().value };
}

Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter> getTemplateParameter(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const And<grammar::Word, Opt<And<Token<EQUAL>, grammar::TypenameOrExpression>>>& parameter_and_optional_value
) {
	if (parameter_and_optional_value.template get<Opt<And<Token<EQUAL>, grammar::TypenameOrExpression>>>().has_value())
		return NodeStructs::TemplateParameterWithDefaultValue{
			.name = parameter_and_optional_value.template get<grammar::Word>().value,
			.value = getStruct(
				file_name,
				vec,
				parameter_and_optional_value.template get<Opt<And<Token<EQUAL>, grammar::TypenameOrExpression>>>().value().template get<grammar::TypenameOrExpression>(),
				tag_expect_empty_category{}
			)
		};
	else
		return NodeStructs::TemplateParameter{
			parameter_and_optional_value.template get<grammar::Word>().value,
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
			grammar::TypenameOrExpression
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

NodeStructs::Template getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Template& t
) {
	return getStruct(file_name, vec, t, std::nullopt);
}

NodeStructs::Template getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Template& t, std::optional<NodeStructs::Typename> name_space
) {
	using parameters_t = CommaStar<parameter_t>;
	return {
		.name = t.template get<grammar::Word>().value,
		.name_space = std::move(name_space),
		.parameters = t.template get<parameters_t>().template get<parameter_t>()
			| std::views::transform([&](auto&& e) { return getTemplateParameter(file_name, vec, e); })
			| to_vec(),
		.templated = t.template get<TemplateBody>().value,
		.indent = t.n_indent + 1,
		.info = rule_info_from_rule(file_name, vec, t)
	};
}

NodeStructs::MemberVariable getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::MemberVariable& f
) {
	return NodeStructs::MemberVariable{
		.type = getStruct(file_name, vec, f.template get<grammar::Typename>(), tag_allow_value_category_or_empty{}),
		.name = f.template get<grammar::Word>().value,
	};
}

NodeStructs::Alias getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Alias& f, std::optional<NodeStructs::Typename> name_space
) {
	return NodeStructs::Alias{
		.name = f.template get<grammar::Word>().value,
		.aliasTo = getStruct(file_name, vec, f.template get<grammar::Typename>(), tag_allow_value_category_or_empty{}),
		.name_space = std::move(name_space)
	};
}

template <template <typename> typename Statement, typename context>
NodeStructs::CompileTimeStatement<context> get_compile_time_statement_from_specific(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const Statement<context>& grammar_statement
);

template <typename context>
NodeStructs::CompileTimeStatement<context> get_compile_time_statement(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::CompileTimeStatement<context>& statement
) {
	return std::visit([&](const auto& e) -> NodeStructs::CompileTimeStatement<context> {
		return get_compile_time_statement_from_specific(file_name, vec, e);
	}, statement.template get<Or<
		grammar::VariableDeclarationStatement<context>,
		grammar::IfStatement<context>,
		grammar::ForStatement<context>,
		grammar::IForStatement<context>,
		grammar::WhileStatement<context>,
		grammar::BreakStatement<context>,
		grammar::BlockStatement<context>,
		grammar::MatchStatement<context>,
		grammar::SwitchStatement<context>,
		grammar::Assignment<context>
	>>().value());
}

NodeStructs::Type getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Type& t,
	std::optional<NodeStructs::Typename> name_space
) {
	auto elems = t.template get<Indent<Star<grammar::Statement<type_context>>>>().template get_view<grammar::Statement<type_context>>();
	return {
		.name = t.template get<grammar::Word>().value,
		.name_space = copy(name_space),
		.members = elems
			| std::views::transform([&](auto&& e) { return get_base_statement_struct(file_name, vec, e); })
			| to_vec(),
		.info = rule_info_from_rule(file_name, vec, t.template get<grammar::Word>())
	};
}

NodeStructs::Interface getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Interface& t,
	std::optional<NodeStructs::Typename> name_space
) {
	auto elems = t.template get<Indent<Star<grammar::Statement<type_context>>>>().template get_view<grammar::Statement<type_context>>();
	return {
		.name = t.template get<grammar::Word>().value,
		.name_space = copy(name_space),
		.members = elems
			| std::views::transform([&](auto&& e) { return get_base_statement_struct(file_name, vec, e); })
			| to_vec(),
		.info = rule_info_from_rule(file_name, vec, t.template get<grammar::Word>())
	};
}

NodeStructs::Enum getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Enum& e, std::optional<NodeStructs::Typename> name_space
) {
	using value_t = Indent<And<IndentToken, grammar::Word, grammar::Newline>>;
	std::vector<std::string> values = {};
	auto enum_values = e.template get<Star<value_t>>().template get<value_t>();
	values.reserve(enum_values.size());
	for (const value_t& v : enum_values)
		values.push_back(v.template get<grammar::Word>().value);
	return {
		e.template get<grammar::Word>().value,
		std::move(values),
		copy(name_space)
	};
}

bool uses_auto(const NodeStructs::Function& fn);

NodeStructs::NameSpace getNamespaceStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const auto& indent_named_range,
	caesium_source_location info,
	std::optional<NodeStructs::Typename> name_space,
	const grammar::Word& name
) {
	std::vector<NodeStructs::Type> types;
	std::vector<NodeStructs::Function> functions;
	std::vector<NodeStructs::Interface> interfaces;
	std::vector<NodeStructs::Template> templates;
	std::vector<NodeStructs::Alias> aliases;
	std::vector<NodeStructs::NameSpace> namespaces;
	std::vector<NodeStructs::Enum> enums;

	NodeStructs::Typename composed_ns = [&]() {
		if (name_space.has_value())
			return make_typename(NodeStructs::NamespacedTypename{
				copy(name_space.value()),
				name.value,
				}, NodeStructs::Value{}, rule_info_from_rule(file_name, vec, name));
		else
			return make_typename(NodeStructs::BaseTypename{ name.value }, NodeStructs::Value{}, rule_info_from_rule(file_name, vec, name));
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
			), named.template get<grammar::Named>().value()
		);
	}

	return {
		.name = name.value,
		.name_space = copy(name_space),
		.functions = functions
			| std::views::transform([&](auto&& fn) { return copy(fn); })
			| to_vec(),
		.types = std::move(types),
		.interfaces = std::move(interfaces),
		.templates = std::move(templates),
		.blocks = {},
		.aliases = std::move(aliases),
		.enums = std::move(enums),
		.namespaces = std::move(namespaces),
		.info = std::move(info)
	};
}

NodeStructs::NameSpace getNamespaceStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const auto& indent_named_range,
	caesium_source_location info
) {
	std::vector<NodeStructs::Type> types;
	std::vector<NodeStructs::Function> functions;
	std::vector<NodeStructs::Interface> interfaces;
	std::vector<NodeStructs::Template> templates;
	std::vector<NodeStructs::Alias> aliases;
	std::vector<NodeStructs::NameSpace> namespaces;
	std::vector<NodeStructs::Enum> enums;

	for (const And<IndentToken, grammar::Named>& named : indent_named_range) {
		std::visit(
			overload(
				[&](const grammar::Type& e) {
					types.push_back(getStruct(file_name, vec, e, std::nullopt));
				},
				[&](const grammar::Function& e) {
					functions.push_back(getStruct(file_name, vec, e, std::nullopt));
				},
				[&](const grammar::Interface& e) {
					interfaces.push_back(getStruct(file_name, vec, e, std::nullopt));
				},
				[&](const grammar::Template& e) {
					templates.push_back(getStruct(file_name, vec, e, std::nullopt));
				},
				[&](const grammar::Alias& e) {
					aliases.push_back(getStruct(file_name, vec, e, std::nullopt));
				},
				[&](const grammar::NameSpace& e) {
					namespaces.push_back(getStruct(file_name, vec, e, std::nullopt));
				},
				[&](const grammar::Enum& e) {
					enums.push_back(getStruct(file_name, vec, e, std::nullopt));
				}
			), named.template get<grammar::Named>().value()
		);
	}

	return {
		.name = "UNNAMED_NAMESPACE",
		.name_space = std::nullopt,
		.functions = functions
		//| std::views::filter([&](auto&& f) { return !uses_auto(f); })
		| std::views::transform([&](auto&& fn) { return copy(fn); })
		| to_vec(),
		/*.functions_using_auto = functions
		| std::views::filter([&](auto&& f) { return uses_auto(f); })
		| std::views::transform([&](auto&& fn) { return copy(fn); })
		| to_vec(),*/
		.types = std::move(types),
		.interfaces = std::move(interfaces),
		.templates = std::move(templates),
		.blocks = {},
		.aliases = std::move(aliases),
		.enums = std::move(enums),
		.namespaces = std::move(namespaces),
		.info = std::move(info)
	};
}

NodeStructs::NameSpace getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::NameSpace& ns,
	std::optional<NodeStructs::Typename> name_space
) {
	return getNamespaceStruct(
		file_name,
		vec,
		ns.template get<Indent<Star<Or<Token<NEWLINE>, And<IndentToken, grammar::Named>>>>>().template get<And<IndentToken, grammar::Named>>(),
		rule_info_from_rule(file_name, vec, ns),
		std::move(name_space),
		ns.template get<grammar::Word>()
	);
}

NodeStructs::Exists getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Exists& e
) {
	return NodeStructs::Exists{ getNamespaceStruct(
		file_name,
		vec,
		e.template get<Indent<Star<Or<Token<NEWLINE>, And<IndentToken, grammar::Named>>>>>().template get<And<IndentToken, grammar::Named>>(),
		rule_info_from_rule(file_name, vec, e)
	) };
}

NodeStructs::File getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::File& f
) {
	using T = Star<Or<Token<NEWLINE>, grammar::Named, grammar::Exists>>;
	auto t = f.template get<T>().template get_view<grammar::Named>()
		| std::views::transform([&](auto&& e) { return e.value(); })
		;

	auto functions = t
		| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::Function>(e); })
		| std::views::transform([&](auto&& e) { return std::get<grammar::Function>(e); })
		| std::views::transform([&](auto&& fn) { return getStruct(file_name, vec, fn, std::nullopt); })
		| to_vec();

	return NodeStructs::File{
		.imports = f.template get<Star<grammar::Import>>().template get<grammar::Import>()
			| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e); })
			| to_vec(),
		.exists = f.template get<T>().template get_view<grammar::Exists>()
			| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e); })
			| to_vec(),
		.content = NodeStructs::NameSpace{
			.name = std::string{ file_name },
			.functions = functions
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
				| to_vec(),
			.info = rule_info_from_rule(file_name, vec, f)
		}
	};
}

NodeStructs::ArgumentCategory getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const Or<Token<MOVE>, And<Token<REF>, Token<NOT>>, Token<REF>>& t
) {
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

NodeStructs::FunctionArgument getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::FunctionArgument& arg
) {
	using call_t = Or<Token<MOVE>, And<Token<REF>, Token<NOT>>, Token<REF>>;
	if (arg.template get<Opt<call_t>>().has_value())
		return NodeStructs::FunctionArgument{
			getStruct(file_name, vec, arg.template get<Opt<call_t>>().value()),
			getExpressionStruct(file_name, vec, arg.template get<grammar::Expression>())
	};
	else
		return NodeStructs::FunctionArgument{
			{},
			getExpressionStruct(file_name, vec, arg.template get<grammar::Expression>())
	};
}

NodeStructs::ParenArguments getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::ParenArguments& args
) {
	return {
		args.template get<CommaStar<grammar::FunctionArgument>>().template get_view<grammar::FunctionArgument>()
			| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e); })
			| to_vec()
	};
}

NodeStructs::BracketArguments getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::BracketArguments& args
) {
	return {
		args.template get<CommaStar<grammar::FunctionArgument>>().template get_view<grammar::FunctionArgument>()
			| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e); })
			| to_vec()
	};
}

NodeStructs::BraceArguments getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::BraceArguments& args
) {
	return {
		args.template get<CommaStar<grammar::FunctionArgument>>().template get_view<grammar::FunctionArgument>()
			| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e); })
			| to_vec()
	};
}

NodeStructs::Expression getExpressionStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::BraceArguments&
) {
	NOT_IMPLEMENTED;
	/*NodeStructs::BraceArguments res;
	for (const auto& arg : statement.template get<CommaStar<FunctionArgument>>().template get<FunctionArgument>())
		res.args.push_back(getExpressionStruct(arg));
	return res;*/
}

NodeStructs::Expression getExpressionStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::ParenExpression& statement
) {
	return std::visit(overload(overload_default_error,
		[&](const grammar::Construct& e) -> NodeStructs::Expression {
			return make_expression(NodeStructs::ConstructExpression{
				.operand = getStruct(file_name, vec, e.template get<grammar::Typename>(), tag_expect_empty_category{}),
				.arguments = { e.template get<grammar::BraceArguments>().template get<CommaStar<grammar::FunctionArgument>>().template get<grammar::FunctionArgument>()
					| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e); })
					| to_vec()
				} }, rule_info_from_rule(file_name, vec, e));
		},
		[&](const grammar::ParenArguments& e) -> NodeStructs::Expression {
			return make_expression(NodeStructs::ParenArguments{
				.args = e.template get<CommaStar<grammar::FunctionArgument>>().template get<grammar::FunctionArgument>()
				| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e); })
				| to_vec()
				}, rule_info_from_rule(file_name, vec, e));
		},
		[&](const grammar::BracketArguments& /*e*/) -> NodeStructs::Expression {
			//const auto& args = e.template get<CommaStar<FunctionArgument>>().template get<FunctionArgument>();
			NOT_IMPLEMENTED;
			//auto res = getExpressionStruct(e);
			//return make_expression({ std::move(res) });
		},
		[&](const grammar::BraceArguments& e) -> NodeStructs::Expression {
			const auto& args = e.template get<CommaStar<grammar::FunctionArgument>>().template get<grammar::FunctionArgument>();
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

NodeStructs::Expression getPostfixExpressionStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	NodeStructs::Expression&& expr,
	const grammar::Postfix& postfix
) {
	return std::visit(
		overload(overload_default_error,
			[&](const And<Token<DOT>, grammar::Word>& e) {
				return make_expression(
					NodeStructs::PropertyAccessExpression{ std::move(expr), e.template get<grammar::Word>().value },
					rule_info_from_rule(file_name, vec, e)
				);
			},
			[&](const And<Token<NS>, grammar::Word>& e) {
				return make_expression(NodeStructs::NamespaceExpression{ std::move(expr), e.template get<grammar::Word>().value },
					rule_info_from_rule(file_name, vec, e)
					);
			},
			[&](const And<Token<DOT>, grammar::Word, grammar::ParenArguments>& e) {
				return make_expression(
					NodeStructs::PropertyAccessAndCallExpression{
						std::move(expr),
						e.template get<grammar::Word>().value,
						getStruct(file_name, vec, e.template get<grammar::ParenArguments>())
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
					args.template get<CommaStar<grammar::TypenameOrExpression>>().template get<grammar::TypenameOrExpression>()
					| std::views::transform([&](auto&& e) { return getStruct(file_name, vec, e, tag_allow_value_category_or_empty{}); })
					| to_vec()
					}, rule_info_from_rule(file_name, vec, args));
			}
		),
		postfix.value()
	);
}

NodeStructs::Expression getExpressionStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::PostfixExpression& expr,
	NodeStructs::Expression cur,
	const auto& expressions,
	size_t i
) {
	if (i == expressions.size())
		return cur;
	return getExpressionStruct(file_name, vec, expr, getPostfixExpressionStruct(file_name, vec, std::move(cur), expressions.at(i)), expressions, i + 1);
}

NodeStructs::Expression getExpressionStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::PostfixExpression& expr
) {
	const auto& postfixes = expr.template get<Star<grammar::Postfix>>().template get<grammar::Postfix>();
	auto base = getExpressionStruct(file_name, vec, expr.template get<grammar::ParenExpression>());
	return getExpressionStruct(file_name, vec, expr, std::move(base), postfixes, 0);
}

NodeStructs::Expression getExpressionStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::UnaryExpression& expr
) {
	const auto& prefixes = expr.get().template get<Star<grammar::unary_operators>>().template get<grammar::unary_operators>();
	if (prefixes.size() == 0)
		return getExpressionStruct(file_name, vec, expr.get().template get<grammar::PostfixExpression>());
	else {
		return make_expression(NodeStructs::UnaryExpression{
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
			.expr = getExpressionStruct(file_name, vec, expr.get().template get<grammar::PostfixExpression>())
			}, rule_info_from_rule(file_name, vec, expr));
	}
}

NodeStructs::Expression getExpressionStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::MultiplicativeExpression& expr
) {
	using operators = Or<Token<ASTERISK>, Token<SLASH>, Token<PERCENT>>;
	using VT = Variant<Token<ASTERISK>, Token<SLASH>, Token<PERCENT>>;
	const auto& multiplications = expr.template get<Star<And<operators, grammar::UnaryExpression>>>().template get<And<operators, grammar::UnaryExpression>>();
	if (multiplications.size() == 0)
		return getExpressionStruct(file_name, vec, expr.template get<grammar::UnaryExpression>());
	else
		return make_expression(NodeStructs::MultiplicativeExpression{
			getExpressionStruct(file_name, vec, expr.template get<grammar::UnaryExpression>()),
			multiplications
				| std::views::transform([&](auto&& op_exp) {
					return std::pair{
						VT{ op_exp.template get<operators>().value() },
						getExpressionStruct(file_name, vec, op_exp.template get<grammar::UnaryExpression>())
					};
				})
				| to_vec()
			}, rule_info_from_rule(file_name, vec, expr));
}

NodeStructs::Expression getExpressionStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::AdditiveExpression& expr
) {
	using operators = Or<Token<PLUS>, Token<DASH>>;
	using VT = Variant<Token<PLUS>, Token<DASH>>;
	const auto& additions = expr.template get<Star<And<operators, grammar::MultiplicativeExpression>>>().template get<And<operators, grammar::MultiplicativeExpression>>();
	if (additions.size() == 0)
		return getExpressionStruct(file_name, vec, expr.template get<grammar::MultiplicativeExpression>());
	else
		return make_expression(NodeStructs::AdditiveExpression{
			getExpressionStruct(file_name, vec, expr.template get<grammar::MultiplicativeExpression>()),
			additions
				| std::views::transform([&](auto&& op_exp) {
					return std::pair{
						VT{ op_exp.template get<operators>().value() },
						getExpressionStruct(file_name, vec, op_exp.template get<grammar::MultiplicativeExpression>())
					};
				})
				| to_vec()
			}, rule_info_from_rule(file_name, vec, expr));
}

NodeStructs::Expression getExpressionStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::CompareExpression& expr
) {
	using op_add = And<grammar::CompareOperator, grammar::AdditiveExpression>;
	using VT = Variant<Token<LT>, Token<LTE>, Token<GT>, Token<GTE>>;
	const auto& comparisons = expr.template get<Star<op_add>>().template get<op_add>();
	if (comparisons.size() == 0)
		return getExpressionStruct(file_name, vec, expr.template get<grammar::AdditiveExpression>());
	else
		return make_expression(NodeStructs::CompareExpression{
			getExpressionStruct(file_name, vec, expr.template get<grammar::AdditiveExpression>()),
			comparisons
				| std::views::transform([&](const op_add& op_exp) {
					return std::pair{
						VT{ op_exp.template get<grammar::CompareOperator>().template get<Or<Token<LT>, Token<LTE>, Token<GT>, Token<GTE>>>().value() },
						getExpressionStruct(file_name, vec, op_exp.template get<grammar::AdditiveExpression>())
					};
				})
				| to_vec()
			}, rule_info_from_rule(file_name, vec, expr));
}

NodeStructs::Expression getExpressionStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::EqualityExpression& expr
) {
	using operators = Or<Token<EQUALEQUAL>, Token<NEQUAL>>;
	using VT = Variant<Token<EQUALEQUAL>, Token<NEQUAL>>;
	const auto& equals = expr.template get<Star<And<operators, grammar::CompareExpression>>>().template get<And<operators, grammar::CompareExpression>>();
	if (equals.size() == 0)
		return getExpressionStruct(file_name, vec, expr.template get<grammar::CompareExpression>());
	else
		return make_expression(NodeStructs::EqualityExpression{
			getExpressionStruct(file_name, vec, expr.template get<grammar::CompareExpression>()),
			equals
				| std::views::transform([&](auto&& op_exp) {
					return std::pair{
						VT{ op_exp.template get<operators>().value() },
						getExpressionStruct(file_name, vec, op_exp.template get<grammar::CompareExpression>())
					};
				})
				| to_vec()
			}, rule_info_from_rule(file_name, vec, expr));
}

NodeStructs::Expression getExpressionStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::AndExpression& expr
) {
	const auto& ands = expr.template get<Star<And<Token<AND>, grammar::EqualityExpression>>>().template get<grammar::EqualityExpression>();
	if (ands.size() == 0)
		return getExpressionStruct(file_name, vec, expr.template get<grammar::EqualityExpression>());
	else
		return make_expression(NodeStructs::AndExpression{
			getExpressionStruct(file_name, vec, expr.template get<grammar::EqualityExpression>()),
			ands
				| std::views::transform([&](auto& e) { return getExpressionStruct(file_name, vec, e); })
				| to_vec()
			}, rule_info_from_rule(file_name, vec, expr));
}

NodeStructs::Expression getExpressionStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::OrExpression& expr
) {
	const auto& ors = expr.template get<Star<And<Token<OR>, grammar::AndExpression>>>().template get<grammar::AndExpression>();
	if (ors.size() == 0)
		return getExpressionStruct(file_name, vec, expr.template get<grammar::AndExpression>());
	else
		return make_expression(NodeStructs::OrExpression{
			getExpressionStruct(file_name, vec, expr.template get<grammar::AndExpression>()),
			ors
				| std::views::transform([&](auto& e) { return getExpressionStruct(file_name, vec, e); })
				| to_vec()
			}, rule_info_from_rule(file_name, vec, expr));
}

NodeStructs::Expression getExpressionStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::ConditionalExpression& expr
) {
	const auto& ifElseExpr = expr.template get<Opt<And<
		Token<IF>,
		grammar::OrExpression,
		Token<ELSE>,
		grammar::OrExpression
		>>>();
	if (ifElseExpr.has_value())
		return make_expression(NodeStructs::ConditionalExpression{
			getExpressionStruct(file_name, vec, expr.template get<grammar::OrExpression>()),
			std::pair{
				getExpressionStruct(file_name, vec, ifElseExpr.value().template get<grammar::OrExpression, 0>()),
				getExpressionStruct(file_name, vec, ifElseExpr.value().template get<grammar::OrExpression, 1>())
			}
			}, rule_info_from_rule(file_name, vec, expr));
	else
		return getExpressionStruct(file_name, vec, expr.template get<grammar::OrExpression>());
}

template <typename context>
NodeStructs::Expression getExpressionStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::ExpressionStatement<context>& statement
) {
	return getExpressionStruct(file_name, vec, statement.template get<grammar::Expression>());
}



NodeStructs::WordTypenameOrExpression getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::TypenameOrExpression& t,
	tag_expect_value_category
) {
	return getTypenameOrExpressionStruct<tag_expect_value_category>(file_name, vec, t);
}

NodeStructs::WordTypenameOrExpression getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::TypenameOrExpression& t,
	tag_expect_empty_category
) {
	return getTypenameOrExpressionStruct<tag_expect_empty_category>(file_name, vec, t);
}

NodeStructs::WordTypenameOrExpression getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::TypenameOrExpression& t,
	tag_allow_value_category_or_empty
) {
	return getTypenameOrExpressionStruct<tag_allow_value_category_or_empty>(file_name, vec, t);
}

std::string accumulate_content(const std::vector<TokenValue>& vec, const unsigned& beg, const unsigned& end) {
	std::stringstream ss;
	auto it = beg;
	while (it != end)
		ss << vec[it++].second;
	return ss.str();
}

std::vector<NodeStructs::Expression> getExpressions(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const std::vector<grammar::Expression>& expressions
) {
	return expressions
		| std::views::transform([&](auto&& e) { return getExpressionStruct(file_name, vec, e); })
		| to_vec();
}

template <template <typename> typename Statement, typename context>
NodeStructs::CompileTimeStatement<context> get_compile_time_statement_from_specific(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const Statement<context>& statement
) {
	return { getStatementStruct<context>(file_name, vec, statement) };
}

NodeStructs::RunTimeStatement getStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::RunTimeStatement& statement
) {
	return std::visit([&](const auto& statement) -> NodeStructs::RunTimeStatement {
		return getStatementStruct(file_name, vec, statement);
	}, statement.value());
}

template <typename context, typename... contextuals>
NodeStructs::Statement<context> getStatementStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const Or<grammar::CompileTimeStatement<context>, Or<contextuals...>>& statement
) {
	return std::visit(overload(
		[&](const grammar::CompileTimeStatement<context>& ct_statement) -> NodeStructs::Statement<context> {
			return NodeStructs::Statement<context>{
				NonCopyableBox<Variant<NodeStructs::CompileTimeStatement<context>, NodeStructs::contextual_options<context>>>{
					Variant<NodeStructs::CompileTimeStatement<context>, NodeStructs::contextual_options<context>>{
						get_compile_time_statement(file_name, vec, ct_statement)
					}
				}
			};
		},
		[&](const Or<contextuals...>& x) -> NodeStructs::Statement<context> {
			if constexpr (std::is_same_v<context, function_context>) {
				return { Variant<NodeStructs::CompileTimeStatement<context>, NodeStructs::contextual_options<context>> {
					std::visit(overload(
						[&](const grammar::RunTimeStatement& x) -> NodeStructs::contextual_options<context> {
							return getStruct(file_name, vec, x);
						}
					), x.value())
				} };
			}
			else if constexpr (std::is_same_v<context, type_context>) {
				return { Variant<NodeStructs::CompileTimeStatement<context>, NodeStructs::contextual_options<context>> {
					std::visit(overload(
						[&](const grammar::Alias& x) -> NodeStructs::contextual_options<context> {
							return getStruct(file_name, vec, x, std::nullopt);
						},
						[&](const grammar::MemberVariable& x) -> NodeStructs::contextual_options<context> {
							return getStruct(file_name, vec, x);
						}
					), x.value())
				} };
			}
			else if constexpr (std::is_same_v<context, top_level_context>) {
				NOT_IMPLEMENTED;
			}
			else {
				static_assert(!sizeof(context*), "bad context...???");
			}
		}
	), statement.value());
}

NodeStructs::Statement<function_context> get_base_statement_struct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Statement<function_context>& statement
) {
	return getStatementStruct(file_name, vec, statement.template get<Or<grammar::CompileTimeStatement<function_context>, Or<grammar::RunTimeStatement>>>());
}

NodeStructs::Statement<grammar::type_context> get_base_statement_struct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Statement<grammar::type_context>& statement
) {
	return getStatementStruct(file_name, vec, statement.template get<Or<grammar::CompileTimeStatement<type_context>, Or<grammar::Alias, grammar::MemberVariable>>>());
}

NodeStructs::Statement<grammar::top_level_context> get_base_statement_struct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Statement<grammar::top_level_context>& statement
) {
	NOT_IMPLEMENTED;
}
