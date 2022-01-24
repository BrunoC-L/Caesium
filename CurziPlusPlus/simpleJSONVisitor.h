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
        res["type"] = node->name;
        res["value"] = value;
        value = res;
    }

    void kNode(Node* node) {
        JSON res("[]");
        std::function<void(JSON)> push = [&](JSON json) { res.push(json); };
        if (node->identifier != "") {
            res = "{'" + node->identifier + "': []}";
            auto& arr = res[node->identifier];
            push = [&](JSON json) { arr.push(json); };
        }
        for (const auto& child : node->nodes) {
            try {
                child->accept(this);
                push(value);
            }
            catch (const std::exception& e) {
                // ignore tokens and indents
                continue;
            }
        }
        value = res;
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
        kNode(node);
    }

    virtual void visit(AndNode* node) override {
        kNode(node);
    }

    virtual void visit(TokenNode_* node) override {
        throw std::exception("most likely user error trying to get value of token node");
    }

    virtual void visit(WordTokenNode* node) {
        value = "{'word':'" + node->value + "'}";
    }

    virtual void visit(NumberTokenNode* node) {
        value = "{'number':'" + node->value + "'}";
    }

    virtual void visit(IndentNode* node) override {
        throw std::exception("most likely user error trying to get value of indent node");
    }

    virtual void visit(UntilTokenNode* node) override {
        JSON res;
        res["which"] = node->name;
        value = res;
    }
};
