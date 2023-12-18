#pragma once
#include "node_structs.h"
#include "grammar.h"
#include <ranges>

NodeStructs::Expression getExpressionStruct(const AssignmentExpression& statement);
NodeStructs::Statement getStatementStruct(const Statement& statement);

NodeStructs::Import getStruct(const Import& f) {
	return { std::visit(
		overload(overload_default_error,
			[](const Word& word) {
				return word.value + ".caesium";
			},
			[](const String& string) {
				return string.value;
		}),
		f.get<Or<Word, String>>().value()
	) };
}

NodeStructs::Typename getStruct(const Typename& t);

NodeStructs::Typename extend(NodeStructs::Typename&& t, const NamespaceTypenameExtension& nst) {
	throw;
	/*return NodeStructs::NamespacedTypename{
		std::move(t),
		getStruct(nst.get<Alloc<Typename>>().get())
	};*/
}

NodeStructs::TemplatedTypename extend_tmpl(NodeStructs::Typename&& t, const std::vector<Alloc<Typename>>& templates) {
	return {
		std::move(t),
		templates
			| LIFT_TRANSFORM_TRAIL(.get())
			| LIFT_TRANSFORM(getStruct)
			| to_vec()
	};
}

NodeStructs::Typename extend(NodeStructs::Typename&& t, const TemplateTypenameExtension& tt) {
	if (tt.get<Opt<NamespaceTypenameExtension>>().has_value())
		return extend(NodeStructs::Typename{ extend_tmpl(std::move(t), tt.get<CommaStar<Alloc<Typename>>>().get<Alloc<Typename>>()) }, tt.get<Opt<NamespaceTypenameExtension>>().value());
	else
		return NodeStructs::Typename{ extend_tmpl(std::move(t), tt.get<CommaStar<Alloc<Typename>>>().get<Alloc<Typename>>()) };
}

NodeStructs::Typename extend(NodeStructs::Typename&& t, const UnionTypenameExtension& ors) {
	std::vector<NodeStructs::Typename> v;
	v.reserve(ors.nodes.size() + 1);
	v.push_back(std::move(t));
	auto ts = ors.get_view<Alloc<Typename>>() | LIFT_TRANSFORM_TRAIL(.get()) | LIFT_TRANSFORM(getStruct);
	v.insert(v.end(), std::make_move_iterator(begin(ts)), std::make_move_iterator(end(ts)));

	return NodeStructs::Typename{ NodeStructs::UnionTypename{
		std::move(v)
	} };
}

NodeStructs::Typename getStruct(const Typename& t) {
	if (!t.get<Opt<Or<NamespaceTypenameExtension, TemplateTypenameExtension, UnionTypenameExtension>>>().has_value())
		return NodeStructs::Typename{ NodeStructs::BaseTypename{ t.get<Word>().value } };
	else
		return std::visit(
			[&](const auto& u) {
				return extend(NodeStructs::Typename{ NodeStructs::BaseTypename{ t.get<Word>().value } }, u);
			},
			t.get<Opt<Or<NamespaceTypenameExtension, TemplateTypenameExtension, UnionTypenameExtension>>>().value().value()
		);
}

NodeStructs::ValueCategory getStruct(const ValueCategory& vc) {
	return std::visit(
		overload(overload_default_error,
			[](const Token<KEY>&) -> NodeStructs::ValueCategory {
				return NodeStructs::Key{};
			},
			[](const Token<VAL>&) -> NodeStructs::ValueCategory {
				return NodeStructs::Value{};
			},
			[](const Token<REF>&) -> NodeStructs::ValueCategory {
				return NodeStructs::Reference{};
			},
			[](const And<Token<REF>, Token<NOT>>&) -> NodeStructs::ValueCategory {
				return NodeStructs::MutableReference{};
			}
		),
		vc.value()
	);
}

NodeStructs::Function getStruct(const Function& f) {
	return NodeStructs::Function{
		.name = f.get<Word>().value,
		.returnType = getStruct(f.get<Typename>()),
		.parameters = f.get<FunctionParameters>().get<And<Typename, ValueCategory, Word>>()
			| LIFT_TRANSFORM_X(type_and_name, std::tuple{ getStruct(type_and_name.get<Typename>()), getStruct(type_and_name.get<ValueCategory>()), type_and_name.get<Word>().value })
			| to_vec(),
		.statements = f.get<ColonIndentCodeBlock>().get<Indent<Star<Statement>>>().get<Statement>()
			| LIFT_TRANSFORM(getStatementStruct)
			| to_vec()
	};
}

NodeStructs::TemplateArguments getStruct(const TemplateDeclaration& t) {
	return {
		t.get<CommaPlus<Word>>().get<Word>()
		| LIFT_TRANSFORM_TRAIL(.value)
		| to_vec()
	};
}

NodeStructs::Template<NodeStructs::Function> getStruct(const Template<Function>& f) {
	throw;
}

NodeStructs::Constructor getStruct(const Constructor& f) {
	throw;
	/*return {
		f.get<FunctionParameters>().get<And<Typename, ValueCategory, Word>>()
			| LIFT_TRANSFORM_X(arg, std::tuple{ getStruct(arg.get<Typename>()), getStruct(arg.get<ValueCategory>()), arg.get<Word>().value })
			| to_vec(),
		f.get<ColonIndentCodeBlock>().get<Indent<Star<Statement>>>().get<Statement>()
			| LIFT_TRANSFORM(getStatementStruct)
			| to_vec()
	};*/
}

NodeStructs::MemberVariable getStruct(const MemberVariable& f) {
	return NodeStructs::MemberVariable{
		.name = f.get<Word>().value,
		.type = getStruct(f.get<Typename>()),
	};
}

NodeStructs::Alias getStruct(const Alias& f) {
	return NodeStructs::Alias{
		.aliasFrom = getStruct(f.get<Typename>()),
		.aliasTo = NodeStructs::BaseTypename{ f.get<Word>().value },
	};
}

NodeStructs::Type getStruct(const Type& cl) {
	auto elems = cl.get<Indent<Star<And<IndentToken, ClassElement>>>>().get_view<ClassElement>() | LIFT_TRANSFORM_TRAIL(.value());
	return {
		.name = cl.get<Word>().value,
		.aliases = elems
			| filter_variant_type_eq<Alias>
			| tranform_variant_type_eq<Alias>
			| LIFT_TRANSFORM(getStruct)
			| to_vec(),
		.constructors = elems
			| filter_variant_type_eq<Constructor>
			| tranform_variant_type_eq<Constructor>
			| LIFT_TRANSFORM(getStruct)
			| to_vec(),
		.methods = elems
			| filter_variant_type_eq<Function>
			| tranform_variant_type_eq<Function>
			| LIFT_TRANSFORM(getStruct)
			| to_vec(),
		.memberVariables = elems
			| filter_transform_variant_type_eq(MemberVariable)
			| LIFT_TRANSFORM(getStruct)
			| to_vec()
	};
}

NodeStructs::Template<NodeStructs::Type> getStruct(const Template<Type>& cl) {
	return {
		getStruct(cl.get<TemplateDeclaration>()),
		getStruct(cl.get<Type>())
	};
}

NodeStructs::File getStruct(const File& f, std::string fileName) {
	using T = Star<Or<Token<NEWLINE>, Type, Function, Template<Type>, Template<Function>, Template<BlockDeclaration>>>;

	return NodeStructs::File{
		.filename = fileName,
		.imports = f.get<Star<Import>>().get<Import>()
			| LIFT_TRANSFORM(getStruct)
			| to_vec(),
		.types = f.get<T>().get<Type>()
			| LIFT_TRANSFORM(getStruct)
			| to_vec(),
		.type_templates = f.get<T>().get<Template<Type>>()
			| LIFT_TRANSFORM(getStruct)
			| to_vec(),
		.functions = f.get<T>().get<Function>()
			| LIFT_TRANSFORM(getStruct)
			| to_vec(),
		.function_templates = f.get<T>().get<Template<Function>>()
			| LIFT_TRANSFORM(getStruct)
			| to_vec(),
	};
}

NodeStructs::ArgumentPassingType getStruct(const Or<Token<COPY>, Token<MOVE>, And<Token<REF>, Token<NOT>>, Token<REF>>& t) {
	return std::visit(overload(overload_default_error,
		[](const Token<COPY>&) -> NodeStructs::ArgumentPassingType {
			return NodeStructs::Copy{};
		},
		[](const Token<MOVE>&) -> NodeStructs::ArgumentPassingType {
			return NodeStructs::Move{};
		},
		[](const And<Token<REF>, Token<NOT>>&) -> NodeStructs::ArgumentPassingType {
			return NodeStructs::MutableReference{};
		},
		[](const Token<REF>&) -> NodeStructs::ArgumentPassingType {
			return NodeStructs::Reference{};
		}),
		t.value()
	);
}

NodeStructs::FunctionArgument getStruct(const FunctionArgument& arg) {
	using call_t = Or<Token<COPY>, Token<MOVE>, And<Token<REF>, Token<NOT>>, Token<REF>>;
	if (arg.get<Opt<call_t>>().has_value())
		return NodeStructs::FunctionArgument{
			getStruct(arg.get<Opt<call_t>>().value()),
			getExpressionStruct(arg.get<Expression>())
		};
	else
		return NodeStructs::FunctionArgument{
			{},
			getExpressionStruct(arg.get<Expression>())
	};
}

NodeStructs::ParenArguments getStruct(const ParenArguments& args) {
	return {
		args.get<CommaStar<FunctionArgument>>().get<FunctionArgument>()
			| LIFT_TRANSFORM(getStruct)
			| to_vec()
	};
}

NodeStructs::BracketArguments getStruct(const BracketArguments& args) {
	return {
		args.get<CommaStar<FunctionArgument>>().get<FunctionArgument>()
			| LIFT_TRANSFORM(getStruct)
			| to_vec()
	};
}

NodeStructs::BraceArguments getStruct(const BraceArguments& args) {
	return {
		args.get<CommaStar<FunctionArgument>>().get<FunctionArgument>()
			| LIFT_TRANSFORM(getStruct)
			| to_vec()
	};
}


/*
Expressions
*/

NodeStructs::Expression getExpressionStruct(const BraceArguments& statement) {
	throw;
	NodeStructs::BraceArguments res;
	/*for (const auto& arg : statement.get<CommaStar<FunctionArgument>>().get<FunctionArgument>())
		res.args.push_back(getExpressionStruct(arg));
	return res;*/
}

NodeStructs::Expression getExpressionStruct(const ParenExpression& statement) {
	return std::visit(overload(overload_default_error,
			[](const ParenArguments& e) -> NodeStructs::Expression {
				const auto& args = e.get<CommaStar<FunctionArgument>>().get<FunctionArgument>();
				throw;
				/*auto res = NodeStructs::ParenArguments{};
				res.args.push_back(getExpressionStruct(e.get<Alloc<FunctionArgument>>().get()));
				return NodeStructs::FunctionArgument{ std::move(res) };*/
			},
			[](const BracketArguments& e) -> NodeStructs::Expression {
				const auto& args = e.get<CommaStar<FunctionArgument>>().get<FunctionArgument>();
				throw;
				//auto res = getExpressionStruct(e);
				//return NodeStructs::Expression{ std::move(res) };
			},
			[](const BraceArguments& e) -> NodeStructs::Expression {
				const auto& args = e.get<CommaStar<FunctionArgument>>().get<FunctionArgument>();
				return { NodeStructs::PostfixExpression{ NodeStructs::BraceArguments{
					args
					| LIFT_TRANSFORM(getStruct)
					| to_vec()
				} } };
				//auto res = getExpressionStruct(e);
				//return NodeStructs::Expression{ std::move(res) };
			},
			[](const Word& e) {
				return NodeStructs::Expression{ e.value };
			},
			[](const Token<NUMBER>& e) {
				return NodeStructs::Expression{ e };
			},
			[](const Token<STRING>& e) {
				return NodeStructs::Expression{ e };
			}
		),
		statement.value()
	);
}

NodeStructs::Expression getExpressionStruct(const PostfixExpression& statement) {
	using opts = Or<
		And<
			Token<DOT>,
			Word
		>,
		ParenArguments,
		BracketArguments,
		BraceArguments,
		Token<PLUSPLUS>,
		Token<MINUSMINUS>
	>;

	const auto& postfixes = statement.get<Star<opts>>().get<opts>();
	if (postfixes.size() == 0) {
		return getExpressionStruct(statement.get<ParenExpression>());
	} else {
		using nodestruct_opts = NodeStructs::PostfixExpression::op_types;
		return NodeStructs::Expression{ NodeStructs::PostfixExpression {
			getExpressionStruct(statement.get<ParenExpression>()),
			postfixes | std::views::transform(
				[](const auto& e) {
					return std::visit(
						overload(overload_default_error,
							[](const And<Token<DOT>, Word>& e) {
								return nodestruct_opts{ e.get<Word>().value };
							},
							[](const ParenArguments& args) {
								return nodestruct_opts{ getStruct(args) };
							},
							[](const BracketArguments& args) {
								return nodestruct_opts{ getStruct(args) };
							},
							[](const BraceArguments& args) {
								return nodestruct_opts{ getStruct(args) };
							},
							[](const Token<PLUSPLUS>& token) {
								return nodestruct_opts{ token };
							},
							[](const Token<MINUSMINUS>& token) {
								return nodestruct_opts{ token };
							}
						),
						e.value()
					);
				})
			| to_vec()
		} };
	}
}

NodeStructs::Expression getExpressionStruct(const UnaryExpression& statement) {
	using op_types = NodeStructs::UnaryExpression::op_types;
	return std::visit(
		overload(overload_default_error,
			[](const PostfixExpression& expr) {
				return getExpressionStruct(expr);
			},
			[](const And<
				Or<	// has to be recursive because of the type cast operator taking the same shape as a ParenExpression
					// so instead of `Star<Or> ... ____` we refer to UnaryExpression inside the Or
					Token<NOT>,
					Token<PLUS>,
					Token<DASH>,
					Token<PLUSPLUS>,
					Token<MINUSMINUS>,
					Token<TILDE>,
					Token<ASTERISK>,
					Token<AMPERSAND>,
					And< // type cast operator
						Token<PARENOPEN>,
						Typename,
						Token<PARENCLOSE>
					>
				>,
				Alloc<UnaryExpression> // recursive here
			>& op_and_unary) -> NodeStructs::Expression {
				const auto& op = op_and_unary.get<Or<
					Token<NOT>,
					Token<PLUS>,
					Token<DASH>,
					Token<PLUSPLUS>,
					Token<MINUSMINUS>,
					Token<TILDE>,
					Token<ASTERISK>,
					Token<AMPERSAND>,
					And< // type cast operator
						Token<PARENOPEN>,
						Typename,
						Token<PARENCLOSE>
					>
				>>();
				return std::visit(overload(
					[&](const auto& token) -> NodeStructs::Expression {
						throw;
						/*auto res = NodeStructs::UnaryExpression{
							getExpressionStruct(op_and_unary.get<Alloc<UnaryExpression>>().get()),
							token,
						};
						return NodeStructs::Expression{ std::move(res) };*/
					},
					[&](const And< // type cast operator
						Token<PARENOPEN>,
						Typename,
						Token<PARENCLOSE>
					>& g) -> NodeStructs::Expression {
						throw;
						/*auto res = NodeStructs::UnaryExpression{
							getExpressionStruct(op_and_unary.get<Alloc<UnaryExpression>>().get()),
							getStruct(g.get<Typename>()),
						};
						return NodeStructs::Expression{ std::move(res) };*/
					}
				), op.value());
			}
		),
		statement.value()
	);
}

NodeStructs::Expression getExpressionStruct(const MultiplicativeExpression& statement) {
	using operators = Or<Token<ASTERISK>, Token<SLASH>, Token<PERCENT>>;
	const auto& multiplications = statement.get<Star<And<operators, UnaryExpression>>>().get<And<operators, UnaryExpression>>();
	if (multiplications.size() == 0)
		return getExpressionStruct(statement.get<UnaryExpression>());
	else
		return NodeStructs::Expression{ NodeStructs::MultiplicativeExpression{
			getExpressionStruct(statement.get<UnaryExpression>()),
			multiplications
				| LIFT_TRANSFORM_X(op_exp, std::pair{
						operators::variant_t{ op_exp.get<operators>().value() },
						getExpressionStruct(op_exp.get<UnaryExpression>())
					})
				| to_vec()
		} };
}

NodeStructs::Expression getExpressionStruct(const AdditiveExpression& statement) {
	using operators = Or<Token<PLUS>, Token<DASH>>;
	const auto& additions = statement.get<Star<And<operators, MultiplicativeExpression>>>().get<And<operators, MultiplicativeExpression>>();
	if (additions.size() == 0)
		return getExpressionStruct(statement.get<MultiplicativeExpression>());
	else
		return NodeStructs::Expression{ NodeStructs::AdditiveExpression{
			getExpressionStruct(statement.get<MultiplicativeExpression>()),
			additions
				| LIFT_TRANSFORM_X(op_exp, std::pair{
						operators::variant_t{ op_exp.get<operators>().value() },
						getExpressionStruct(op_exp.get<MultiplicativeExpression>())
					})
				| to_vec()
		} };
}

NodeStructs::Expression getExpressionStruct(const CompareExpression& statement) {
	using operators = Or<Token<LT>, Token<LTE>, Token<GT>, Token<GTE>>;
	const auto& comparisons = statement.get<Star<And<operators, AdditiveExpression>>>().get<And<operators, AdditiveExpression>>();
	if (comparisons.size() == 0)
		return getExpressionStruct(statement.get<AdditiveExpression>());
	else
		return NodeStructs::Expression{ NodeStructs::CompareExpression{
			getExpressionStruct(statement.get<AdditiveExpression>()),
			comparisons
				| LIFT_TRANSFORM_X(op_exp, std::pair{
						operators::variant_t{ op_exp.get<operators>().value() },
						getExpressionStruct(op_exp.get<AdditiveExpression>())
					})
				| to_vec()
		} };
}

NodeStructs::Expression getExpressionStruct(const EqualityExpression& statement) {
	using operators = Or<Token<EQUALEQUAL>, Token<NEQUAL>>;
	const auto& equals = statement.get<Star<And<operators, CompareExpression>>>().get<And<operators, CompareExpression>>();
	if (equals.size() == 0)
		return getExpressionStruct(statement.get<CompareExpression>());
	else
		return NodeStructs::Expression{ NodeStructs::EqualityExpression{
			getExpressionStruct(statement.get<CompareExpression>()),
			equals
				| LIFT_TRANSFORM_X(op_exp, std::pair{
						operators::variant_t{ op_exp.get<operators>().value() },
						getExpressionStruct(op_exp.get<CompareExpression>())
					})
				| to_vec()
		} };
}

NodeStructs::Expression getExpressionStruct(const AndExpression& statement) {
	const auto& ands = statement.get<Star<And<Token<AND>, EqualityExpression>>>().get<EqualityExpression>();
	if (ands.size() == 0)
		return getExpressionStruct(statement.get<EqualityExpression>());
	else
		return NodeStructs::Expression{ NodeStructs::AndExpression{
			getExpressionStruct(statement.get<EqualityExpression>()),
			ands
				| LIFT_TRANSFORM(getExpressionStruct)
				| to_vec()
		} };
}

NodeStructs::Expression getExpressionStruct(const OrExpression& statement) {
	const auto& ors = statement.get<Star<And<Token<OR>, AndExpression>>>().get<AndExpression>();
	if (ors.size() == 0)
		return getExpressionStruct(statement.get<AndExpression>());
	else
		return NodeStructs::Expression{ NodeStructs::OrExpression{
			getExpressionStruct(statement.get<AndExpression>()),
			ors
				| LIFT_TRANSFORM(getExpressionStruct)
				| to_vec()
		} };
}

NodeStructs::Expression getExpressionStruct(const ConditionalExpression& statement) {
	const auto& ifElseExpr = statement.get<Opt<And<
			Token<IF>,
			OrExpression,
			Token<ELSE>,
			OrExpression
		>>>();
	if (ifElseExpr.has_value())
		return NodeStructs::Expression{ NodeStructs::ConditionalExpression{
			getExpressionStruct(statement.get<OrExpression>()),
			std::pair{
				getExpressionStruct(ifElseExpr.value().get<OrExpression, 0>()),
				getExpressionStruct(ifElseExpr.value().get<OrExpression, 1>())
			}
		} };
	else
		return getExpressionStruct(statement.get<OrExpression>());
}

NodeStructs::Expression getExpressionStruct(const AssignmentExpression& statement) {
	using operators = Or<
		Token<EQUAL>,
		Token<PLUSEQUAL>,
		Token<MINUSEQUAL>,
		Token<TIMESEQUAL>,
		Token<DIVEQUAL>,
		Token<MODEQUAL>,
		Token<ANDEQUAL>,
		Token<OREQUAL>,
		Token<XOREQUAL>
	>;

	const auto& assignments = statement.get<Star<And<operators, ConditionalExpression>>>().get<And<operators, ConditionalExpression>>();
	if (assignments.size() == 0)
		return getExpressionStruct(statement.get<ConditionalExpression>());
	else
		return NodeStructs::Expression{ NodeStructs::AssignmentExpression{
			getExpressionStruct(statement.get<ConditionalExpression>()),
			assignments
				| LIFT_TRANSFORM_X(op_exp, std::pair{
						operators::variant_t{ op_exp.get<operators>().value() },
						getExpressionStruct(op_exp.get<ConditionalExpression>())
					})
				| to_vec()
		} };
}



/*
Statements
*/

NodeStructs::Expression getExpressionStruct(const ExpressionStatement& statement) {
	return getExpressionStruct(statement.get<Expression>());
}

NodeStructs::Expression getStatementStruct(const ExpressionStatement& statement) {
	return getExpressionStruct(statement);
}

NodeStructs::VariableDeclarationStatement getStatementStruct(const VariableDeclarationStatement& statement) {
	return { getStruct(statement.get<Typename>()), statement.get<Word>().value, getExpressionStruct(statement.get<Expression>()) };
}

std::vector<NodeStructs::Statement> getStatements(const ColonIndentCodeBlock& code) {
	return code.get<Indent<Star<Statement>>>().get<Statement>()
		| LIFT_TRANSFORM(getStatementStruct)
		| to_vec();
}

NodeStructs::BlockStatement getStatementStruct(const BlockStatement& statement) {
	return { getStruct(statement.get<Typename>()) };
}

NodeStructs::IfStatement getStatementStruct(const IfStatement& statement) {
	using T = std::variant<Box<NodeStructs::IfStatement>, std::vector<NodeStructs::Statement>>;
	return {
		getExpressionStruct(statement.get<Expression>()),
		statement.get<ColonIndentCodeBlock>().get<Indent<Star<Statement>>>().get<Statement>()
			| LIFT_TRANSFORM(getStatementStruct)
			| to_vec(),
		statement.get<Opt<Alloc<ElseStatement>>>().node.transform([](const auto& e) -> T {
			return std::visit(
				overload(overload_default_error,
					[&](const Alloc<IfStatement>& e) -> T {
						return getStatementStruct(e.get());
					},
					[&](const ColonIndentCodeBlock& e) -> T {
						return getStatements(e);
					}
				),
				e.get()
				.get<Or<Alloc<IfStatement>, ColonIndentCodeBlock>>()
				.value()
			);
		})
	};
}

NodeStructs::ForStatement getStatementStruct(const ForStatement& statement) {
	throw;
	/*return {
		.collection = getExpressionStruct(statement.get<Expression>()),
		.iterators = statement.get<CommaPlus<Or<VariableDeclaration, Word>>>().get<Or<VariableDeclaration, Word>>()
			| std::views::transform([](const Or<VariableDeclaration, Word>& or_node) {
				return std::visit(overload(
					[](const Word& e) -> std::variant<NodeStructs::VariableDeclaration, std::string> {
						return { e.value };
					},
					[](const VariableDeclaration& e) -> std::variant<NodeStructs::VariableDeclaration, std::string> {
						return NodeStructs::VariableDeclaration{ getStruct(e.get<Typename>()), e.get<Word>().value };
					}
				), or_node.value());
			})
			| to_vec(),
		.statements = statement.get<ColonIndentCodeBlock>().get<Indent<Star<Statement>>>().get<Statement>()
			| LIFT_TRANSFORM(getStatementStruct)
			| to_vec()
	};*/
}

NodeStructs::IForStatement getStatementStruct(const IForStatement& statement) {
	throw;
	/*NodeStructs::IForStatement res {
		statement.get<Word>().value,
		getExpressionStruct(statement.get<Expression>())
	};
	for (const auto& it : statement.get<CommaPlus<Or<VariableDeclaration, Word>>>().get<Word>())
		res.iterators.push_back({ it.value });
	for (const auto& it : statement.get<CommaPlus<Or<VariableDeclaration, Word>>>().get<VariableDeclaration>())
		res.iterators.push_back(NodeStructs::VariableDeclaration{ getStruct(it.get<Typename>()), it.get<Word>().value });
	for (const auto& statement : statement.get<ColonIndentCodeBlock>().get<Indent<Star<Statement>>>().get<Statement>())
		res.statements.push_back(getStatementStruct(statement));
	return res;*/
}

NodeStructs::WhileStatement getStatementStruct(const WhileStatement& statement) {
	return {
		getExpressionStruct(statement.get<Expression>()),
		getStatements(statement.get<ColonIndentCodeBlock>())
	};
}

NodeStructs::BreakStatement getStatementStruct(const BreakStatement& statement) {
	return {
		statement.get<Opt<And<Token<IF>, Expression>>>().has_value()
			? getExpressionStruct(statement.get<Opt<And<Token<IF>, Expression>>>().value().get<Expression>())
			: std::optional<NodeStructs::Expression>{}
	};
}

std::vector<NodeStructs::Expression> getExpressions(const std::vector<Expression>& vec) {
	return vec
		| LIFT_TRANSFORM(getExpressionStruct)
		| to_vec();
}

NodeStructs::ReturnStatement getStatementStruct(const ReturnStatement& statement) {
	return {
		getExpressions(statement.get<CommaStar<Expression>>().get<Expression>()),
		statement.get<Opt<And<Token<IF>, Expression>>>().has_value()
			? getExpressionStruct(statement.get<Opt<And<Token<IF>, Expression>>>().value().get<Expression>())
			: std::optional<NodeStructs::Expression>{}
	};
}

NodeStructs::Statement getStatementStruct(const Statement& statement) {
	return NodeStructs::Statement{ std::visit(
		[](const auto& statement) -> NodeStructs::Statement {
			return { getStatementStruct(statement) };
		},
		statement.get<Alloc<Or<
			VariableDeclarationStatement,
			ExpressionStatement,
			IfStatement,
			ForStatement,
			IForStatement,
			WhileStatement,
			BreakStatement,
			ReturnStatement,
			BlockStatement
		>>>().get().value()) };
}
