#pragma once
#include <fstream>
#include <string>
#include <sstream>

#include "node_structs.h"

class toCPP {
public:
	void transpile(std::ofstream& h, std::ofstream& cpp, const std::unique_ptr<NodeStructs::File>& file) {
		for (const auto& Class : file->classes) {
			h << "class " << Class->name;
			if (Class->inheritances.size()) {
				h << " : ";
				bool first = true;
				for (const auto& inheritance : Class->inheritances) {
					if (!first)
						h << ", ";
					first = false;
					h << "public ";
					h << transpileType(inheritance);
				}
			}
		}
	}

	std::string transpileType(const std::unique_ptr<NodeStructs::Typename>& type) {
		std::stringstream ss;
		ss << type->type;
		for (const auto& ext : type->extensions)
			if (std::holds_alternative<NodeStructs::NSTypeExtension>(ext))
				ss << "::" << std::get<NodeStructs::NSTypeExtension>(ext).NSTypename;
			else if (std::holds_alternative<NodeStructs::TemplateTypeExtension>(ext)) {
				ss << "<";
				bool isFirst = true;
				for (const auto& T : std::get<NodeStructs::TemplateTypeExtension>(ext).templateTypes) {
					if (!isFirst)
						ss << ", ";
					isFirst = false;
					ss << transpileType(T);
				}
				ss << ">";
			}
			else if (std::holds_alternative<NodeStructs::PointerTypeExtension>(ext)) {
				const NodeStructs::PointerTypeExtension& pt = std::get<NodeStructs::PointerTypeExtension>(ext);
				if (pt.ptr_count) {
					std::stringstream temp;
					for (int i = 0; i < pt.ptr_count; ++i)
						temp << "std::shared_ptr<";
					temp << ss.str();
					for (int i = 0; i < pt.ptr_count; ++i)
						temp << ">";
					std::swap(ss, temp);
				}
				if (pt.isRef)
					ss << "&";
			}
		return ss.str();
	}
};