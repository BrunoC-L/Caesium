#pragma once
#include "nodeVisitor.h"
#include "json.h"
#include "fileNode.h";

class JSONNodeVisitor : public NodeVisitor<JSON> {
public:
	virtual JSON visit(FileNode<JSON>* node) override {
        JSON res;
        if (node->nodes[0]->nodes.size())
            res["class"] = node->nodes[0]->nodes[0]->accept(this);
        return res;
    }
    
	virtual JSON visit(ClassNode<JSON>* node) override {
        JSON res;
        auto& children = node->nodes[0]->nodes;
        res["className"] = children[1]->accept(this);
        res["inheritance"] = children[2]->accept(this);
        res["classElements"] = children[4]->accept(this);
        return res;
    }
    
	virtual JSON visit(OPTNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(StarNode<JSON>* node) override {
        JSON res("[]");
        for (const auto& node : node->nodes)
            res.push(node->accept(this));
        return res;
    }
    
	virtual JSON visit(PlusNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(OrNode<JSON>* node) override {
        if (node->nodes.size() == 0)
            return JSON();
        else
            return node->nodes[0]->accept(this);
    }
    
	virtual JSON visit(TokenNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(UntilTokenNode<JSON>* node) override {
        return JSON();
    }
    

	virtual JSON visit(ClassElementNode<JSON>* node) override {
        if (node->nodes[0]->nodes.size())
            return node->nodes[0]->nodes[0]->accept(this);
        throw 1;
    }
    
	virtual JSON visit(ClassMemberNode<JSON>* node) override {
        JSON res;
        if (node->nodes[0]->nodes.size())
            res["member"] = node->nodes[0]->nodes[0]->accept(this);
        return res;
    }
    
	virtual JSON visit(MemberVariableNode<JSON>* node) override {
        JSON res;
        auto children = node->nodes[0]->nodes;
        res["which"] = node->name;
        res["qualifiers"] = children[0]->accept(this);
        res["type"] = children[1]->accept(this);
        res["name"] = children[2]->accept(this);
        return res;
    }
    
	virtual JSON visit(MethodNode<JSON>* node) override {
        JSON res;
        const auto children = node->nodes[0]->nodes;
        res["which"] = node->name;
        res["qualifiers"] = children[0]->accept(this);
        res["type"] = children[1]->accept(this);
        res["name"] = children[2]->accept(this);
        res["arguments"] = children[4]->accept(this);
        res["code"] = children[6]->accept(this);
        return res;
    }
    
	virtual JSON visit(ClassMemberQualifiers<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(ConstructorNode<JSON>* node) override {
        JSON res;
        const auto children = node->nodes[0]->nodes;
        res["which"] = node->name;
        res["qualifiers"] = children[0]->accept(this);
        res["type"] = children[1]->accept(this);
        res["name"] = children[2]->accept(this);
        res["arguments"] = children[3]->accept(this);
        res["code"] = children[5]->accept(this);
        return res;
    }
    

	virtual JSON visit(ExpressionNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(AssignmentExpressionNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(ConditionalExpressionNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(OrExpressionNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(AndExpressionNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(BitOrExpressionNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(BitXorExpressionNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(BitAndExpressionNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(EqualityExpressionNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(CompareExpressionNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(BitShiftExpressionNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(AdditiveExpressionNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(MultiplicativeExpressionNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(UnaryExpressionNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(PostfixExpressionNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(ParenExpressionNode<JSON>* node) override {
        return JSON();
    }
    

	virtual JSON visit(ClassInheritanceNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(MultipleInheritanceNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(SingleInheritanceNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(MacroNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(UsingNode<JSON>* node) override {
        return JSON();
    }
    

	virtual JSON visit(StatementNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(ElseStatementNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(IfStatementNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(ForStatementNode<JSON>* node) override {
        return JSON();
    }
    

	virtual JSON visit(TypenameNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(NSTypenameNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(TypenameListNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(TemplateTypenameNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(ParenthesisTypenameNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(PointerTypenameNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(PPPQualifierNode<JSON>* node) override {
        return JSON();
    }
    

	virtual JSON visit(ArgumentsSignatureNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(InnerArgumentsNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(ParenArgumentsNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(BracketArgumentsNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(BraceArgumentsNode<JSON>* node) override {
        return JSON();
    }
    
	virtual JSON visit(CodeBlockNode<JSON>* node) override {
        return JSON();
    }
};
