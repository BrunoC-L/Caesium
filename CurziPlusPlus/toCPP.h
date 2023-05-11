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
					h << " : public ";
				}
				else
					h << ", public ";
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

			for (const auto& ctor : Class.constructors) {
				h << "\t" << Class.name << "(";
				auto first = true;
				for (const auto& type_and_name : ctor.parameters) {
					if (first)
						first = false;
					else
						h << ", ";
					transpileType(type_and_name.first, h, false); // change to true for shared (removed for now)
					h << " " << type_and_name.second;
				}
				h << ") {\n";
				for (const auto& statement : ctor.statements)
					transpileStatement(statement, h, 2);
				h << "\t}\n";
			}

			for (const auto& method : Class.methods) {
				h << "\t";
				transpileType(method.returnType, h, false); // change to true for shared (removed for now)
				h << " " << method.name << "(";
				auto first = true;
				for (const auto& type_and_name : method.parameters) {
					if (first)
						first = false;
					else
						h << ", ";
					transpileType(type_and_name.first, h, false); // change to true for shared (removed for now)
					h << " " << type_and_name.second;
				}
				h << ") {\n";
				for (const auto& statement : method.statements)
					std::visit([&](const auto& stmt) {
						transpileStatement(stmt, h, 2);
					}, statement.statement);
				h << "\t}\n";
			}
			h << "};\n\n";
		}
	}

	template <typename stream>
	void transpileExpression(const NodeStructs::ParenExpression& expr, stream& ss) {
		std::visit(overload(
			[&](const std::unique_ptr<NodeStructs::Expression>& expr) {
				transpileExpression(*expr.get(), ss);
			},
			[&](const NodeStructs::Typename& type) {
				transpileType(type, ss, false);
			},
			[&](const Token<NUMBER>& num) {
				ss << num.value;
			}
		), expr.expr);
	}

	template <typename stream>
	void transpileExpression(const NodeStructs::PostfixExpression& expr, stream& ss) {
		transpileExpression(expr.expr, ss);
		for (const auto& postfix : expr.postfixes) {
			std::visit(overload(
				[&](const std::string& property) {
					ss << "." << property;
				},
				[&](const NodeStructs::ParenArguments& call) {
					ss << "(";
					bool hasPrevious = false;
					for (const NodeStructs::Expression& arg : call.args) {
						if (hasPrevious)
							ss << ", ";
						hasPrevious = true;
						transpileExpression(arg, ss);
					}
					ss << ")";
				},
				[&](const NodeStructs::BracketArguments& access) {
					ss << "[";
					bool hasPrevious = false;
					for (const NodeStructs::Expression& arg : access.args) {
						if (hasPrevious)
							ss << ", ";
						hasPrevious = true;
						transpileExpression(arg, ss);
					}
					ss << "]";
				},
				[&](const auto& token) {
					ss << token.value;
				}
			), postfix);
		}
	}

	template <typename stream>
	void transpileExpression(const NodeStructs::UnaryExpression& expr, stream& ss) {
		std::visit(overload(
			[&](const NodeStructs::PostfixExpression& expr) {
				transpileExpression(expr, ss);
			},
			[&](const NodeStructs::UnaryExpression::op_and_unaryexpr& expr) {
				std::visit(overload(
					[&](const NodeStructs::Typename& type) {
						ss << "(";
						transpileType(type, ss, false);
						ss << ")";
					},
					[&](const auto& token) {
						ss << token.value;
					}
				), expr.first);
				transpileExpression(*expr.second.get(), ss);
			}
		), expr.expr);
	}

	template <typename stream>
	void transpileExpression(const NodeStructs::MultiplicativeExpression& expr, stream& ss) {
		transpileExpression(expr.expr, ss);
		for (const auto& pair : expr.muls) {
			ss << std::visit(
				[](const auto& token) {
					return token.value;
				},
				pair.first
			);
			transpileExpression(pair.second, ss);
		}
	}

	template <typename stream>
	void transpileExpression(const NodeStructs::AdditiveExpression& expr, stream& ss) {
		transpileExpression(expr.expr, ss);
		for (const auto& pair : expr.adds) {
			ss << std::visit(
				[](const auto& token) {
					return token.value;
				},
				pair.first
			);
			transpileExpression(pair.second, ss);
		}
	}

	template <typename stream>
	void transpileExpression(const NodeStructs::CompareExpression& expr, stream& ss) {
		transpileExpression(expr.expr, ss);
		for (const auto& pair : expr.comparisons) {
			ss << std::visit(
				[](const auto& token) {
					return token.value;
				},
				pair.first
			);
			transpileExpression(pair.second, ss);
		}
	}

	template <typename stream>
	void transpileExpression(const NodeStructs::EqualityExpression& expr, stream& ss) {
		transpileExpression(expr.expr, ss);
	}

	template <typename stream>
	void transpileExpression(const NodeStructs::AndExpression& expr, stream& ss) {
		transpileExpression(expr.expr, ss);
	}

	template <typename stream>
	void transpileExpression(const NodeStructs::OrExpression& expr, stream& ss) {
		transpileExpression(expr.expr, ss);
	}

	template <typename stream>
	void transpileExpression(const NodeStructs::ConditionalExpression& expr, stream& ss) {
		transpileExpression(expr.expr, ss);
	}

	template <typename stream>
	void transpileExpression(const NodeStructs::AssignmentExpression& expr, stream& ss) {
		transpileExpression(expr.expr, ss);
		for (const auto& pair : expr.assignments) {
			ss << std::visit(
				[](const auto& token) {
					return token.value;
				},
				pair.first
			);
			transpileExpression(pair.second, ss);
		}
	}

	template <typename stream>
	void transpileStatement(const NodeStructs::Statement& statement, stream& ss, int indnt) {
		std::visit([&](const auto& stmt) {
			transpileStatement(stmt, ss, indnt);
		}, statement.statement);
	}

	template <typename stream>
	void transpileStatement(const NodeStructs::Expression& statement, stream& ss, int indnt) {
		indent(ss, indnt);
		transpileExpression(statement, ss);
		ss << ";\n";
	}
	 
	template <typename stream>
	void transpileStatement(const NodeStructs::VariableDeclarationStatement& statement, stream& ss, int indnt) {
		indent(ss, indnt);
		transpileType(statement.type, ss, false);
		ss << " " << statement.name << ";";
		ss << "\n";
	}

	template <typename stream>
	void transpileStatement(const NodeStructs::ForStatement& statement, stream& ss, int indnt) {
		indent(ss, indnt);
		if (bool requiresStructuredBinding = statement.iterators.size() > 1) {
			ss << "for (auto& forstatementvar : ";
			transpileExpression(statement.collection, ss);
			ss << ") {\n";
			indent(ss, indnt + 1);

			// making a structured binding as auto& [v1,v2,v3] = forstatementvar;
			ss << "auto& [";
			bool hasPrevious = false;
			for (const auto& it : statement.iterators) {
				if (hasPrevious)
					ss << ", ";
				hasPrevious = true;
				std::visit(
					overload(
						[&](const auto& it) {
							const auto& [type, name] = it;
							// if variables are typed, the "auto" structured binding variable name will start with
							// "__" instead and the typed variable will come after with the right name
							ss << name;
						},
						[&](const std::string& name) {
							ss << name;
						}
					),
					it
				);
			}
			ss << "] = forstatementvar;\n";
			for (const auto& it : statement.iterators) {
				std::visit(
					overload(
						[&](const auto& it) {
							const auto& [type, name] = it;
							indent(ss, indnt + 1);
							transpileType(type, ss, false);
							ss << "& "  << name << " = __" << name;
						},
						[&](const std::string& name) { }
					),
					it
				);
			}
		}
		else {
			ss << "for (";
			std::visit(
				overload(
					[&](const auto& it) {
						const auto& [type, name] = it;
						transpileType(type, ss, false);
						ss << "& " << name;
					},
					[&](const std::string& name) {
						ss << "auto& " << name;
					}
				),
				statement.iterators.at(0)
			);
			ss << " : ";
			transpileExpression(statement.collection, ss);
			ss << ") {\n";
		}

		for (const auto& statement : statement.statements)
			transpileStatement(statement, ss, indnt + 1);

		indent(ss, indnt);
		ss << "}\n";
	}

	template <typename stream>
	void transpileStatement(const NodeStructs::IForStatement& statement, stream& ss, int indnt) {
		indent(ss, indnt);
		ss << "\n";
	}

	template <typename stream>
	void transpileStatement(const NodeStructs::IfStatement& statement, stream& ss, int indnt) {
		/*indent(ss, indnt);
		ss << "if (";
		ss << ") {";
		for (const auto& s : statement.ifStatements)
			transpileStatement(s, ss, indnt + 1);
		ss << "}\n";
		if (statement.elseExpr.has_value()) {
			indent(ss, indnt);
		}*/
	}

	template <typename stream>
	void transpileStatement(const NodeStructs::WhileStatement& statement, stream& ss, int indnt) {
		indent(ss, indnt);
		ss << "\n";
	}

	template <typename stream>
	void transpileStatement(const NodeStructs::BreakStatement& statement, stream& ss, int indnt) {
		indent(ss, indnt);
		ss << "\n";
	}

	template <typename stream>
	void transpileStatement(const NodeStructs::ReturnStatement& statement, stream& ss, int indnt) {
		indent(ss, indnt);
		ss << "\n";
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
