#pragma once

class Node;

class OPTNode;
class StarNode;
class PlusNode;
class CommaStarNode;
class CommaPlusNode;
class AndNode;
class OrNode;
class TokenNode_;
class WordTokenNode;
class NumberTokenNode;
class UntilTokenNode;
class IndentNode;

class FileNode;
class ClassNode;
class FunctionNode;

class ClassElementNode;
class ClassMemberNode;
class MemberVariableNode;
class MethodNode;
class ClassMemberQualifiers;
class ConstructorNode;

class ExpressionNode;
class AssignmentExpressionNode;
class ConditionalExpressionNode;
class OrExpressionNode;
class AndExpressionNode;
class BitOrExpressionNode;
class BitXorExpressionNode;
class BitAndExpressionNode;
class EqualityExpressionNode;
class CompareExpressionNode;
class BitShiftExpressionNode;
class AdditiveExpressionNode;
class MultiplicativeExpressionNode;
class UnaryExpressionNode;
class PostfixExpressionNode;
class ParenExpressionNode;

class ClassInheritanceNode;
class MultipleInheritanceNode;
class MacroNode;
class UsingNode;

class StatementNode;
class ExpressionStatementNode;
class ElseStatementNode;
class IfStatementNode;
class ForStatementNode;
class IForStatementNode;
class WhileStatementNode;
class ReturnStatementNode;
class BreakStatementNode;

class TypenameNode;
class NSTypenameNode;
class TypenameListNode;
class TypenameListNodeEndingWithRShift;
class TemplateTypenameNode;
class ParenthesisTypenameNode;
class PointerTypenameNode;
class PPPQualifierNode;

class ArgumentsSignatureNode;
class InnerArgumentsNode;
class ParenArgumentsNode;
class BracketArgumentsNode;
class BraceArgumentsNode;
class CodeBlockNode;
class ColonIndentCodeBlockNode;

class NodeVisitor {
public:
	virtual void default_behavior(Node* node);
	virtual void visit(Node* node);
	virtual void visit(OPTNode* node);
	virtual void visit(StarNode* node);
	virtual void visit(PlusNode* node);
	virtual void visit(CommaStarNode* node);
	virtual void visit(CommaPlusNode* node);
	virtual void visit(OrNode* node);
	virtual void visit(AndNode* node);
	virtual void visit(TokenNode_* node);
	virtual void visit(WordTokenNode* node);
	virtual void visit(NumberTokenNode* node);
	virtual void visit(UntilTokenNode* node);
	virtual void visit(IndentNode* node);

	virtual void visit(FileNode* node);
	virtual void visit(ClassNode* node);
	virtual void visit(FunctionNode* node);

	virtual void visit(ClassElementNode* node);
	virtual void visit(ClassMemberNode* node);
	virtual void visit(MemberVariableNode* node);
	virtual void visit(MethodNode* node);
	virtual void visit(ClassMemberQualifiers* node);
	virtual void visit(ConstructorNode* node);

	virtual void visit(ExpressionNode* node);
	virtual void visit(AssignmentExpressionNode* node);
	virtual void visit(ConditionalExpressionNode* node);
	virtual void visit(OrExpressionNode* node);
	virtual void visit(AndExpressionNode* node);
	virtual void visit(BitOrExpressionNode* node);
	virtual void visit(BitXorExpressionNode* node);
	virtual void visit(BitAndExpressionNode* node);
	virtual void visit(EqualityExpressionNode* node);
	virtual void visit(CompareExpressionNode* node);
	virtual void visit(BitShiftExpressionNode* node);
	virtual void visit(AdditiveExpressionNode* node);
	virtual void visit(MultiplicativeExpressionNode* node);
	virtual void visit(UnaryExpressionNode* node);
	virtual void visit(PostfixExpressionNode* node);
	virtual void visit(ParenExpressionNode* node);

	virtual void visit(ClassInheritanceNode* node);
	virtual void visit(MultipleInheritanceNode* node);
	virtual void visit(MacroNode* node);
	virtual void visit(UsingNode* node);

	virtual void visit(StatementNode* node);
	virtual void visit(ExpressionStatementNode* node);
	virtual void visit(ElseStatementNode* node);
	virtual void visit(IfStatementNode* node);
	virtual void visit(ForStatementNode* node);
	virtual void visit(IForStatementNode* node);
	virtual void visit(WhileStatementNode* node);
	virtual void visit(ReturnStatementNode* node);
	virtual void visit(BreakStatementNode* node);

	virtual void visit(TypenameNode* node);
	virtual void visit(NSTypenameNode* node);
	virtual void visit(TypenameListNode* node);
	virtual void visit(TypenameListNodeEndingWithRShift* node);
	virtual void visit(TemplateTypenameNode* node);
	virtual void visit(ParenthesisTypenameNode* node);
	virtual void visit(PointerTypenameNode* node);
	virtual void visit(PPPQualifierNode* node);

	virtual void visit(ArgumentsSignatureNode* node);
	virtual void visit(InnerArgumentsNode* node);
	virtual void visit(ParenArgumentsNode* node);
	virtual void visit(BracketArgumentsNode* node);
	virtual void visit(BraceArgumentsNode* node);
	virtual void visit(CodeBlockNode* node);
	virtual void visit(ColonIndentCodeBlockNode* node);
};
