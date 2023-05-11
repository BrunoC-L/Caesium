#pragma once
#include "node_structs.h"
#include "grammar.h"

NodeStructs::AssignmentExpression getExpressionStruct(const AssignmentExpression& statement);
std::vector<NodeStructs::TemplateDeclaration> getTemplatesFromTemplateTypenameDeclaration(const TemplateTypenameDeclaration& templateTypename);
NodeStructs::Statement getStatementStruct(const Statement& statement);

NodeStructs::Import getStruct(const Import& f) {
	NodeStructs::Import res;
	res.imported = std::visit(
		overload(
			[](const Word& word) {
				return "\"" + word.value + ".h\"";
			},
			[](const String& string) {
				return string.value;
		}), f.get<Or<Word, String>>().value());
	return res;
}

NodeStructs::TemplateDeclaration getTemplateDeclaration(const TemplateTypenameDeclaration& templateTypename) {
	NodeStructs::TemplateDeclaration res;
	res.name = templateTypename.get<Word>().value;
	res.parameters = getTemplatesFromTemplateTypenameDeclaration(templateTypename);
	return res;
}

NodeStructs::TemplateDeclaration getTemplateDeclaration(Token<TYPE>) {
	NodeStructs::TemplateDeclaration res;
	return res;
}

std::vector<NodeStructs::TemplateDeclaration> getTemplatesFromTemplateTypenameDeclaration(const TemplateTypenameDeclaration& templateTypename) {
	std::vector<NodeStructs::TemplateDeclaration> res;
	const TemplateDeclaration& tmpl = templateTypename.get<Alloc<TemplateDeclaration>>().get();
	const auto& l = tmpl.get<CommaPlus<Or<TemplateTypenameDeclaration, Token<TYPE>>>>();
	for (const auto& t : l.get<Or<TemplateTypenameDeclaration, Token<TYPE>>>())
		std::visit([&res](const auto& t) {
			res.push_back(getTemplateDeclaration(t));
		}, t.value());
	return res;
}

void extend(std::vector<NodeStructs::TypenameExtension>& vec, const NSTypename& nst) {
	const Typename& t = nst.get<Alloc<Typename>>().get();
	vec.push_back(NodeStructs::NSTypeExtension{ t.get<Word>().value });
	if (t.get<Opt<Or<NSTypename, TemplateTypename>>>().has_value())
		std::visit([&vec](const auto& t) {
			extend(vec, t);
		}, t.get<Opt<Or<NSTypename, TemplateTypename>>>().value().value());
}

NodeStructs::Typename getStruct(const Typename& t);

void extend(std::vector<NodeStructs::TypenameExtension>& vec, const TemplateTypename& tt) {
	NodeStructs::TemplateTypeExtension res;
	for (const Typename& t : tt.get<CommaStar<Alloc<Typename>>>().get<Typename>())
		res.templateTypes.push_back(getStruct(t));
	vec.push_back(std::move(res));
	if (tt.get<Opt<NSTypename>>().has_value())
		extend(vec, tt.get<Opt<NSTypename>>().value());
}

/*
expr    -> Typename                                              -> NodeStructs::Typename
T::T<T> -> typename{T, ns{typename{T, template{[typename{T}]}}}} -> typename{T, [ns{T}, template{T}]}
T<T>::T -> typename{T, template{[typename{T}], ns{typename{T}}}} -> typename{T, [template{T}, ns{T}]}
T<T::T> -> typename{T, template{[typename{T, ns{typename{T}}}]}} -> typename{T, [template{T, [ns{T}]}]}
*/
NodeStructs::Typename getStruct(const Typename& t) {
	NodeStructs::Typename res;
	res.type = t.get<Word>().value;
	if (t.get<Opt<Or<NSTypename, TemplateTypename>>>().has_value())
		std::visit([&res](const auto& t) {
			extend(res.extensions, t);
		}, t.get<Opt<Or<NSTypename, TemplateTypename>>>().value().value());
	return res;
}

NodeStructs::Function getStruct(const Function& f) {
	NodeStructs::Function res;
	res.name = f.get<Word>().value;
	res.returnType = getStruct(f.get<Typename>());
	for (const auto& statement : f.get<ColonIndentCodeBlock>().get<Indent<Star<Statement>>>().get<Statement>())
		res.statements.push_back(getStatementStruct(statement));
	for (const auto& type_and_name : f.get<ArgumentsSignature>().get<And<Typename, Word>>())
		res.parameters.push_back({ getStruct(type_and_name.get<Typename>()), type_and_name.get<Word>().value });
	return res;
}

NodeStructs::Constructor getStruct(const Constructor& f) {
	NodeStructs::Constructor res;
	for (const auto& statement : f.get<ColonIndentCodeBlock>().get<Indent<Star<Statement>>>().get<Statement>())
		res.statements.push_back(getStatementStruct(statement));
	for (const auto& type_and_name : f.get<ArgumentsSignature>().get<And<Typename, Word>>())
		res.parameters.push_back({ getStruct(type_and_name.get<Typename>()), type_and_name.get<Word>().value });
	return res;
}

NodeStructs::MemberVariable getStruct(const MemberVariable& f) {
	NodeStructs::MemberVariable res;
	res.type = getStruct(f.get<Typename>());
	res.name = f.get<Word>().value;
	return res;
}

NodeStructs::Alias getStruct(const Alias& f) {
	NodeStructs::Alias res;
	res.aliasFrom = NodeStructs::Typename{ f.get<Word>().value, {} };
	res.aliasTo = getStruct(f.get<Typename>());
	return res;
}

NodeStructs::Class getStruct(const Type& cl) {
	NodeStructs::Class computedClass{ cl.get<Word>().value };
	for (const ClassElement& ce : cl.get<Indent<Star<And<IndentToken, ClassElement>>>>().get<ClassElement>())
		std::visit([&computedClass](const auto& e) {
			computedClass.get<decltype(getStruct(e))>().push_back(getStruct(e));
		}, ce.value());
	if (cl.get<Opt<ClassInheritance>>().has_value())
		for (const Typename& t : cl
								.get<Opt<ClassInheritance>>().value()
								.get<CommaPlus<Typename>>()
								.get<Typename>())
			computedClass.inheritances.push_back(getStruct(t));
	return computedClass;
}

NodeStructs::File getStruct(const File& f) {
	NodeStructs::File res;
	for (const Import& import : f.get<Star<Import>>().get<Import>())
		res.imports.push_back(getStruct(import));
	for (const Type& cl : f.get<Star<Or<Type, Function>>>().get<Type>())
		res.classes.push_back(getStruct(cl));
	for (const Function& fun : f.get<Star<Or<Type, Function>>>().get<Function>())
		res.functions.push_back(getStruct(fun));
	return res;
}

NodeStructs::ParenArguments getStruct(const ParenArguments& args) {
	NodeStructs::ParenArguments res{};
	for (const auto& arg : args.get<CommaStar<Expression>>().get<Expression>())
		res.args.push_back(getExpressionStruct(arg));
	return res;
}

NodeStructs::BracketArguments getStruct(const BracketArguments& args) {
	NodeStructs::BracketArguments res{};
	for (const auto& arg : args.get<CommaStar<Expression>>().get<Expression>())
		res.args.push_back(getExpressionStruct(arg));
	return res;
}



/*
Expressions
*/

NodeStructs::ParenExpression getExpressionStruct(const ParenExpression& statement) {
	return std::visit(overload(
			[](const And<
				Token<PARENOPEN>,
				Alloc<Expression>,
				Token<PARENCLOSE>
			>& e) {
				return NodeStructs::ParenExpression{
					std::make_unique<NodeStructs::AssignmentExpression>(getExpressionStruct(e.get<Alloc<Expression>>().get()))
				};
			},
			[](const Typename& e) {
				return NodeStructs::ParenExpression{ getStruct(e)};
			},
			[](const Token<NUMBER>& e) {
				return NodeStructs::ParenExpression{ e };
			}
		),
		statement.value()
	);
}

NodeStructs::PostfixExpression getExpressionStruct(const PostfixExpression& statement) {
	auto res = NodeStructs::PostfixExpression{ getExpressionStruct(statement.get<ParenExpression>()), {} };
	using opts = Or<
		And<
			Token<DOT>,
			Word
		>,
		ParenArguments,
		BracketArguments,
		Token<PLUSPLUS>,
		Token<MINUSMINUS>
	>;
	using nodestruct_opts = NodeStructs::PostfixExpression::op_types;
	for (const auto& n : statement.get<Star<opts>>().get<opts>()) {
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
	return res;
}

NodeStructs::UnaryExpression getExpressionStruct(const UnaryExpression& statement) {
	using op_types = NodeStructs::UnaryExpression::op_types;
	using op_and_unaryexpr = NodeStructs::UnaryExpression::op_and_unaryexpr;
	return std::visit(
		overload(
			[](const PostfixExpression& expr) {
				return NodeStructs::UnaryExpression{ getExpressionStruct(expr) };
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
			>& op_and_unary) {
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
						return NodeStructs::UnaryExpression{
							op_and_unaryexpr {
								token,
								std::make_unique<NodeStructs::UnaryExpression>(getExpressionStruct(op_and_unary.get<Alloc<UnaryExpression>>().get()))
							}
						};
					},
					[&](const And< // type cast operator
						Token<PARENOPEN>,
						Typename,
						Token<PARENCLOSE>
					>& g) {
						return NodeStructs::UnaryExpression{
							op_and_unaryexpr {
								getStruct(g.get<Typename>()),
								std::make_unique<NodeStructs::UnaryExpression>(getExpressionStruct(op_and_unary.get<Alloc<UnaryExpression>>().get()))
							}
						};
					}
				), op.value());
			}
		),
		statement.value()
	);
}

NodeStructs::MultiplicativeExpression getExpressionStruct(const MultiplicativeExpression& statement) {
	auto res = NodeStructs::MultiplicativeExpression{ getExpressionStruct(statement.get<UnaryExpression>()), {} };
	using operators = Or<Token<ASTERISK>, Token<SLASH>, Token<PERCENT>>;
	for (const auto& op_exp : statement.get<Star<And<operators, UnaryExpression>>>().get<And<operators, UnaryExpression>>())
		res.muls.push_back({
			operators::variant_t{ op_exp.get<operators>().value() },
			getExpressionStruct(op_exp.get<UnaryExpression>())
		});
	return res;
}

NodeStructs::AdditiveExpression getExpressionStruct(const AdditiveExpression& statement) {
	auto res = NodeStructs::AdditiveExpression{ getExpressionStruct(statement.get<MultiplicativeExpression>()), {}};
	using operators = Or<Token<PLUS>, Token<DASH>>;
	for (const auto& op_exp : statement.get<Star<And<operators, MultiplicativeExpression>>>().get<And<operators, MultiplicativeExpression>>())
		res.adds.push_back({
			operators::variant_t{ op_exp.get<operators>().value() },
			getExpressionStruct(op_exp.get<MultiplicativeExpression>())
			});
	return res;
}

NodeStructs::CompareExpression getExpressionStruct(const CompareExpression& statement) {
	auto res = NodeStructs::CompareExpression{ getExpressionStruct(statement.get<AdditiveExpression>()), {} };
	using operators = Or<Token<LT>, Token<LTE>, Token<GT>, Token<GTE>>;
	for (const auto& op_exp : statement.get<Star<And<operators, AdditiveExpression>>>().get<And<operators, AdditiveExpression>>())
		res.comparisons.push_back({
			operators::variant_t{ op_exp.get<operators>().value() },
			getExpressionStruct(op_exp.get<AdditiveExpression>())
			});
	return res;
}

NodeStructs::EqualityExpression getExpressionStruct(const EqualityExpression& statement) {
	NodeStructs::EqualityExpression res{ getExpressionStruct(statement.get<CompareExpression>()) };
	for (const auto& n : statement.get<Star<And<Token<EQUALEQUAL>, CompareExpression>>>().get<CompareExpression>())
		res.equals.push_back(getExpressionStruct(n));
	return res;
}

NodeStructs::AndExpression getExpressionStruct(const AndExpression& statement) {
	NodeStructs::AndExpression res{ getExpressionStruct(statement.get<EqualityExpression>()) };
	for (const auto& n : statement.get<Star<And<Token<AND>, EqualityExpression>>>().get<EqualityExpression>())
		res.ands.push_back(getExpressionStruct(n));
	return res;
}

NodeStructs::OrExpression getExpressionStruct(const OrExpression& statement) {
	NodeStructs::OrExpression res{ getExpressionStruct(statement.get<AndExpression>()) };
	for (const auto& n : statement.get<Star<And<Token<OR>, AndExpression>>>().get<AndExpression>())
		res.ors.push_back(getExpressionStruct(n));
	return res;
}

NodeStructs::ConditionalExpression getExpressionStruct(const ConditionalExpression& statement) {
	NodeStructs::ConditionalExpression res{ getExpressionStruct(statement.get<OrExpression>()) };
	auto ifElseExpr = statement.get<Opt<And<
			Token<IF>,
			OrExpression,
			Opt<And<
				Token<ELSE>,
				OrExpression
			>>
		>>>();
	if (ifElseExpr.has_value()) {
		res.ifExpr = getExpressionStruct(ifElseExpr.value().get<OrExpression>());
		auto elseExpr = ifElseExpr.value().get<Opt<And<Token<ELSE>, OrExpression>>>();
		if (elseExpr.has_value())
			res.elseExpr = getExpressionStruct(elseExpr.value().get<OrExpression>());
	}
	return res;
}

NodeStructs::AssignmentExpression getExpressionStruct(const AssignmentExpression& statement) {
	auto res = NodeStructs::AssignmentExpression{ getExpressionStruct(statement.get<ConditionalExpression>()), {} };
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
	for (const auto& op_exp : statement.get<Star<And<operators, ConditionalExpression>>>().get<And<operators, ConditionalExpression>>())
		res.assignments.push_back({
			operators::variant_t{ op_exp.get<operators>().value() },
			getExpressionStruct(op_exp.get<ConditionalExpression>())
		});
	return res;
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
	return { getStruct(statement.get<Typename>()), statement.get<Word>().value };
}

NodeStructs::IfStatement getStatementStruct(const IfStatement& statement) {
	NodeStructs::IfStatement res{ getExpressionStruct(statement.get<Expression>()) };
	for (const auto& statement : statement.get<ColonIndentCodeBlock>().get<Indent<Star<Statement>>>().get<Statement>())
		res.ifStatements.push_back(getStatementStruct(statement));
	//if (statement.get<Opt<ElseStatement>>().has_value()) {
	//	// std::variant<std::unique_ptr<IfStatement>, std::vector<Statement>>
	//	const auto& elseStatment = statement.get<Opt<Alloc<ElseStatement>>>().value().get();
	//	if (elseStatment.get<Opt<And<Token<IF>, Expression>>>().has_value()) {
	//		//res.elseExprStatements = std::make_unique<NodeStructs::IfStatement>();
	//	}
	//	else {
	//	}
	//}
	return res;
}

NodeStructs::ForStatement getStatementStruct(const ForStatement& statement) {
	NodeStructs::ForStatement res{ getExpressionStruct(statement.get<Expression>()) };
	for (const auto& it : statement.get<CommaPlus<Or<And<Typename, Word>, Word>>>().get<Word>())
		res.iterators.push_back({ it.value });
	for (const auto& it : statement.get<CommaPlus<Or<And<Typename, Word>, Word>>>().get<And<Typename, Word>>())
		res.iterators.push_back(NodeStructs::VariableDeclarationStatement{ getStruct(it.get<Typename>()), it.get<Word>().value });
	for (const auto& statement : statement.get<ColonIndentCodeBlock>().get<Indent<Star<Statement>>>().get<Statement>())
		res.statements.push_back(getStatementStruct(statement));
	return res;
}

NodeStructs::IForStatement getStatementStruct(const IForStatement& statement) {
	return {};
}

NodeStructs::WhileStatement getStatementStruct(const WhileStatement& statement) {
	return {};
}

NodeStructs::BreakStatement getStatementStruct(const BreakStatement& statement) {
	return {};
}

NodeStructs::ReturnStatement getStatementStruct(const ReturnStatement& statement) {
	return {};
}

NodeStructs::Statement getStatementStruct(const Statement& statement) {
	return NodeStructs::Statement{ std::visit(
		[](const auto& statement) -> NodeStructs::Statement {
			return { getStatementStruct(statement) };
		},
		statement.get<Alloc<Or<
			ExpressionStatement,
			VariableDeclarationStatement,
			IfStatement,
			ForStatement,
			IForStatement,
			WhileStatement,
			BreakStatement,
			ReturnStatement
		>>>().get().value()) };
}
