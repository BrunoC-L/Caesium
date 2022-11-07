#pragma once
#include <vector>
#include <string>
#include <variant>
#include <optional>

#define NODE_CAST(T, E) std::dynamic_pointer_cast<T>(E)

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>; // to help IDE

namespace NodeStructs {
	struct Typename;

	struct TemplateTypeExtension {
		std::vector<Typename> templateTypes;
	};

	struct NSTypeExtension {
		std::string NSTypename;
	};

	using TypenameExtension = std::variant<TemplateTypeExtension, NSTypeExtension>;

	struct Typename {
		std::string type;
		std::vector<TypenameExtension> extensions;
	};

	struct Alias {
		Typename aliasFrom;
		Typename aliasTo;
	};

	struct ForStatement {

	};

	struct IfStatement {

	};

	struct WhileStatement {

	};

	using Statement = std::variant<std::vector<Statement>, ForStatement, IfStatement, WhileStatement>;

	struct Constructor {
		std::vector<Typename> parameterTypes;
		std::vector<Statement> statements;
	};

	struct MemberVariable {
		std::string name;
		Typename type;
	};

	struct Import {
		std::string imported;
	};

	struct TemplateDeclaration {
		std::string name;
		std::vector<TemplateDeclaration> parameters;
	};

	struct Function {
		std::string name;
		std::optional<TemplateDeclaration> templated;
		NodeStructs::Typename returnType;
		std::vector<Typename> parameterTypes;
		std::vector<Statement> statements;
	};

	struct Class {
		std::string name;
		std::optional<TemplateDeclaration> templated;
		std::vector<Typename> inheritances;
		std::vector<Alias> aliases;
		std::vector<Constructor> constructors;
		std::vector<Function> methods;
		std::vector<MemberVariable> memberVariables;

		template <typename T> std::vector<T>& get() {
			if constexpr (std::is_same_v<T, Alias>)
				return aliases;
			else if constexpr (std::is_same_v<T, Constructor>)
				return constructors;
			else if constexpr (std::is_same_v<T, Function>)
				return methods;
			else if constexpr (std::is_same_v<T, MemberVariable>)
				return memberVariables;
			else
				static_assert(!sizeof(T*), "T is not supported");
		}
	};

	struct File {
		std::vector<Import> imports;
		std::vector<Class> classes;
		std::vector<Function> functions;
	};
}
