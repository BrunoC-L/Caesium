#pragma once
#include "nodeVisitor.h"
#include "json.h"
#include "fileNode.h"

class SimpleJSONNodeVisitor : public NodeVisitor {
protected:
    JSON value;
public:
    const JSON& getValue() {
        return value;
    }

    virtual void default_behavior(Node* node) override {
        JSON res;
        node->nodes[0]->accept(this);
        res[node->name] = value;
        value = res;
    }

    void kNode(Node* node) {
        auto pushChildrenTo = [&](JSON& arr) {
            for (const auto& child : node->nodes) {
                try {
                    child->accept(this);
                    arr.push(value);
                }
                catch (...) {
                    // ignore tokens and indents
                    continue;
                }
            }
        };
        if (node->identifier == "") {
            JSON res("[]");
            pushChildrenTo(res);
            value = res;
        }
        else {
            JSON res("{'" + node->identifier + "': []}");
            pushChildrenTo(res[node->identifier]);
            value = res;
        }
    }

    virtual void visit(OPTNode* node) override {
        kNode(node);
    }

    virtual void visit(StarNode* node) override {
        kNode(node);
    }

    virtual void visit(CommaStarNode* node) override {
        kNode(node);
    }

    virtual void visit(PlusNode* node) override {
        kNode(node);
    }

    virtual void visit(CommaPlusNode* node) override {
        kNode(node);
    }

    virtual void visit(OrNode* node) override {
        node->nodes[0]->accept(this);
    }

    virtual void visit(AndNode* node) override {
        kNode(node);
        JSON res;
        const auto& arr = value.getChildren();
        if (arr.size() == 0)
            throw std::exception("unexpected number of andnode children");
        for (const auto& child : arr) {
            if (child.size() != 1)
                throw std::exception("unexpected size of andnode child");
            std::string name = child.getProperties().at(0);
            if (name == "")
                throw std::exception("unexpected size of andnode child");
            res[name] = child.get(name);
        }
        value = res;
    }

    virtual void visit(TokenNode_* node) override {
        throw std::exception("most likely developper error trying to get value of token node");
    }

    virtual void visit(PublicToken* node) {
        value = "public";
    }

    virtual void visit(ProtectedToken* node) {
        value = "protected";
    }

    virtual void visit(PrivateToken* node) {
        value = "private";
    }

    virtual void visit(WordTokenNode* node) {
        value = "{'word':'" + node->value + "'}";
    }

    virtual void visit(NumberTokenNode* node) {
        value = "{'number':'" + node->value + "'}";
    }

    virtual void visit(IndentNode* node) override {
        throw std::exception("most likely developper error trying to get value of indent node");
    }

    virtual void visit(UntilTokenNode* node) override {
        JSON res;
        res["which"] = node->name;
        res["value"] = node->value;
        value = res;
    }

    /*virtual void visit(ExpressionNode* node) override {
        value = "{'expression':'expression'}";
    }*/

#define COLLAPSE(T) virtual void visit(T* node) override { collapseExpression(node); }

    COLLAPSE(TemplateTypenameNode);
    COLLAPSE(NSTypenameNode);
    COLLAPSE(TypenameNode);

    COLLAPSE(PostfixExpressionNode);
    COLLAPSE(MultiplicativeExpressionNode);
    COLLAPSE(AdditiveExpressionNode);
    COLLAPSE(CompareExpressionNode);
    COLLAPSE(EqualityExpressionNode);
    COLLAPSE(AndExpressionNode);
    COLLAPSE(OrExpressionNode);
    COLLAPSE(ConditionalExpressionNode);
    COLLAPSE(AssignmentExpressionNode);
    COLLAPSE(ExpressionNode);


    void collapseExpression(Node* collapsibleExpression) {
        ifHas2PropertiesAndOnePropertyMapsToEmptyArrayTakeOnlyOtherProperty(collapsibleExpression);
    }
    
    void ifHas2PropertiesAndOnePropertyMapsToEmptyArrayTakeOnlyOtherProperty(Node* node) {
        node->nodes[0]->accept(this);
        if (value.size() == 1)
            return;
        if (value.size() == 2)
            for (int i = 0; i < 2; ++i)
                if (value[value.getProperties().at(i)].asString() == "[]") {
                    JSON res;
                    auto prop = value.getProperties().at(1 - i);
                    res[prop] = value.get(prop);
                    value = res;
                    return;
                }
        JSON res;
        res[node->name] = value;
        value = res;
    }
};
