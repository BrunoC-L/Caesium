#pragma once

template <typename T> class FileNode;
template <typename T> class ClassNode;
template <typename T> class OPTNode;
template <typename T> class StarNode;
template <typename T> class PlusNode;
template <typename T> class OrNode;
template <typename T> class TokenNode;
template <typename T> class UntilTokenNode;

template <typename T> class ClassElementNode;
template <typename T> class ClassMemberNode;
template <typename T> class MemberVariableNode;
template <typename T> class MethodNode;
template <typename T> class ClassMemberQualifiers;
template <typename T> class ConstructorNode;

template <typename T> class ExpressionNode;
template <typename T> class AssignmentExpressionNode;
template <typename T> class ConditionalExpressionNode;
template <typename T> class OrExpressionNode;
template <typename T> class AndExpressionNode;
template <typename T> class BitOrExpressionNode;
template <typename T> class BitXorExpressionNode;
template <typename T> class BitAndExpressionNode;
template <typename T> class EqualityExpressionNode;
template <typename T> class CompareExpressionNode;
template <typename T> class BitShiftExpressionNode;
template <typename T> class AdditiveExpressionNode;
template <typename T> class MultiplicativeExpressionNode;
template <typename T> class UnaryExpressionNode;
template <typename T> class PostfixExpressionNode;
template <typename T> class ParenExpressionNode;

template <typename T> class ClassInheritanceNode;
template <typename T> class MultipleInheritanceNode;
template <typename T> class SingleInheritanceNode;
template <typename T> class MacroNode;
template <typename T> class UsingNode;

template <typename T> class StatementNode;
template <typename T> class ElseStatementNode;
template <typename T> class IfStatementNode;
template <typename T> class ForStatementNode;

template <typename T> class TypenameNode;
template <typename T> class NSTypenameNode;
template <typename T> class TypenameListNode;
template <typename T> class TemplateTypenameNode;
template <typename T> class ParenthesisTypenameNode;
template <typename T> class PointerTypenameNode;
template <typename T> class PPPQualifierNode;

template <typename T> class ArgumentsSignatureNode;
template <typename T> class InnerArgumentsNode;
template <typename T> class ParenArgumentsNode;
template <typename T> class BracketArgumentsNode;
template <typename T> class BraceArgumentsNode;
template <typename T> class CodeBlockNode;

template <typename T>
class NodeVisitor {
public:
	virtual T visit(FileNode<T>* node) = 0;
	virtual T visit(ClassNode<T>* node) = 0;
	virtual T visit(OPTNode<T>* node) = 0;
	virtual T visit(StarNode<T>* node) = 0;
	virtual T visit(PlusNode<T>* node) = 0;
	virtual T visit(OrNode<T>* node) = 0;
	virtual T visit(TokenNode<T>* node) = 0;
	virtual T visit(UntilTokenNode<T>* node) = 0;

	virtual T visit(ClassElementNode<T>* node) = 0;
	virtual T visit(ClassMemberNode<T>* node) = 0;
	virtual T visit(MemberVariableNode<T>* node) = 0;
	virtual T visit(MethodNode<T>* node) = 0;
	virtual T visit(ClassMemberQualifiers<T>* node) = 0;
	virtual T visit(ConstructorNode<T>* node) = 0;

    virtual T visit(ExpressionNode<T>* node) = 0;
    virtual T visit(AssignmentExpressionNode<T>* node) = 0;
    virtual T visit(ConditionalExpressionNode<T>* node) = 0;
    virtual T visit(OrExpressionNode<T>* node) = 0;
    virtual T visit(AndExpressionNode<T>* node) = 0;
    virtual T visit(BitOrExpressionNode<T>* node) = 0;
    virtual T visit(BitXorExpressionNode<T>* node) = 0;
    virtual T visit(BitAndExpressionNode<T>* node) = 0;
    virtual T visit(EqualityExpressionNode<T>* node) = 0;
    virtual T visit(CompareExpressionNode<T>* node) = 0;
    virtual T visit(BitShiftExpressionNode<T>* node) = 0;
    virtual T visit(AdditiveExpressionNode<T>* node) = 0;
    virtual T visit(MultiplicativeExpressionNode<T>* node) = 0;
    virtual T visit(UnaryExpressionNode<T>* node) = 0;
    virtual T visit(PostfixExpressionNode<T>* node) = 0;
	virtual T visit(ParenExpressionNode<T>* node) = 0;

	virtual T visit(ClassInheritanceNode<T>* node) = 0;
	virtual T visit(MultipleInheritanceNode<T>* node) = 0;
	virtual T visit(SingleInheritanceNode<T>* node) = 0;
	virtual T visit(MacroNode<T>* node) = 0;
	virtual T visit(UsingNode<T>* node) = 0;

	virtual T visit(StatementNode<T>* node) = 0;
	virtual T visit(ElseStatementNode<T>* node) = 0;
	virtual T visit(IfStatementNode<T>* node) = 0;
	virtual T visit(ForStatementNode<T>* node) = 0;

	virtual T visit(TypenameNode<T>* node) = 0;
	virtual T visit(NSTypenameNode<T>* node) = 0;
	virtual T visit(TypenameListNode<T>* node) = 0;
	virtual T visit(TemplateTypenameNode<T>* node) = 0;
	virtual T visit(ParenthesisTypenameNode<T>* node) = 0;
	virtual T visit(PointerTypenameNode<T>* node) = 0;
	virtual T visit(PPPQualifierNode<T>* node) = 0;

	virtual T visit(ArgumentsSignatureNode<T>* node) = 0;
	virtual T visit(InnerArgumentsNode<T>* node) = 0;
	virtual T visit(ParenArgumentsNode<T>* node) = 0;
	virtual T visit(BracketArgumentsNode<T>* node) = 0;
	virtual T visit(BraceArgumentsNode<T>* node) = 0;
	virtual T visit(CodeBlockNode<T>* node) = 0;
};
