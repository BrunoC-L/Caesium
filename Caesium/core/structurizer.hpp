#pragma once
#include "node_structs.hpp"
#include "helpers.hpp"
#include "parse.hpp"
#include "../utility/overload.hpp"

struct tag_expect_value_category {};
struct tag_expect_empty_category {};
struct tag_allow_value_category_or_empty {};

NodeStructs::Import getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Import& f);

NodeStructs::Typename getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Typename& t, tag_expect_value_category);
NodeStructs::Typename getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Typename& t, tag_expect_empty_category);
NodeStructs::Typename getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Typename& t, tag_allow_value_category_or_empty);

NodeStructs::WordTypenameOrExpression getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::TypenameOrExpression& t, tag_expect_value_category);
NodeStructs::WordTypenameOrExpression getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::TypenameOrExpression& t, tag_expect_empty_category);
NodeStructs::WordTypenameOrExpression getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::TypenameOrExpression& t, tag_allow_value_category_or_empty);

NodeStructs::Typename extend(NodeStructs::Typename&&, const grammar::NamespaceTypenameExtension&);

NodeStructs::ParameterCategory getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const Opt<grammar::ParameterCategory>& vc, tag_expect_value_category);
NodeStructs::ParameterCategory getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const Opt<grammar::ParameterCategory>& vc, tag_expect_empty_category);
NodeStructs::ParameterCategory getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const Opt<grammar::ParameterCategory>& vc, tag_allow_value_category_or_empty);

NodeStructs::Function getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Function& f, std::optional<NodeStructs::Typename> name_space);

NodeStructs::Template getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Template& t);

NodeStructs::Template getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Template& t, std::optional<NodeStructs::Typename> name_space);

NodeStructs::MemberVariable getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::MemberVariable& f);

NodeStructs::Alias getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Alias& f, std::optional<NodeStructs::Typename> name_space);

NodeStructs::Type getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Type& cl, std::optional<NodeStructs::Typename> name_space);

NodeStructs::File getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::File& f);

NodeStructs::ArgumentCategory getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const Or<Token<MOVE>, And<Token<REF>, Token<NOT>>, Token<REF>>& t);

NodeStructs::FunctionArgument getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::FunctionArgument& arg);

NodeStructs::ParenArguments getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::ParenArguments& args);

NodeStructs::BracketArguments getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::BracketArguments& args);

NodeStructs::BraceArguments getStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::BraceArguments& args);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::BraceArguments&);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::ParenExpression& expr);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::PostfixExpression& expr);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::UnaryExpression& expr);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::MultiplicativeExpression& expr);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::AdditiveExpression& expr);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::CompareExpression& expr);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::EqualityExpression& expr);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::AndExpression& expr);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::OrExpression& expr);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::ConditionalExpression& expr);

std::vector<NodeStructs::Expression> getExpressions(const std::string& file_name, const std::vector<TokenValue>& vec, const std::vector<grammar::Expression>& expressions);

std::string accumulate_content(const std::vector<TokenValue>& vec, const unsigned& beg, const unsigned& end);

rule_info rule_info_from_rule(const std::string& file_name, const std::vector<TokenValue>& vec, const auto& rule) {
	auto x = rule_info{
		.file_name = file_name,
		.content = accumulate_content(vec, rule.beg_offset, rule.end_offset),
		.beg = cursor_info{ 0, rule.beg_offset },
		.end = cursor_info{ 0, rule.end_offset }
	};
	return x;
}

NodeStructs::Statement<grammar::function_context> get_base_statement_struct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Statement<grammar::function_context>& statement
);

NodeStructs::Statement<grammar::type_context> get_base_statement_struct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Statement<grammar::type_context>& statement
);

NodeStructs::Statement<grammar::top_level_context> get_base_statement_struct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Statement<grammar::top_level_context>& statement
);

template <typename context>
NodeStructs::Statement<context> getStatementStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const Expect<grammar::Statement<context>>& statement
) {
	const grammar::Statement<context>& st = statement;
	return get_base_statement_struct(file_name, vec, st);
}

template <typename context>
NodeStructs::Expression getStatementStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::ExpressionStatement<context>& statement
) {
	return getExpressionStruct(file_name, vec, statement);
}

template <typename context>
NodeStructs::VariableDeclarationStatement<context> getStatementStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::VariableDeclarationStatement<context>& statement
) {
	return {
		getStruct(file_name, vec, statement.get<grammar::Typename>(), tag_allow_value_category_or_empty{}),
		statement.get<grammar::Word>().value, getExpressionStruct(file_name, vec, statement.get<grammar::Expression>())
	};
}

template <typename context>
std::vector<NodeStructs::Statement<context>> getStatements(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::ColonIndentCodeBlock<context>& code
) {
	return code.get<Indent<Star<Or<Token<NEWLINE>, Expect<grammar::Statement<context>>>>>>().get<Or<Token<NEWLINE>, Expect<grammar::Statement<context>>>>()
		| std::views::transform(
			[&](const Or<Token<NEWLINE>, Expect<grammar::Statement<context>>>& e)
			-> const std::variant<Token<NEWLINE>, Expect<grammar::Statement<context>>>&{
				return e.value();
			})
		| std::views::filter([&](auto&& e) { return std::holds_alternative<Expect<grammar::Statement<context>>>(e); })
				| std::views::transform([&](auto&& e) { return std::get<Expect<grammar::Statement<context>>>(e); })
				| std::views::transform([&](const Expect<grammar::Statement<context>>& e) { return getStatementStruct<context>(file_name, vec, e); })
			| to_vec();
}

template <typename context>
NodeStructs::BlockStatement<context> getStatementStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::BlockStatement<context>& statement
) {
	// wtf even is this right now it doesn't make sense
	throw;
	//return { getStruct(statement.get<grammar::Typename>()) };
}

template <typename context>
NodeStructs::IfStatement<context> getStatementStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::IfStatement<context>& statement
) {
	using T = Variant<NonCopyableBox<NodeStructs::IfStatement<context>>, std::vector<NodeStructs::Statement<context>>>;
	return {
		getExpressionStruct(file_name, vec, statement.get<grammar::Expression>()),
		getStatements(file_name, vec, statement.get<grammar::ColonIndentCodeBlock<context>>()),
		statement.get<Opt<Alloc<grammar::ElseStatement<context>>>>().node.transform([&](const auto& e) -> T {
			return std::visit(
				overload(overload_default_error,
					[&](const Alloc<grammar::IfStatement<context>>& e) -> T {
						return getStatementStruct(file_name, vec, e.get());
					},
					[&](const grammar::ColonIndentCodeBlock<context>& e) -> T {
						return getStatements(file_name, vec, e);
					}
				),
				e.get()
				.get<Or<Alloc<grammar::IfStatement<context>>, grammar::ColonIndentCodeBlock<context>>>()
				.value()
			);
		})
	};
}

template <typename context>
NodeStructs::ForStatement<context> getForStatementStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const auto& statement,
	context
) {
	return {
		.collection = getExpressionStruct(file_name, vec, statement.get<grammar::Expression>()),
		.iterators = statement.get<CommaPlus<Or<grammar::VariableDeclaration<context>, grammar::Word>>>().get<Or<grammar::VariableDeclaration<context>, grammar::Word>>()
			| std::views::transform([&](const Or<grammar::VariableDeclaration<context>, grammar::Word>& or_node) {
				return std::visit(overload(
					[&](const grammar::Word& e) -> Variant<NodeStructs::VariableDeclaration, std::string> {
						return { e.value };
					},
					[&](const grammar::VariableDeclaration<context>& e) -> Variant<NodeStructs::VariableDeclaration, std::string> {
						return NodeStructs::VariableDeclaration{
							getStruct(file_name, vec, e.get<grammar::Typename>(), tag_allow_value_category_or_empty{}),
							e.get<grammar::Word>().value
						};
					}
				), or_node.value());
			})
			| to_vec(),
		.statements = getStatements(file_name, vec, statement.get<grammar::ColonIndentCodeBlock<context>>())
	};
}

template <typename context>
NodeStructs::ForStatement<context> getStatementStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::ForStatement<context>& statement
) {
	return getForStatementStruct(file_name, vec, statement, context{});
}

template <typename context>
NodeStructs::IForStatement<context> getStatementStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::IForStatement<context>& statement
) {
	return {
		statement.get<grammar::Word>().value,
		getForStatementStruct(file_name, vec, statement, context{})
	};
}

template <typename context>
NodeStructs::WhileStatement<context> getStatementStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::WhileStatement<context>& statement
) {
	return {
		getExpressionStruct(file_name, vec, statement.get<grammar::Expression>()),
		getStatements(file_name, vec, statement.get<grammar::ColonIndentCodeBlock<context>>())
	};
}

template <typename context>
NodeStructs::MatchCase<context> getCase(const std::string& file_name, const std::vector<TokenValue>& vec, const auto& typenames, const auto& statements) {
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
		| std::views::filter([&](auto&& e) { return std::holds_alternative<Expect<grammar::Statement<context>>>(e); })
		| std::views::transform([&](auto&& e) { return std::get<Expect<grammar::Statement<context>>>(e); })
		| std::views::transform([&](auto&& e) { return getStatementStruct(file_name, vec, e); })
		| to_vec()
	};
}

template <typename context>
std::vector<NodeStructs::MatchCase<context>> getCases(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const Indent<Plus<And<IndentToken, CommaPlus<grammar::VariableDeclaration<context>>, grammar::ColonIndentCodeBlock<context>>>>& cases
) {
	std::vector<NodeStructs::MatchCase<context>> res;
	res.reserve(cases.nodes.size());
	for (const auto& and_node : cases.nodes) {
		const auto& [_, typenames, statements] = and_node.value;
		res.push_back(getCase<context>(
			file_name,
			vec,
			typenames.nodes,
			statements.get<
				Indent<Star<Or<
					Token<NEWLINE>,
					Expect<grammar::Statement<context>>
				>>>
			>().get_view<Or<Token<NEWLINE>, Expect<grammar::Statement<context>>>>()
		));
	}
	return res;
}

template <typename context>
NodeStructs::MatchStatement<context> getStatementStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::MatchStatement<context>& statement
) {
	return {
		statement.get<CommaPlus<grammar::Expression>>().get_view<grammar::Expression>()
			| std::views::transform([&](auto&& e) { return getExpressionStruct(file_name, vec, e); })
			| to_vec(),
		getCases(file_name, vec, statement.get<Indent<Plus<And<
			IndentToken,
			CommaPlus<grammar::VariableDeclaration<context>>,
			grammar::ColonIndentCodeBlock<context>
		>>>>())
	};
}

template <typename context>
NodeStructs::BreakStatement<context> getStatementStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::BreakStatement<context>& statement
) {
	return {
		statement.get<Opt<And<Token<IF>, grammar::Expression>>>().has_value()
			? getExpressionStruct(file_name, vec, statement.get<Opt<And<Token<IF>, grammar::Expression>>>().value().get<grammar::Expression>())
			: std::optional<NodeStructs::Expression>{}
	};
}

std::vector<NodeStructs::Expression> getExpressions(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const std::vector<grammar::Expression>& expressions
);

template <typename context>
NodeStructs::ReturnStatement<context> getStatementStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::ReturnStatement<context>& statement
) {
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

template <typename context>
NodeStructs::SwitchStatement<context> getStatementStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::SwitchStatement<context>& statement
) {
	return NodeStructs::SwitchStatement<context>{
		.expr = getExpressionStruct(file_name, vec, statement.get<grammar::Expression>()),
		.cases = statement.get<Indent<Star<And<
			IndentToken,
			grammar::Expression,
			grammar::ColonIndentCodeBlock<context>
		>>>>().get<And<IndentToken, grammar::Expression, grammar::ColonIndentCodeBlock<context>>>()
		| std::views::transform([&](const And<IndentToken, grammar::Expression, grammar::ColonIndentCodeBlock<context>>& switch_case) {
			return NodeStructs::SwitchCase<context>{
				getExpressionStruct(file_name, vec, switch_case.get<grammar::Expression>()),
				getStatements(file_name, vec, switch_case.get<grammar::ColonIndentCodeBlock<context>>())
			};
		})
		| to_vec()
	};
}

template <typename context>
NodeStructs::Assignment<context> getStatementStruct(
	const std::string& file_name,
	const std::vector<TokenValue>& vec,
	const grammar::Assignment<context>& statement
) {
	return { getExpressionStruct(file_name, vec, statement.get<grammar::Expression, 0>()), getExpressionStruct(file_name, vec, statement.get<grammar::Expression, 1>()) };
}
