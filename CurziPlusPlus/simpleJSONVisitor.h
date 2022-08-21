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

    std::string getPreferredProperty(Node* n) {
        return n->identifier == "" ? n->name : n->identifier;
    }

    virtual void default_behavior(Node* node) override {
        node->nodes[0]->accept(this);
        JSON res;
        res[getPreferredProperty(node)] = value;
        value = res;
    }

    void kNode(Node* node) {
        auto pushChildrenTo = [&](JSON& arr) {
            for (const auto& child : node->nodes) {
                child->accept(this);
                arr.push(value);
            }
        };
        // non named collections are just arrays
        if (node->identifier == "") {
            JSON res("[]");
            pushChildrenTo(res);
            value = res;
        }
        // named collections are placed into an object with a single property
        else {
            JSON res;
            res[node->identifier] = "[]";
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
        if (node->identifier == "")
            return;
        JSON temp;
        temp[node->nodes[0]->identifier] = value;
        value = temp;
    }

    virtual void visit(AndNode* node) override {
        kNode(node);
        const auto& arr = value.getChildren();
        if (arr.size() == 0)
            throw std::exception("unexpected number of AndNode children");

        // unpack children json objects into a single json object (and hope for no key collisions!)
        JSON res;
        for (const auto& child : arr)
            for (int i = 0; i < child.size(); ++i) {
                const std::string& key = child.getProperties().at(i);
                if (res.find(key) == res.getProperties().end())
                    res[key] = child.getChildren().at(i);
                else {
                    std::cout << "key collision " << key << ": " << child.getChildren().at(i).asString() << " & " << res.get(key).asString() << "\n";
                    throw std::exception(("key collision, key was: " + key).c_str());
                }
            }
        value = res;
    }

    virtual void visit(TokenNode_* node) override {
        value = JSON();
        if (node->identifier != "")
            value[node->identifier] = node->value;
    }

    virtual void visit(PublicToken* node) {
        JSON res;
        res[getPreferredProperty(node)] = node->value;
        value = res;
    }

    virtual void visit(ProtectedToken* node) {
        JSON res;
        res[getPreferredProperty(node)] = node->value;
        value = res;
    }

    virtual void visit(PrivateToken* node) {
        JSON res;
        res[getPreferredProperty(node)] = node->value;
        value = res;
    }

    virtual void visit(WordTokenNode* node) {
        JSON res;
        res[getPreferredProperty(node)] = node->value;
        value = res;
    }

    virtual void visit(NumberTokenNode* node) {
        JSON res;
        res[getPreferredProperty(node)] = node->value;
        value = res;
    }

    virtual void visit(IndentNode* node) override {
        value = JSON();
        if (node->identifier != "")
            value[node->identifier] = std::to_string(node->n_indent);
    }

    virtual void visit(UntilTokenNode* node) override {
        JSON res;
        res[getPreferredProperty(node)] = node->value;
        value = res;
    }

#define COLLAPSE(T) virtual void visit(T* node) override { collapseExpression(node, #T); }

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


    void collapseExpression(Node* collapsibleExpression, std::string prop) {
        ifHas2PropertiesAndOnePropertyMapsToEmptyArrayTakeOnlyOtherProperty(collapsibleExpression);
        JSON temp;
        temp[prop] = value;
        value = temp;
    }
    
    void ifHas2PropertiesAndOnePropertyMapsToEmptyArrayTakeOnlyOtherProperty(Node* node) {
        node->nodes[0]->accept(this);
        if (value.size() == 1)
            return;
        if (value.size() == 2)
            for (int i = 0; i < 2; ++i)
                if (value.getChildren().at(i).asString() == "[]") {
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
