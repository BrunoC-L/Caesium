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

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::ParenExpression& statement);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::PostfixExpression& statement);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::UnaryExpression& statement);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::MultiplicativeExpression& statement);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::AdditiveExpression& statement);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::CompareExpression& statement);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::EqualityExpression& statement);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::AndExpression& statement);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::OrExpression& statement);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::ConditionalExpression& statement);

NodeStructs::Expression getExpressionStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::ExpressionStatement& statement);

NodeStructs::Expression getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::ExpressionStatement& statement);

NodeStructs::VariableDeclarationStatement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::VariableDeclarationStatement& statement);

std::vector<NodeStructs::Statement> getStatements(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::ColonIndentCodeBlock& code);

NodeStructs::BlockStatement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::BlockStatement& statement);

NodeStructs::IfStatement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::IfStatement& statement);

NodeStructs::ForStatement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::ForStatement& statement);

NodeStructs::IForStatement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::IForStatement&);

NodeStructs::WhileStatement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::WhileStatement& statement);

NodeStructs::BreakStatement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::BreakStatement& statement);

std::vector<NodeStructs::Expression> getExpressions(const std::string& file_name, const std::vector<TokenValue>& vec, const std::vector<grammar::Expression>& expressions);

NodeStructs::ReturnStatement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::ReturnStatement& statement);

NodeStructs::MatchStatement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::MatchStatement& statement);

NodeStructs::Statement getStatementStruct(const std::string& file_name, const std::vector<TokenValue>& vec, const grammar::Statement<grammar::function_context>& statement);

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
