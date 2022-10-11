#pragma once
#include "classInheritanceNode.h"
#include "classelementnode.h"
#include "indentNode.h"

class ClassNode : public Node {
public:
	baseCtor(ClassNode);

	virtual void prepare() override {
		this->nodes = {
			_AND_
				TOKEN(CLASS),
				_OR_
					MAKE2(TemplateTypenameDeclarationNode),
					TOKEN(WORD),
				__,
				_OPT_
					MAKE2(ClassInheritanceNode)
				___,
				TOKEN(COLON),
				TOKEN(NEWLINE),
				_STAR_
					_AND_
						MAKE(IndentNode)(n_indent + 1),
						MAKE_INDENTED(ClassElementNode, n_indent + 1)
					__
				___,
			__,
		};
	}

	std::unique_ptr<NodeStructs::Class> getStruct() {
		std::unique_ptr<NodeStructs::Class> res = std::make_unique<NodeStructs::Class>();
		if (auto classTemplate = NODE_CAST(TemplateTypenameDeclarationNode, nodes[0]->nodes[1]->nodes[0])) {
			res->templated.emplace(std::move(classTemplate->getStruct()));
			res->name = res->templated->type;
		}
		else
			res->name = NODE_CAST(TokenNode<WORD>, nodes[0]->nodes[1]->nodes[0])->value;
		if (nodes[0]->nodes[2]->nodes.size())
			res->inheritances = NODE_CAST(ClassInheritanceNode, nodes[0]->nodes[2]->nodes[0])->getInheritance();

		auto classElementF = overload(
			[&res](const NodeStructs::Constructor&) {
			},
			[&res](const NodeStructs::Function&) {
			},
			[&res](const NodeStructs::MemberVariable&) {
			},
			[&res](const NodeStructs::Alias&) {
			}
		);

		for (const auto& classElementAnd : nodes[0]->nodes[5]->nodes) {
			const std::shared_ptr<ClassElementNode>& classElement = NODE_CAST(ClassElementNode, classElementAnd->nodes[1]);
			const auto& x = classElement->getStruct();
			std::visit(classElementF , *x.get());
		}
		return res;
	}
};
