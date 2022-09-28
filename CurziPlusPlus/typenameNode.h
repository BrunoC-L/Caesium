#pragma once
#include "node.h"
#include "grammarizer.h"
#include "andorNode.h"
#include "tokenNode.h"
#include "macros.h"
#include "kNode.h"

class TypenameNode : public Node {
public:
	baseCtor(TypenameNode);
	virtual void build() override;

	std::unique_ptr<NodeStructs::Typename> getStruct();
};

class NSTypenameNode : public Node {
public:
	baseCtor(NSTypenameNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				TOKEN(NS),
				MAKE2(TypenameNode)
			__
		};
	}

	std::vector<NodeStructs::TypenameExtension> getTypenameExtensions() {
		std::unique_ptr<NodeStructs::Typename> T = NODE_CAST(TypenameNode, nodes[0]->nodes[1])->getStruct();

		std::vector<NodeStructs::TypenameExtension> res;
		NodeStructs::NSTypeExtension nst;
		nst.NSTypename = T->type;
		res.emplace_back(std::move(nst));
		
		std::move(T->extensions.begin(), T->extensions.end(), std::back_inserter(res));
		T->extensions.erase(T->extensions.begin(), T->extensions.end());

		return res;
	}
};

class TypenameListNode : public Node {
public:
	baseCtor(TypenameListNode);

	virtual void build() override {
		this->nodes = {
			_COMMA_STAR_
				MAKE2(TypenameNode)
			___
		};
	}

	std::vector<std::unique_ptr<NodeStructs::Typename>> getStruct() {
		std::vector<std::unique_ptr<NodeStructs::Typename>> res;
		for (const auto& node : nodes[0]->nodes)
			res.emplace_back(NODE_CAST(TypenameNode, node)->getStruct());
		return res;
	}
}; 

class TemplateTypenameDeclarationNode : public Node {
public:
	baseCtor(TemplateTypenameDeclarationNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				TOKEN(WORD),
				TOKEN(LT),
				_COMMA_PLUS_ _OR_
					MAKE2(TemplateTypenameDeclarationNode),
					TOKEN(WORD),
				____,
				TOKEN(GT),
			__
		};
	}

	NodeStructs::templateDeclaration getStruct() {
		NodeStructs::templateDeclaration res;
		res.type = NODE_CAST(TokenNode<WORD>, nodes[0]->nodes[0])->value;
		for (const auto& ornode : nodes[0]->nodes[2]->nodes) {
			if (const auto& templatedecnode = NODE_CAST(TemplateTypenameDeclarationNode, ornode->nodes[0]))
				res.templated.emplace_back(templatedecnode->getStruct());
			else
				res.templated.emplace_back(NodeStructs::templateDeclaration{NODE_CAST(TokenNode<WORD>, ornode->nodes[0])->value, {} });
		}
		return res;
	}
};

class TemplateTypenameNode : public Node {
public:
	baseCtor(TemplateTypenameNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				TOKEN(LT),
				_AND_
					MAKE2(TypenameListNode),
					TOKEN(GT),
				__,
				_OPT_
					MAKE2(NSTypenameNode)
				___
			__
		};
	}

	std::vector<NodeStructs::TypenameExtension> getTypenameExtensions() {
		std::vector<NodeStructs::TypenameExtension> res;
		NodeStructs::TemplateTypeExtension tte;
		bool firstList = nodes[0]->nodes[1]->nodes[0]->nodes.size() == 2;
		tte.templateTypes = NODE_CAST(TypenameListNode, nodes[0]->nodes[1]->nodes[0])->getStruct();

		res.emplace_back(std::move(tte));

		bool hasExts = nodes[0]->nodes[2]->nodes.size();

		if (hasExts) {
			auto exts = NODE_CAST(NSTypenameNode, nodes[0]->nodes[2]->nodes[0])->getTypenameExtensions();
			std::move(exts.begin(), exts.end(), std::back_inserter(res));
			exts.erase(exts.begin(), exts.end());
		}

		return res;
	}
};
