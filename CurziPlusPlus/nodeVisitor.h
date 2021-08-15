#pragma once

class FileNode;
class ClassNode;
class OPTNode;
class StarNode;
class PlusNode;
class OrNode;
class TokenNode;
class UntilTokenNode;

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
class SingleInheritanceNode;
class MacroNode;
class UsingNode;

class StatementNode;
class ExpressionStatementNode;
class ElseStatementNode;
class IfStatementNode;
class ForStatementNode;

class TypenameNode;
class NSTypenameNode;
class TypenameListNode;
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
class BracedCodeBlockNode;

class NodeVisitor {
public:
	virtual void visit(FileNode* node) = 0;
	virtual void visit(ClassNode* node) = 0;
	virtual void visit(OPTNode* node) = 0;
	virtual void visit(StarNode* node) = 0;
	virtual void visit(PlusNode* node) = 0;
	virtual void visit(OrNode* node) = 0;
	virtual void visit(TokenNode* node) = 0;
	virtual void visit(UntilTokenNode* node) = 0;

	virtual void visit(ClassElementNode* node) = 0;
	virtual void visit(ClassMemberNode* node) = 0;
	virtual void visit(MemberVariableNode* node) = 0;
	virtual void visit(MethodNode* node) = 0;
	virtual void visit(ClassMemberQualifiers* node) = 0;
	virtual void visit(ConstructorNode* node) = 0;

    virtual void visit(ExpressionNode* node) = 0;
    virtual void visit(AssignmentExpressionNode* node) = 0;
    virtual void visit(ConditionalExpressionNode* node) = 0;
    virtual void visit(OrExpressionNode* node) = 0;
    virtual void visit(AndExpressionNode* node) = 0;
    virtual void visit(BitOrExpressionNode* node) = 0;
    virtual void visit(BitXorExpressionNode* node) = 0;
    virtual void visit(BitAndExpressionNode* node) = 0;
    virtual void visit(EqualityExpressionNode* node) = 0;
    virtual void visit(CompareExpressionNode* node) = 0;
    virtual void visit(BitShiftExpressionNode* node) = 0;
    virtual void visit(AdditiveExpressionNode* node) = 0;
    virtual void visit(MultiplicativeExpressionNode* node) = 0;
    virtual void visit(UnaryExpressionNode* node) = 0;
    virtual void visit(PostfixExpressionNode* node) = 0;
	virtual void visit(ParenExpressionNode* node) = 0;

	virtual void visit(ClassInheritanceNode* node) = 0;
	virtual void visit(MultipleInheritanceNode* node) = 0;
	virtual void visit(SingleInheritanceNode* node) = 0;
	virtual void visit(MacroNode* node) = 0;
	virtual void visit(UsingNode* node) = 0;

	virtual void visit(StatementNode* node) = 0;
	virtual void visit(ExpressionStatementNode* node) = 0;
	virtual void visit(ElseStatementNode* node) = 0;
	virtual void visit(IfStatementNode* node) = 0;
	virtual void visit(ForStatementNode* node) = 0;

	virtual void visit(TypenameNode* node) = 0;
	virtual void visit(NSTypenameNode* node) = 0;
	virtual void visit(TypenameListNode* node) = 0;
	virtual void visit(TemplateTypenameNode* node) = 0;
	virtual void visit(ParenthesisTypenameNode* node) = 0;
	virtual void visit(PointerTypenameNode* node) = 0;
	virtual void visit(PPPQualifierNode* node) = 0;

	virtual void visit(ArgumentsSignatureNode* node) = 0;
	virtual void visit(InnerArgumentsNode* node) = 0;
	virtual void visit(ParenArgumentsNode* node) = 0;
	virtual void visit(BracketArgumentsNode* node) = 0;
	virtual void visit(BraceArgumentsNode* node) = 0;
	virtual void visit(CodeBlockNode* node) = 0;
	virtual void visit(BracedCodeBlockNode* node) = 0;
};
