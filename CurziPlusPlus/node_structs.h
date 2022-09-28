#pragma once
#include <vector>
#include <string>
#include <variant>
#include <optional>

#define NODE_CAST(T, E) std::dynamic_pointer_cast<T>(E)

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>;

namespace NodeStructs {

	struct File;
	struct Class;
	struct Constructor;
	struct Function;
	struct MemberVariable;
	struct Alias;
	using ClassElement = std::variant<Constructor, Function, MemberVariable, Alias>;
	struct TemplateTypeExtension;
	struct NSTypeExtension;
	struct Typename;
	using TypenameExtension = std::variant<TemplateTypeExtension, NSTypeExtension>;
	struct templateDeclaration;
	struct Statement;

	struct Function {
		std::string name;
		std::unique_ptr<Typename> returnType;
		std::vector<std::unique_ptr<Typename>> parameterTypes;
		std::vector<std::unique_ptr<Statement>> statements;
	};

	struct Alias {
		std::unique_ptr<Typename> aliasFrom;
		std::unique_ptr<Typename> aliasTo;
	};

	struct Constructor {
		std::vector<std::unique_ptr<Typename>> parameterTypes;
		std::vector<std::unique_ptr<Statement>> statements;
	};

	struct MemberVariable {
		std::string name;
		std::unique_ptr<Typename> type;
	};

	struct TemplateTypeExtension {
		std::vector<std::unique_ptr<Typename>> templateTypes;
	};

	struct NSTypeExtension {
		std::string NSTypename;
	};

	struct Typename {
		std::string type;
		std::vector<TypenameExtension> extensions;
	};

	struct templateDeclaration {
		std::string type;
		std::vector<templateDeclaration> templated;
	};

	struct Statement {
	};

	struct File {
		std::vector<std::unique_ptr<Class>> classes;
		std::vector<std::unique_ptr<Function>> functions;
	};

	struct Class {
		std::string name;
		std::optional<templateDeclaration> templated;
		std::vector<std::unique_ptr<Typename>> inheritances;
		std::vector<std::unique_ptr<Alias>> aliases;
		std::vector<std::unique_ptr<Constructor>> constructors;
		std::vector<std::unique_ptr<Function>> methods;
		std::vector<std::unique_ptr<MemberVariable>> memberVariables;
	};
}