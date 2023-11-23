#pragma once
#include "node_structs.h"
#include "grammar.h"
#include <ranges>
#include "cpp23_ranges_to.h"

NodeStructs::Expression getExpressionStruct(const AssignmentExpression& statement);
NodeStructs::Statement getStatementStruct(const Statement& statement);

NodeStructs::Import getStruct(const Import& f) {
	NodeStructs::Import res;
	res.imported = std::visit(
		overload(
			[](const Word& word) {
				return word.value + ".caesium";
			},
			[](const String& string) {
				return string.value;
		}), f.get<Or<Word, String>>().value());
	return res;
}

NodeStructs::Typename getStruct(const Typename& t);

NodeStructs::Typename extend(NodeStructs::Typename&& t, const NSTypename& nst) {
	return NodeStructs::NamespacedTypename{
		std::move(t),
		getStruct(nst.get<Alloc<Typename>>().get())
	};
}
NodeStructs::TemplatedTypename extend_tmpl(NodeStructs::Typename&& t, const std::vector<Alloc<Typename>>& templates) {
	NodeStructs::TemplatedTypename res{
		std::move(t),
		{}
	};
	res.templated_with.reserve(templates.size());
	for (const auto& tn : templates)
		res.templated_with.push_back(getStruct(tn.get()));
	return res;
}

NodeStructs::Typename extend(NodeStructs::Typename&& t, const TemplateTypename& tt) {
	if (tt.get<Opt<NSTypename>>().has_value())
		return extend(extend_tmpl(std::move(t), tt.get<CommaStar<Alloc<Typename>>>().get<Alloc<Typename>>()), tt.get<Opt<NSTypename>>().value());
	else
		return extend_tmpl(std::move(t), tt.get<CommaStar<Alloc<Typename>>>().get<Alloc<Typename>>());
}

NodeStructs::Typename getStruct(const Typename& t) {
	NodeStructs::Typename res = NodeStructs::BaseTypename{ t.get<Word>().value };
	if (t.get<Opt<Or<NSTypename, TemplateTypename>>>().has_value())
		return std::visit([&res](const auto& t) {
			return extend(std::move(res), t);
		}, t.get<Opt<Or<NSTypename, TemplateTypename>>>().value().value());
	else
		return res;
}

NodeStructs::ValueCategory getStruct(const ValueCategory& vc) {
	return NodeStructs::ValueCategory{
		std::visit(
			overload(
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
		)
	};
}

NodeStructs::Function getStruct(const Function& f) {
	NodeStructs::Function res{
		.name = f.get<Word>().value,
		.returnType = getStruct(f.get<Typename>()),
		.parameters = {},
		.statements = {}
	};
	for (const auto& statement : f.get<ColonIndentCodeBlock>().get<Indent<Star<Statement>>>().get<Statement>())
		res.statements.push_back(getStatementStruct(statement));
	for (const auto& type_and_name : f.get<FunctionParameters>().get<And<Typename, ValueCategory, Word>>())
		res.parameters.push_back({ getStruct(type_and_name.get<Typename>()), getStruct(type_and_name.get<ValueCategory>()), type_and_name.get<Word>().value });
	return res;
}

NodeStructs::TemplateArguments getStruct(const TemplateDeclaration& t) {
	return {
		t.get<CommaPlus<Word>>().get<Word>()
		| std::views::transform([](const Word& w) { return w.value; })
		| std::ranges::to<std::vector>()
	};
}

NodeStructs::Template<NodeStructs::Function> getStruct(const Template<Function>& f) {
	throw std::runtime_error("");
}

NodeStructs::Constructor getStruct(const Constructor& f) {
	NodeStructs::Constructor res;
	for (const auto& statement : f.get<ColonIndentCodeBlock>().get<Indent<Star<Statement>>>().get<Statement>())
		res.statements.push_back(getStatementStruct(statement));
	for (const auto& arg : f.get<FunctionParameters>().get<And<Typename, ValueCategory, Word>>())
		res.parameters.push_back({ getStruct(arg.get<Typename>()), getStruct(arg.get<ValueCategory>()), arg.get<Word>().value });
	return res;
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
	NodeStructs::Type computedClass{ cl.get<Word>().value };
	for (const ClassElement& ce : cl.get<Indent<Star<And<IndentToken, ClassElement>>>>().get<ClassElement>())
		std::visit([&computedClass](const auto& e) {
		computedClass.get<decltype(getStruct(e))>().push_back(getStruct(e));
			}, ce.value());
	return computedClass;
}

NodeStructs::Template<NodeStructs::Type> getStruct(const Template<Type>& cl) {
	return {
		getStruct(cl.get<TemplateDeclaration>()),
		getStruct(cl.get<Type>())
	};
}

NodeStructs::File getStruct(const File& f, std::string fileName) {
	NodeStructs::File res;
	res.filename = fileName;
	for (const Import& import : f.get<Star<Import>>().get<Import>())
		res.imports.push_back(getStruct(import));

	using T = Star<Or<Token<NEWLINE>, Type, Function, Template<Type>, Template<Function>, Template<BlockDeclaration>>>;

	for (const Type& t : f.get<T>().get<Type>())
		res.types.push_back(getStruct(t));
	for (const Template<Type>& t : f.get<T>().get<Template<Type>>())
		res.type_templates.push_back(getStruct(t));

	for (const Function& fun : f.get<T>().get<Function>())
		res.functions.push_back(getStruct(fun));
	for (const Template<Function>& fun : f.get<T>().get<Template<Function>>())
		res.function_templates.push_back(getStruct(fun));
	return res;
}

NodeStructs::ArgumentPassingType getStruct(const Or<Token<COPY>, Token<MOVE>, And<Token<REF>, Token<NOT>>, Token<REF>>& t) {
	return std::visit(overload(
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
	NodeStructs::ParenArguments res{};
	for (const auto& arg : args.get<CommaStar<FunctionArgument>>().get<FunctionArgument>())
		res.args.push_back(getStruct(arg));
	return res;
}

NodeStructs::BracketArguments getStruct(const BracketArguments& args) {
	NodeStructs::BracketArguments res{};
	for (const auto& arg : args.get<CommaStar<FunctionArgument>>().get<FunctionArgument>())
		res.args.push_back(getStruct(arg));
	return res;
}



/*
Expressions
*/

NodeStructs::Expression getExpressionStruct(const BraceArguments& statement) {
	throw std::runtime_error("");
	NodeStructs::BraceArguments res;
	/*for (const auto& arg : statement.get<CommaStar<FunctionArgument>>().get<FunctionArgument>())
		res.args.push_back(getExpressionStruct(arg));
	return res;*/
}

NodeStructs::Expression getExpressionStruct(const ParenExpression& statement) {
	return std::visit(overload(
			[](const ParenArguments& e) -> NodeStructs::Expression {
			throw std::runtime_error("");
				/*auto res = NodeStructs::ParenArguments{};
				res.args.push_back(getExpressionStruct(e.get<Alloc<FunctionArgument>>().get()));
				return NodeStructs::FunctionArgument{ std::move(res) };*/
			},
			[](const BracketArguments& e) -> NodeStructs::Expression {
				throw std::runtime_error("");
				//auto res = getExpressionStruct(e);
				//return NodeStructs::Expression{ std::move(res) };
			},
			[](const BraceArguments& e) -> NodeStructs::Expression {
				throw std::runtime_error("");
				//auto res = getExpressionStruct(e);
				//return NodeStructs::Expression{ std::move(res) };
			},
			[](const Word& e) -> NodeStructs::Expression {
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
		throw std::runtime_error("");
		/*auto res = NodeStructs::PostfixExpression{ getExpressionStruct(statement.get<ParenExpression>()), {} };
		using nodestruct_opts = NodeStructs::PostfixExpression::op_types;
		for (const auto& n : postfixes) {
			res.postfixes.push_back(
				std::visit(
					overload(
						[](const And<Token<DOT>, Word>& e) {
							return nodestruct_opts{ e.get<Word>().value };
						},
						[](const ParenArguments& args) {
							return nodestruct_opts{ getStruct(args) };
						},
						[](const BracketArguments& args) {
							return nodestruct_opts{ getStruct(args) };
						},
						[](const Token<PLUSPLUS>& token) {
							return nodestruct_opts{ token };
						},
						[](const Token<MINUSMINUS>& token) {
							return nodestruct_opts{ token };
						}
					),
					n.value()
				)
			);
		}
		return NodeStructs::Expression{ std::move(res) };*/
	}
}

NodeStructs::Expression getExpressionStruct(const UnaryExpression& statement) {
	using op_types = NodeStructs::UnaryExpression::op_types;
	return std::visit(
		overload(
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
					[&](const auto& token) {
						auto res = NodeStructs::UnaryExpression{
							getExpressionStruct(op_and_unary.get<Alloc<UnaryExpression>>().get()),
							token,
						};
						return NodeStructs::Expression{ std::move(res) };
					},
					[&](const And< // type cast operator
						Token<PARENOPEN>,
						Typename,
						Token<PARENCLOSE>
					>& g) {
						auto res = NodeStructs::UnaryExpression{
							getExpressionStruct(op_and_unary.get<Alloc<UnaryExpression>>().get()),
							getStruct(g.get<Typename>()),
						};
						return NodeStructs::Expression{ std::move(res) };
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
	else {
		auto res = NodeStructs::MultiplicativeExpression{ getExpressionStruct(statement.get<UnaryExpression>()) };
		for (const auto& op_exp : multiplications)
			res.muls.push_back({
				operators::variant_t{ op_exp.get<operators>().value() },
				getExpressionStruct(op_exp.get<UnaryExpression>())
			});
		return NodeStructs::Expression{ std::move(res) };
	}
}

NodeStructs::Expression getExpressionStruct(const AdditiveExpression& statement) {
	using operators = Or<Token<PLUS>, Token<DASH>>;
	const auto& additions = statement.get<Star<And<operators, MultiplicativeExpression>>>().get<And<operators, MultiplicativeExpression>>();
	if (additions.size() == 0)
		return getExpressionStruct(statement.get<MultiplicativeExpression>());
	else {
		auto res = NodeStructs::AdditiveExpression{ getExpressionStruct(statement.get<MultiplicativeExpression>()) };
		for (const auto& op_exp : additions)
			res.adds.push_back({
				operators::variant_t{ op_exp.get<operators>().value() },
				getExpressionStruct(op_exp.get<MultiplicativeExpression>())
			});
		return NodeStructs::Expression{ std::move(res) };
	}
}

NodeStructs::Expression getExpressionStruct(const CompareExpression& statement) {
	using operators = Or<Token<LT>, Token<LTE>, Token<GT>, Token<GTE>>;
	const auto& comparisons = statement.get<Star<And<operators, AdditiveExpression>>>().get<And<operators, AdditiveExpression>>();
	if (comparisons.size() == 0)
		return getExpressionStruct(statement.get<AdditiveExpression>());
	else {
		auto res = NodeStructs::CompareExpression{ getExpressionStruct(statement.get<AdditiveExpression>()) };
		for (const auto& op_exp : comparisons)
			res.comparisons.push_back({
				operators::variant_t{ op_exp.get<operators>().value() },
				getExpressionStruct(op_exp.get<AdditiveExpression>())
			});
		return NodeStructs::Expression{ std::move(res) };
	}
}

NodeStructs::Expression getExpressionStruct(const EqualityExpression& statement) {
	using operators = Or<Token<EQUALEQUAL>, Token<NEQUAL>>;
	const auto& equals = statement.get<Star<And<operators, CompareExpression>>>().get<And<operators, CompareExpression>>();
	if (equals.size() == 0)
		return getExpressionStruct(statement.get<CompareExpression>());
	else {
		NodeStructs::EqualityExpression res{ getExpressionStruct(statement.get<CompareExpression>()) };
		for (const auto& op_exp : equals)
			res.equals.push_back({
				operators::variant_t{ op_exp.get<operators>().value() },
				getExpressionStruct(op_exp.get<CompareExpression>())
			});
		return NodeStructs::Expression{ std::move(res) };
	}
}

NodeStructs::Expression getExpressionStruct(const AndExpression& statement) {
	const auto& ands = statement.get<Star<And<Token<AND>, EqualityExpression>>>().get<EqualityExpression>();
	if (ands.size() == 0) {
		return getExpressionStruct(statement.get<EqualityExpression>());
	}
	else {
		NodeStructs::AndExpression res{ getExpressionStruct(statement.get<EqualityExpression>()) };
		for (const auto& n : ands)
			res.ands.push_back(getExpressionStruct(n));
		return NodeStructs::Expression{ std::move(res) };
	}
}

NodeStructs::Expression getExpressionStruct(const OrExpression& statement) {
	const auto& ors = statement.get<Star<And<Token<OR>, AndExpression>>>().get<AndExpression>();
	if (ors.size() == 0) {
		return getExpressionStruct(statement.get<AndExpression>());
	}
	else {
		NodeStructs::OrExpression res{ getExpressionStruct(statement.get<AndExpression>()) };
		for (const auto& n : ors)
			res.ors.push_back(getExpressionStruct(n));
		return NodeStructs::Expression{ std::move(res) };
	}
}

NodeStructs::Expression getExpressionStruct(const ConditionalExpression& statement) {
	const auto& ifElseExpr = statement.get<Opt<And<
			Token<IF>,
			OrExpression,
			Token<ELSE>,
			OrExpression
		>>>();
	if (ifElseExpr.has_value()) {
		NodeStructs::ConditionalExpression res{ getExpressionStruct(statement.get<OrExpression>()) };
		res.ifElseExprs = {
			getExpressionStruct(ifElseExpr.value().get<OrExpression, 0>()),
			getExpressionStruct(ifElseExpr.value().get<OrExpression, 1>())
		};
		return NodeStructs::Expression{ std::move(res) };
	}
	else {
		return getExpressionStruct(statement.get<OrExpression>());
	}
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
	if (assignments.size() == 0) {
		return getExpressionStruct(statement.get<ConditionalExpression>());
	}
	else {
		auto res = NodeStructs::AssignmentExpression{ getExpressionStruct(statement.get<ConditionalExpression>()), {} };
		for (const auto& op_exp : assignments)
			res.assignments.push_back({
				operators::variant_t{ op_exp.get<operators>().value() },
				getExpressionStruct(op_exp.get<ConditionalExpression>())
			});
		return NodeStructs::Expression{ std::move(res) };
	}
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
	std::vector<NodeStructs::Statement> res;
	for (const auto& statement : code.get<Indent<Star<Statement>>>().get<Statement>())
		res.push_back(getStatementStruct(statement));
	return res;
}

NodeStructs::BlockStatement getStatementStruct(const BlockStatement& statement) {
	return { getStruct(statement.get<Typename>()) };
}

NodeStructs::IfStatement getStatementStruct(const IfStatement& statement) {
	NodeStructs::IfStatement res{ getExpressionStruct(statement.get<Expression>()) };
	for (const auto& statement : statement.get<ColonIndentCodeBlock>().get<Indent<Star<Statement>>>().get<Statement>())
		res.ifStatements.push_back(getStatementStruct(statement));
	if (statement.get<Opt<Alloc<ElseStatement>>>().has_value()) {
		const auto& elseWithCndOrNot = statement
										.get<Opt<Alloc<ElseStatement>>>()
										.value()
										.get()
										.get<Or<Alloc<IfStatement>, ColonIndentCodeBlock>>()
										.value();
		res.elseExprStatements = std::visit(
			overload(
				[&](const Alloc<IfStatement>& e) -> decltype(res.elseExprStatements) {
					return getStatementStruct(e.get());
				},
				[&](const ColonIndentCodeBlock& e) -> decltype(res.elseExprStatements) {
					return getStatements(e);
				}
			),
			elseWithCndOrNot
		);
	}
	return res;
}

NodeStructs::ForStatement getStatementStruct(const ForStatement& statement) {
	NodeStructs::ForStatement res { getExpressionStruct(statement.get<Expression>()) };
	for (const auto& it : statement.get<CommaPlus<Or<VariableDeclaration, Word>>>().get<Word>())
		res.iterators.push_back({ it.value });
	for (const auto& it : statement.get<CommaPlus<Or<VariableDeclaration, Word>>>().get<VariableDeclaration>())
		res.iterators.push_back(NodeStructs::VariableDeclaration{ getStruct(it.get<Typename>()), it.get<Word>().value });
	for (const auto& statement : statement.get<ColonIndentCodeBlock>().get<Indent<Star<Statement>>>().get<Statement>())
		res.statements.push_back(getStatementStruct(statement));
	return res;
}

NodeStructs::IForStatement getStatementStruct(const IForStatement& statement) {
	NodeStructs::IForStatement res {
		statement.get<Word>().value,
		getExpressionStruct(statement.get<Expression>())
	};
	for (const auto& it : statement.get<CommaPlus<Or<VariableDeclaration, Word>>>().get<Word>())
		res.iterators.push_back({ it.value });
	for (const auto& it : statement.get<CommaPlus<Or<VariableDeclaration, Word>>>().get<VariableDeclaration>())
		res.iterators.push_back(NodeStructs::VariableDeclaration{ getStruct(it.get<Typename>()), it.get<Word>().value });
	for (const auto& statement : statement.get<ColonIndentCodeBlock>().get<Indent<Star<Statement>>>().get<Statement>())
		res.statements.push_back(getStatementStruct(statement));
	return res;
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
	std::vector<NodeStructs::Expression> res;
	for (const auto& expr : vec)
		res.push_back(getExpressionStruct(expr));
	return res;
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
