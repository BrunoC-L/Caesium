#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include "node_structs.h"

class toCPP {
public:
	void transpile(std::ofstream& h, std::ofstream& cpp, const NodeStructs::File& file) {
		h << "#pragma once\n"
			"#include <memory>\n"
			"\n";

		for (const auto& import : file.imports)
			h << "#include " << import.imported << "\n";

		h << "\n";

		for (const auto& Class : file.classes) {
			
			if (Class.templated.has_value())
				transpileTypeTemplateDeclaration(Class.templated.value(), h);

			h << "struct " << Class.name;

			bool firstInheritance = true;
			for (const auto& inheritance : Class.inheritances) {
				if (firstInheritance) {
					firstInheritance = false;
					h << " : ";
				}
				else
					h << ", ";
				transpileType(inheritance, h, false);
			}
			h << " {\n";


			for (const auto& alias : Class.aliases) {
				h << "\tusing ";
				transpileType(alias.aliasFrom, h, false);
				h << " = ";
				transpileType(alias.aliasTo, h, false);
				h << ";\n";
			}

			for (const auto& member : Class.memberVariables) {
				h << "\t";
				transpileType(member.type, h, false); // change to true for shared (removed for now)
				h << " " << member.name << ";\n";
			}

			for (const auto& method : Class.methods) {
				h << "\t";
				transpileType(method.returnType, h, false); // change to true for shared (removed for now)
				h << " " << method.name << "(";
				auto first = true;
				for (const auto& t : method.parameterTypes) {
					if (first)
						first = false;
					else
						h << ", ";
					transpileType(t, h, false); // change to true for shared (removed for now)
				}
				h << ") {\n";
				for (const auto& statement : method.statements)
					std::visit([&](const auto& stmt) {
						transpileStatement(stmt, h, 1);
					}, statement.statement);
				h << "\t}\n";
			}
			h << "};\n\n";
		}
	}

	template <typename stream>
	void transpileStatement(const std::vector<NodeStructs::Statement>& statements, stream& ss, int indent) {
		for (const auto& statement : statements)
			std::visit([&](const auto& stmt) {
				transpileStatement(stmt, ss, indent);
			}, statement.statement);
	}

	template <typename stream>
	void transpileStatement(const NodeStructs::ExpressionStatement& statement, stream& ss, int indnt) {
		indent(ss, indnt);
	}

	template <typename stream>
	void transpileStatement(const NodeStructs::VariableDeclarationStatement& statement, stream& ss, int indnt) {
		indent(ss, indnt);
	}

	template <typename stream>
	void transpileStatement(const NodeStructs::ForStatement& statement, stream& ss, int indnt) {
		indent(ss, indnt);
	}

	template <typename stream>
	void transpileStatement(const NodeStructs::IForStatement& statement, stream& ss, int indnt) {
		indent(ss, indnt);
	}

	template <typename stream>
	void transpileStatement(const NodeStructs::IfStatement& statement, stream& ss, int indnt) {
		indent(ss, indnt);
	}

	template <typename stream>
	void transpileStatement(const NodeStructs::WhileStatement& statement, stream& ss, int indnt) {
		indent(ss, indnt);
	}

	template <typename stream>
	void transpileStatement(const NodeStructs::BreakStatement& statement, stream& ss, int indnt) {
		indent(ss, indnt);
	}

	template <typename stream>
	void transpileStatement(const NodeStructs::ReturnStatement& statement, stream& ss, int indnt) {
		indent(ss, indnt);
	}

	template <typename stream>
	void indent(stream& ss, int n) {
		for (int i = 0; i < n; ++i)
			ss << "\t";
	}

	template <typename stream>
	void transpileType(const NodeStructs::Typename& type, stream& ss, bool shared_ptr) {
		auto f = overload(
			[&](const NodeStructs::NSTypeExtension& ext) {
				ss << "::" << ext.NSTypename;
			},
			[&](const NodeStructs::TemplateTypeExtension& ext) {
				ss << "<";
				bool isFirst = true;
				for (const auto& T : ext.templateTypes) {
					if (!isFirst)
						ss << ", ";
					isFirst = false;
					transpileType(T, ss, shared_ptr);
				}
				ss << ">";
			}
		);
		if (shared_ptr)
			ss << "std::shared_ptr<";
		ss << type.type;
		for (const auto& ext : type.extensions)
			std::visit(f, ext);
		if (shared_ptr)
			ss << ">";
	}

/*	<U>   : template <typename U>
	<U, V>: template <typename U, typename V>
	<U<V>>: template <template <typename V> typename U>*/
	template <typename stream>
	void transpileTypeTemplateDeclaration(const NodeStructs::TemplateDeclaration& tmpl, stream& ss, bool printNameAtEnd = false) {
		if (tmpl.parameters.size())
			ss << "template <";
		bool first = true;
		for (const auto& templateParameterType : tmpl.parameters) {
			if (!first)
				ss << ", ";
			first = false;
			transpileTypeTemplateDeclaration(templateParameterType, ss, true);
		}
		if (tmpl.parameters.size())
			ss << "> ";
		if (printNameAtEnd)
			ss << "typename " << tmpl.name;
		else
			ss << "\n";
	}
};
