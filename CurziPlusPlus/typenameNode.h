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
				MAKE_NAMED(TypenameNode, "NSTypename")
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
			_COMMA_STAR_("types")
				MAKE_NAMED(TypenameNode, "Typename")
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

class TypenameListNodeEndingWithRShift : public Node {
public:
	baseCtor(TypenameListNodeEndingWithRShift);

	virtual void build() override {
		this->nodes = {
			_AND_
				// NOT A _COMMA_STAR_, this one requires the trailing comma because we are parsing stuff like `E,E<E>>`
				// so we need the rule to fail after `E,` not after `E,E<E>`, we want it to stop before the last type and parse it ourselves
				_STAR_("typenames") _AND_ 
					MAKE_NAMED(TypenameNode, "Typename"),
					TOKEN(COMMA),
				____,
				std::make_shared<WordTokenNode>("word"),
				TOKEN(LT),
				MAKE_NAMED(TypenameListNode, "list"),
				TOKEN(RSHIFT),
				_OPT_("opt_trailing_comma")
					TOKEN(COMMA)
				___
			__
		};
	}

	std::vector<std::unique_ptr<NodeStructs::Typename>> getStruct() {
		std::vector<std::unique_ptr<NodeStructs::Typename>> res;
		for (const auto& node : nodes[0]->nodes[0]->nodes)
			res.emplace_back(NODE_CAST(TypenameNode, node->nodes[0])->getStruct());

		std::unique_ptr<NodeStructs::Typename> trailing = std::make_unique<NodeStructs::Typename>();
		trailing->type = NODE_CAST(WordTokenNode, nodes[0]->nodes[1])->value;

		NodeStructs::TemplateTypeExtension tte;
		tte.templateTypes =	NODE_CAST(TypenameListNode, nodes[0]->nodes[3])->getStruct();
		trailing->extensions.emplace_back(std::move(tte));

		res.emplace_back(std::move(trailing));

		return res;
	}
};

class PointerTypenameNode : public Node {
public:
	baseCtor(PointerTypenameNode);

	virtual void build() override {
		this->nodes = {
				_OR_
					_AND_
						_PLUS_("ptrs")
							TOKEN(ASTERISK)
						___,
						_OPT_("ref")
							TOKEN(AMPERSAND)
						___,
					__,
					TOKEN(AMPERSAND)
				__
		};
	}

	std::vector<NodeStructs::TypenameExtension> getTypenameExtensions() {
		std::vector<NodeStructs::TypenameExtension> res;
		NodeStructs::PointerTypeExtension pt;
		bool hasPtrs = nodes[0]->nodes[0]->nodes.size() == 2;
		pt.isRef = !hasPtrs || nodes[0]->nodes[0]->nodes[1]->nodes.size();
		pt.ptr_count = hasPtrs ? nodes[0]->nodes[0]->nodes[0]->nodes.size() : 0;
		res.emplace_back(std::move(pt));
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
				_OR_
					_AND_
						MAKE_NAMED(TypenameListNode, "TypenameList"),
						TOKEN(GT),
					__,
					MAKE_NAMED(TypenameListNodeEndingWithRShift, "TypenameListNodeEndingWithRShift"),
				__,
				_OPT_("extension") _OR_
					MAKE_NAMED(NSTypenameNode, "NSTypename"),
					MAKE_NAMED(PointerTypenameNode, "PointerTypename"),
				____
			__
		};
	}

	std::vector<NodeStructs::TypenameExtension> getTypenameExtensions() {
		NodeStructs::TemplateTypeExtension tte;
		bool firstList = nodes[0]->nodes[1]->nodes[0]->nodes.size() == 2;
		if (firstList)
			tte.templateTypes = NODE_CAST(TypenameListNode, nodes[0]->nodes[1]->nodes[0]->nodes[0])->getStruct();
		else
			tte.templateTypes = NODE_CAST(TypenameListNodeEndingWithRShift, nodes[0]->nodes[1]->nodes[0])->getStruct();


		std::vector<NodeStructs::TypenameExtension> res;
		res.emplace_back(std::move(tte));

		bool hasExts = nodes[0]->nodes[2]->nodes.size();

		if (hasExts) {
			std::vector<NodeStructs::TypenameExtension> exts;
			if (nodes[0]->nodes[2]->nodes[0]->nodes[0]->identifier == "NSTypename")
				exts = NODE_CAST(NSTypenameNode, nodes[0]->nodes[2]->nodes[0]->nodes[0])->getTypenameExtensions();
			else if (nodes[0]->nodes[2]->nodes[0]->nodes[0]->identifier == "PointerTypename")
				exts = NODE_CAST(PointerTypenameNode, nodes[0]->nodes[2]->nodes[0]->nodes[0])->getTypenameExtensions();
			else
				throw std::exception();
			std::move(exts.begin(), exts.end(), std::back_inserter(res));
			exts.erase(exts.begin(), exts.end());
		}

		return res;
	}
};
