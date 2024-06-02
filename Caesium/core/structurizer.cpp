#pragma once
#include "structurizer.hpp"

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

NodeStructs::Typename getStruct(const grammar::Typename& t, const auto& exts, NodeStructs::Typename res, size_t i) {
	if (i == exts.size())
		return std::move(res);
	const auto& ext = exts.at(i);
	return getStruct(t, exts, std::visit(overload(overload_default_error,
		[&](const grammar::NamespaceTypenameExtension& e) -> NodeStructs::Typename {
			return { NodeStructs::NamespacedTypename{ std::move(res), word_or_auto(e.get<Or<Token<AUTO>, grammar::Word>>()) } };
		},
		[&](const grammar::TemplateTypenameExtension& e) -> NodeStructs::Typename {
			return { NodeStructs::TemplatedTypename{
				std::move(res),
				e.get<CommaStar<Alloc<grammar::Typename>>>().get<Alloc<grammar::Typename>>()
				| std::views::transform([&](auto&& e) { return e.get(); })
				| std::views::transform([&](auto&& e) { return getStruct(e); })
				| to_vec()
			} };
		},
		[&](const grammar::UnionTypenameExtension& ext) -> NodeStructs::Typename {
			auto temp = getStruct(ext.get<Alloc<grammar::Typename>>().get());
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
			} };
		}
	), ext.value()), i + 1);
}

NodeStructs::Typename getStruct(const grammar::Typename& t) {
	NodeStructs::Typename res = { NodeStructs::BaseTypename{ word_or_auto(t.get<Or<Token<AUTO>, grammar::Word>>()) } };
	using opts = Or<grammar::NamespaceTypenameExtension, grammar::TemplateTypenameExtension, grammar::UnionTypenameExtension>;
	return getStruct(t, t.get<Star<opts>>().get<opts>(), std::move(res), 0);
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

NodeStructs::Function getStruct(const grammar::Function& f, std::optional<NodeStructs::Typename> name_space) {
	return NodeStructs::Function{
		.name = f.get<grammar::Word>().value,
		.name_space = std::move(name_space),
		.returnType = getStruct(f.get<grammar::Typename>()),
		.parameters = f.get<grammar::FunctionParameters>().get<And<grammar::Typename, grammar::ParameterCategory, grammar::Word>>()
			| std::views::transform([&](auto&& type_and_name) { return NodeStructs::FunctionParameter{
					getStruct(type_and_name.get<grammar::Typename>()),
					getStruct(type_and_name.get<grammar::ParameterCategory>()),
					type_and_name.get<grammar::Word>().value
				};
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
		.templated = t.get<TemplateBody>().value
	};
}

NodeStructs::MemberVariable getStruct(const grammar::MemberVariable& f) {
	return NodeStructs::MemberVariable{
		.type = getStruct(f.get<grammar::Typename>()),
		.name = f.get<grammar::Word>().value,
	};
}

NodeStructs::Alias getStruct(const grammar::Alias& f, std::optional<NodeStructs::Typename> name_space) {
	return NodeStructs::Alias{
		.aliasFrom = f.get<grammar::Word>().value,
		.aliasTo = getStruct(f.get<grammar::Typename>()),
		.name_space = std::move(name_space)
	};
}

NodeStructs::Type getStruct(const grammar::Type& cl, std::optional<NodeStructs::Typename> name_space) {
	auto elems = cl.get<Indent<Star<And<IndentToken, grammar::TypeElement>>>>().get_view<grammar::TypeElement>()
		| std::views::transform([&](auto&& e) { return e.value(); });
	return {
		.name = cl.get<grammar::Word>().value,
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

NodeStructs::NameSpace getStruct(const grammar::NameSpace& ns, std::optional<NodeStructs::Typename> name_space) {
	std::vector<NodeStructs::Type> types;
	std::vector<NodeStructs::Function> functions;
	std::vector<NodeStructs::Interface> interfaces;
	std::vector<NodeStructs::Template> templates;
	std::vector<NodeStructs::Alias> aliases;
	std::vector<NodeStructs::NameSpace> namespaces;
	std::vector<NodeStructs::Enum> enums;
	for (const And<IndentToken, grammar::Named>& named : ns.get<Indent<Star<Or<Token<NEWLINE>, And<IndentToken, grammar::Named>>>>>().get_view<And<IndentToken, grammar::Named>>()) {
		std::visit(
			overload(
				[&](const grammar::Type& e) {
					types.push_back(getStruct(e, copy(name_space)));
				},
				[&](const grammar::Function& e) {
					functions.push_back(getStruct(e, copy(name_space)));
				},
				[&](const grammar::Interface& e) {
					interfaces.push_back(getStruct(e, copy(name_space)));
				},
				[&](const grammar::Template& e) {
					templates.push_back(getStruct(e, copy(name_space)));
				},
				[&](const grammar::Alias& e) {
					aliases.push_back(getStruct(e, copy(name_space)));
				},
				[&](const grammar::NameSpace& e) {
					namespaces.push_back(getStruct(e, copy(name_space)));
				},
				[&](const grammar::Enum& e) {
					enums.push_back(getStruct(e, copy(name_space)));
				}
			), named.get<grammar::Named>().value()
		);
	}


	return {
		.name = ns.get<grammar::Word>().value,
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

NodeStructs::File getStruct(const grammar::File& f, std::string_view fileName) {
	using T = Star<Or<Token<NEWLINE>, grammar::Named>>;
	auto t = f.get<T>().get_view<grammar::Named>()
		| std::views::transform([&](auto&& e) { return e.value(); })
		;

	auto functions = t
		| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::Function>(e); })
		| std::views::transform([&](auto&& e) { return std::get<grammar::Function>(e); })
		| std::views::transform([&](auto&& fn) { return getStruct(fn, std::nullopt); })
		| to_vec();

	return NodeStructs::File{
		.imports = f.get<Star<grammar::Import>>().get<grammar::Import>()
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

NodeStructs::TemplateArguments getStruct(const grammar::TemplateArguments& args) {
	return {
		args.get<CommaStar<grammar::Expression>>().get_view<grammar::Expression>()
			| std::views::transform([&](auto&& e) { return getExpressionStruct(e); })
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
			return { NodeStructs::ConstructExpression{
				.operand = getStruct(e.get<grammar::Typename>()),
				.arguments = e.get<grammar::BraceArguments>().get<CommaStar<grammar::FunctionArgument>>().get<grammar::FunctionArgument>()
				| std::views::transform([&](auto&& e) { return getStruct(e); })
				| to_vec()
			} };
		},
		[](const grammar::ParenArguments& e) -> NodeStructs::Expression {
			return { NodeStructs::ParenArguments{
				.args = e.get<CommaStar<grammar::FunctionArgument>>().get<grammar::FunctionArgument>()
				| std::views::transform([&](auto&& e) { return getStruct(e); })
				| to_vec()
			} };
		},
		[](const grammar::BracketArguments& /*e*/) -> NodeStructs::Expression {
			//const auto& args = e.get<CommaStar<FunctionArgument>>().get<FunctionArgument>();
			throw;
			//auto res = getExpressionStruct(e);
			//return NodeStructs::Expression{ std::move(res) };
		},
		[](const grammar::BraceArguments& e) -> NodeStructs::Expression {
			const auto& args = e.get<CommaStar<grammar::FunctionArgument>>().get<grammar::FunctionArgument>();
			return { NodeStructs::BraceArguments{
				args
				| std::views::transform([&](auto&& e) { return getStruct(e); })
				| to_vec()
			} };
			//auto res = getExpressionStruct(e);
			//return NodeStructs::Expression{ std::move(res) };
		},
		[](const grammar::Word& e) {
			return NodeStructs::Expression{ e.value };
		},
		[](const Token<INTEGER_NUMBER>& e) {
			return NodeStructs::Expression{ e };
		},
		[](const Token<FLOATING_POINT_NUMBER>& e) {
			return NodeStructs::Expression{ e };
		},
		[](const Token<STRING>& e) {
			return NodeStructs::Expression{ e };
		}
	),
		statement.value()
	);
}

NodeStructs::Expression getPostfixExpressionStruct(NodeStructs::Expression&& expr, const grammar::Postfix& postfix) {
	return std::visit(
		overload(overload_default_error,
			[&](const And<Token<DOT>, grammar::Word, grammar::ParenArguments>& e) {
				return NodeStructs::Expression{ NodeStructs::PropertyAccessAndCallExpression{ std::move(expr), e.get<grammar::Word>().value, getStruct(e.get<grammar::ParenArguments>()) } };
			},
			[&](const And<Token<DOT>, grammar::Word>& e) {
				return NodeStructs::Expression{ NodeStructs::PropertyAccessExpression{ std::move(expr), e.get<grammar::Word>().value } };
			},
			[&](const And<Token<NS>, grammar::Word>& e) {
				return NodeStructs::Expression{ NodeStructs::NamespaceExpression{ std::move(expr), e.get<grammar::Word>().value } };
			},
			[&](const grammar::ParenArguments& args) {
				return NodeStructs::Expression{ NodeStructs::CallExpression{ std::move(expr), getStruct(args) } };
			},
			[&](const grammar::BracketArguments& args) {
				return NodeStructs::Expression{ NodeStructs::BracketAccessExpression{ std::move(expr), getStruct(args) } };
			},
			/*[&](const BraceArguments& args) {
				return NodeStructs::Expression{ NodeStructs::ConstructExpression{ std::move(expr), getStruct(args) } };
			},*/
			[&](const grammar::TemplateArguments& args) {
				return NodeStructs::Expression{ NodeStructs::TemplateExpression{ std::move(expr), getStruct(args) } };
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
		return NodeStructs::Expression{ NodeStructs::UnaryExpression {
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
		} };
	}
}

NodeStructs::Expression getExpressionStruct(const grammar::MultiplicativeExpression& statement) {
	using operators = Or<Token<ASTERISK>, Token<SLASH>, Token<PERCENT>>;
	using VT = Variant<Token<ASTERISK>, Token<SLASH>, Token<PERCENT>>;
	const auto& multiplications = statement.get<Star<And<operators, grammar::UnaryExpression>>>().get<And<operators, grammar::UnaryExpression>>();
	if (multiplications.size() == 0)
		return getExpressionStruct(statement.get<grammar::UnaryExpression>());
	else
		return NodeStructs::Expression{ NodeStructs::MultiplicativeExpression{
			getExpressionStruct(statement.get<grammar::UnaryExpression>()),
			multiplications
				| std::views::transform([&](auto&& op_exp) {
					return std::pair{
						VT{ op_exp.get<operators>().value() },
						getExpressionStruct(op_exp.get<grammar::UnaryExpression>())
					};
				})
				| to_vec()
		} };
}

NodeStructs::Expression getExpressionStruct(const grammar::AdditiveExpression& statement) {
	using operators = Or<Token<PLUS>, Token<DASH>>;
	using VT = Variant<Token<PLUS>, Token<DASH>>;
	const auto& additions = statement.get<Star<And<operators, grammar::MultiplicativeExpression>>>().get<And<operators, grammar::MultiplicativeExpression>>();
	if (additions.size() == 0)
		return getExpressionStruct(statement.get<grammar::MultiplicativeExpression>());
	else
		return NodeStructs::Expression{ NodeStructs::AdditiveExpression{
			getExpressionStruct(statement.get<grammar::MultiplicativeExpression>()),
			additions
				| std::views::transform([&](auto&& op_exp) {
					return std::pair{
						VT{ op_exp.get<operators>().value() },
						getExpressionStruct(op_exp.get<grammar::MultiplicativeExpression>())
					};
				})
				| to_vec()
		} };
}

NodeStructs::Expression getExpressionStruct(const grammar::CompareExpression& statement) {
	using op_add = And<grammar::CompareOperator, grammar::AdditiveExpression>;
	using VT = Variant<Token<LTQ>, Token<LTEQ>, Token<GTQ>, Token<GTEQ>>;
	const auto& comparisons = statement.get<Star<op_add>>().get<op_add>();
	if (comparisons.size() == 0)
		return getExpressionStruct(statement.get<grammar::AdditiveExpression>());
	else
		return NodeStructs::Expression{ NodeStructs::CompareExpression{
			getExpressionStruct(statement.get<grammar::AdditiveExpression>()),
			comparisons
				| std::views::transform([&](auto&& op_exp) {
					return std::pair{
						VT{ op_exp.get<grammar::CompareOperator>().value() },
						getExpressionStruct(op_exp.get<grammar::AdditiveExpression>())
					};
				})
				| to_vec()
		} };
}

NodeStructs::Expression getExpressionStruct(const grammar::EqualityExpression& statement) {
	using operators = Or<Token<EQUALEQUAL>, Token<NEQUAL>>;
	using VT = Variant<Token<EQUALEQUAL>, Token<NEQUAL>>;
	const auto& equals = statement.get<Star<And<operators, grammar::CompareExpression>>>().get<And<operators, grammar::CompareExpression>>();
	if (equals.size() == 0)
		return getExpressionStruct(statement.get<grammar::CompareExpression>());
	else
		return NodeStructs::Expression{ NodeStructs::EqualityExpression{
			getExpressionStruct(statement.get<grammar::CompareExpression>()),
			equals
				| std::views::transform([&](auto&& op_exp) {
					return std::pair{
						VT{ op_exp.get<operators>().value() },
						getExpressionStruct(op_exp.get<grammar::CompareExpression>())
					};
				})
				| to_vec()
		} };
}

NodeStructs::Expression getExpressionStruct(const grammar::AndExpression& statement) {
	const auto& ands = statement.get<Star<And<Token<AND>, grammar::EqualityExpression>>>().get<grammar::EqualityExpression>();
	if (ands.size() == 0)
		return getExpressionStruct(statement.get<grammar::EqualityExpression>());
	else
		return NodeStructs::Expression{ NodeStructs::AndExpression{
			getExpressionStruct(statement.get<grammar::EqualityExpression>()),
			ands
				| std::views::transform([&](auto& e) { return getExpressionStruct(e); })
				| to_vec()
		} };
}

NodeStructs::Expression getExpressionStruct(const grammar::OrExpression& statement) {
	const auto& ors = statement.get<Star<And<Token<OR>, grammar::AndExpression>>>().get<grammar::AndExpression>();
	if (ors.size() == 0)
		return getExpressionStruct(statement.get<grammar::AndExpression>());
	else
		return NodeStructs::Expression{ NodeStructs::OrExpression{
			getExpressionStruct(statement.get<grammar::AndExpression>()),
			ors
				| std::views::transform([&](auto& e) { return getExpressionStruct(e); })
				| to_vec()
		} };
}

NodeStructs::Expression getExpressionStruct(const grammar::ConditionalExpression& statement) {
	const auto& ifElseExpr = statement.get<Opt<And<
		Token<IF>,
		grammar::OrExpression,
		Token<ELSE>,
		grammar::OrExpression
		>>>();
	if (ifElseExpr.has_value())
		return NodeStructs::Expression{ NodeStructs::ConditionalExpression{
			getExpressionStruct(statement.get<grammar::OrExpression>()),
			std::pair{
				getExpressionStruct(ifElseExpr.value().get<grammar::OrExpression, 0>()),
				getExpressionStruct(ifElseExpr.value().get<grammar::OrExpression, 1>())
			}
		} };
	else
		return getExpressionStruct(statement.get<grammar::OrExpression>());
}

NodeStructs::Expression getExpressionStruct(const grammar::ExpressionStatement& statement) {
	return getExpressionStruct(statement.get<grammar::Expression>());
}

NodeStructs::Expression getStatementStruct(const grammar::ExpressionStatement& statement) {
	return getExpressionStruct(statement);
}

NodeStructs::VariableDeclarationStatement getStatementStruct(const grammar::VariableDeclarationStatement& statement) {
	return { getStruct(statement.get<grammar::Typename>()), statement.get<grammar::Word>().value, getExpressionStruct(statement.get<grammar::Expression>()) };
}

std::vector<NodeStructs::Statement> getStatements(const grammar::ColonIndentCodeBlock& code) {
	return code.get<Indent<Star<Or<Token<NEWLINE>, grammar::Statement>>>>().get<Or<Token<NEWLINE>, grammar::Statement>>()
		| std::views::transform([&](auto&& e) { return e.value(); })
		| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::Statement>(e); })
		| std::views::transform([&](auto&& e) { return std::get<grammar::Statement>(e); })
		| std::views::transform([&](auto&& e) { return getStatementStruct(e); })
		| to_vec();
}

NodeStructs::BlockStatement getStatementStruct(const grammar::BlockStatement& statement) {
	return { getStruct(statement.get<grammar::Typename>()) };
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

NodeStructs::ForStatement getStatementStruct(const grammar::ForStatement& statement) {
	return {
		.collection = getExpressionStruct(statement.get<grammar::Expression>()),
		.iterators = statement.get<CommaPlus<Or<grammar::VariableDeclaration, grammar::Word>>>().get<Or<grammar::VariableDeclaration, grammar::Word>>()
			| std::views::transform([](const Or<grammar::VariableDeclaration, grammar::Word>& or_node) {
				return std::visit(overload(
					[](const grammar::Word& e) -> Variant<NodeStructs::VariableDeclaration, std::string> {
						return { e.value };
					},
					[](const grammar::VariableDeclaration& e) -> Variant<NodeStructs::VariableDeclaration, std::string> {
						return NodeStructs::VariableDeclaration{ getStruct(e.get<grammar::Typename>()), e.get<grammar::Word>().value };
					}
				), or_node.value());
			})
			| to_vec(),
		.statements = getStatements(statement.get<grammar::ColonIndentCodeBlock>())
	};
}

NodeStructs::IForStatement getStatementStruct(const grammar::IForStatement&) {
	throw;
	/*NodeStructs::IForStatement res {
		statement.get<grammar::Word>().value,
		getExpressionStruct(statement.get<Expression>())
	};
	for (const auto& it : statement.get<CommaPlus<Or<VariableDeclaration, grammar::Word>>>().get<grammar::Word>())
		res.iterators.push_back({ it.value });
	for (const auto& it : statement.get<CommaPlus<Or<VariableDeclaration, grammar::Word>>>().get<VariableDeclaration>())
		res.iterators.push_back(NodeStructs::VariableDeclaration{ getStruct(it.get<Typename>()), it.get<grammar::Word>().value });
	for (const auto& statement : statement.get<ColonIndentCodeBlock>().get<Indent<Star<Statement>>>().get<Statement>())
		res.statements.push_back(getStatementStruct(statement));
	return res;*/
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
		| std::views::transform([&](auto&& vdec) { return std::pair{ getStruct(vdec.get<grammar::Typename>()), vdec.get<grammar::Word>().value }; })
		| to_vec(),
		statements
		| std::views::transform([&](auto&& e) { return e.value(); })
		| std::views::filter([&](auto&& e) { return std::holds_alternative<grammar::Statement>(e); })
		| std::views::transform([&](auto&& e) { return std::get<grammar::Statement>(e); })
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
			statements.get<Indent<Star<Or<Token<NEWLINE>, grammar::Statement>>>>().get_view<Or<Token<NEWLINE>, grammar::Statement>>()
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
	return NodeStructs::Statement{ std::visit(
		[](const auto& statement) -> NodeStructs::Statement {
			return { getStatementStruct(statement) };
		},
		statement.get<Alloc<Or<
			grammar::VariableDeclarationStatement,
			grammar::ExpressionStatement,
			grammar::IfStatement,
			grammar::ForStatement,
			grammar::IForStatement,
			grammar::WhileStatement,
			grammar::BreakStatement,
			grammar::ReturnStatement,
			grammar::BlockStatement,
			grammar::MatchStatement,
			grammar::SwitchStatement,
			grammar::Assignment
		>>>().get().value()) };
}
