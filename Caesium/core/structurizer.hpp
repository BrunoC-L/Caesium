#pragma once
#include <ranges>
#include "node_structs.hpp"
#include "grammar.hpp"
#include "../utility/overload.hpp"

NodeStructs::Import getStruct(const Import& f);

NodeStructs::Typename getStruct(const Typename& t);

NodeStructs::Typename extend(NodeStructs::Typename&&, const NamespaceTypenameExtension&);

NodeStructs::TemplatedTypename extend_tmpl(NodeStructs::Typename&& t, const std::vector<Alloc<Typename>>& templates);

NodeStructs::Typename extend(NodeStructs::Typename&& t, const TemplateTypenameExtension& tt);

NodeStructs::Typename extend(NodeStructs::Typename&& t, const UnionTypenameExtension& ut);

NodeStructs::Typename getStruct(const Typename& t);

NodeStructs::ParameterCategory getStruct(const ParameterCategory& vc);

NodeStructs::Function getStruct(const Function& f);

NodeStructs::Template getStruct(const Template& t);

NodeStructs::MemberVariable getStruct(const MemberVariable& f);

NodeStructs::Alias getStruct(const Alias& f);

NodeStructs::Type getStruct(const Type& cl);

NodeStructs::File getStruct(const File& f, std::string fileName);

NodeStructs::ArgumentCategory getStruct(const Or<Token<COPY>, Token<MOVE>, And<Token<REF>, Token<NOT>>, Token<REF>>& t);

NodeStructs::FunctionArgument getStruct(const FunctionArgument& arg);

NodeStructs::ParenArguments getStruct(const ParenArguments& args);

NodeStructs::BracketArguments getStruct(const BracketArguments& args);

NodeStructs::BraceArguments getStruct(const BraceArguments& args);

NodeStructs::TemplateArguments getStruct(const TemplateArguments& args);

NodeStructs::Expression getExpressionStruct(const ConditionalExpression& statement);

NodeStructs::Expression getExpressionStruct(const BraceArguments&);

NodeStructs::Expression getExpressionStruct(const ParenExpression& statement);

NodeStructs::Expression getExpressionStruct(const PostfixExpression& statement);

NodeStructs::Expression getExpressionStruct(const UnaryExpression& statement);

NodeStructs::Expression getExpressionStruct(const MultiplicativeExpression& statement);

NodeStructs::Expression getExpressionStruct(const AdditiveExpression& statement);

NodeStructs::Expression getExpressionStruct(const CompareExpression& statement);

NodeStructs::Expression getExpressionStruct(const EqualityExpression& statement);

NodeStructs::Expression getExpressionStruct(const AndExpression& statement);

NodeStructs::Expression getExpressionStruct(const OrExpression& statement);

NodeStructs::Expression getExpressionStruct(const ConditionalExpression& statement);

NodeStructs::Expression getExpressionStruct(const ExpressionStatement& statement);

NodeStructs::Statement getStatementStruct(const Statement& statement);

NodeStructs::Expression getStatementStruct(const ExpressionStatement& statement);

NodeStructs::VariableDeclarationStatement getStatementStruct(const VariableDeclarationStatement& statement);

std::vector<NodeStructs::Statement> getStatements(const ColonIndentCodeBlock& code);

NodeStructs::BlockStatement getStatementStruct(const BlockStatement& statement);

NodeStructs::IfStatement getStatementStruct(const IfStatement& statement);

NodeStructs::ForStatement getStatementStruct(const ForStatement& statement);

NodeStructs::IForStatement getStatementStruct(const IForStatement&);

NodeStructs::WhileStatement getStatementStruct(const WhileStatement& statement);

NodeStructs::BreakStatement getStatementStruct(const BreakStatement& statement);

std::vector<NodeStructs::Expression> getExpressions(const std::vector<Expression>& vec);

NodeStructs::ReturnStatement getStatementStruct(const ReturnStatement& statement);

NodeStructs::Statement getStatementStruct(const Statement& statement);
