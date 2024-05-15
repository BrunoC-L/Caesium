#pragma once
#include "node_structs.hpp"
#include "grammar.hpp"
#include "../utility/overload.hpp"

NodeStructs::Import getStruct(const grammar::Import& f);

NodeStructs::Typename getStruct(const grammar::Typename& t);

NodeStructs::Typename extend(NodeStructs::Typename&&, const grammar::NamespaceTypenameExtension&);

NodeStructs::ParameterCategory getStruct(const grammar::ParameterCategory& vc);

NodeStructs::Function getStruct(const grammar::Function& f, std::optional<NodeStructs::Typename> name_space);

NodeStructs::Template getStruct(const grammar::Template& t);

NodeStructs::Template getStruct(const grammar::Template& t, std::optional<NodeStructs::Typename> name_space);

NodeStructs::MemberVariable getStruct(const grammar::MemberVariable& f);

NodeStructs::Alias getStruct(const grammar::Alias& f, std::optional<NodeStructs::Typename> name_space);

NodeStructs::Type getStruct(const grammar::Type& cl, std::optional<NodeStructs::Typename> name_space);

NodeStructs::File getStruct(const grammar::File& f, std::string_view fileName);

NodeStructs::ArgumentCategory getStruct(const Or<Token<MOVE>, And<Token<REF>, Token<NOT>>, Token<REF>>& t);

NodeStructs::FunctionArgument getStruct(const grammar::FunctionArgument& arg);

NodeStructs::ParenArguments getStruct(const grammar::ParenArguments& args);

NodeStructs::BracketArguments getStruct(const grammar::BracketArguments& args);

NodeStructs::BraceArguments getStruct(const grammar::BraceArguments& args);

NodeStructs::TemplateArguments getStruct(const grammar::TemplateArguments& args);

NodeStructs::Expression getExpressionStruct(const grammar::BraceArguments&);

NodeStructs::Expression getExpressionStruct(const grammar::ParenExpression& statement);

NodeStructs::Expression getExpressionStruct(const grammar::PostfixExpression& statement);

NodeStructs::Expression getExpressionStruct(const grammar::UnaryExpression& statement);

NodeStructs::Expression getExpressionStruct(const grammar::MultiplicativeExpression& statement);

NodeStructs::Expression getExpressionStruct(const grammar::AdditiveExpression& statement);

NodeStructs::Expression getExpressionStruct(const grammar::CompareExpression& statement);

NodeStructs::Expression getExpressionStruct(const grammar::EqualityExpression& statement);

NodeStructs::Expression getExpressionStruct(const grammar::AndExpression& statement);

NodeStructs::Expression getExpressionStruct(const grammar::OrExpression& statement);

NodeStructs::Expression getExpressionStruct(const grammar::ConditionalExpression& statement);

NodeStructs::Expression getExpressionStruct(const grammar::ExpressionStatement& statement);

NodeStructs::Statement getStatementStruct(const grammar::Statement& statement);

NodeStructs::Expression getStatementStruct(const grammar::ExpressionStatement& statement);

NodeStructs::VariableDeclarationStatement getStatementStruct(const grammar::VariableDeclarationStatement& statement);

std::vector<NodeStructs::Statement> getStatements(const grammar::ColonIndentCodeBlock& code);

NodeStructs::BlockStatement getStatementStruct(const grammar::BlockStatement& statement);

NodeStructs::IfStatement getStatementStruct(const grammar::IfStatement& statement);

NodeStructs::ForStatement getStatementStruct(const grammar::ForStatement& statement);

NodeStructs::IForStatement getStatementStruct(const grammar::IForStatement&);

NodeStructs::WhileStatement getStatementStruct(const grammar::WhileStatement& statement);

NodeStructs::BreakStatement getStatementStruct(const grammar::BreakStatement& statement);

std::vector<NodeStructs::Expression> getExpressions(const std::vector<grammar::Expression>& vec);

NodeStructs::ReturnStatement getStatementStruct(const grammar::ReturnStatement& statement);

NodeStructs::MatchStatement getStatementStruct(const grammar::MatchStatement& statement);

NodeStructs::Statement getStatementStruct(const grammar::Statement& statement);
