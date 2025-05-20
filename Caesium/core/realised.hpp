#pragma once
#include "../structured/node_structs.hpp"

using default_less_than = decltype([](const auto& l, const auto& r) { return l <=> r == std::weak_ordering::less; });

namespace Realised {
	struct PrimitiveType;
	struct Type;
	struct Function;
	struct FunctionType;
	struct InterfaceType;
	struct NamespaceType;
	struct UnionType;
	struct TemplateType;
	struct Builtin;
	struct EnumType;
	struct EnumValueType;
	struct OptionalType;
	struct AggregateType;
	struct VectorType;
	struct SetType;
	struct MapType;
	struct TypeListType;
	struct CompileTimeType;

	/*struct NameSpace {
		std::string name;

		template <typename T> using map_to_vec = std::map<std::string, std::vector<T>, default_less_than>;

		map_to_vec<NodeStructs::Function> functions;
		map_to_vec<Function> realised_functions;

		map_to_vec<NodeStructs::Type> types;
		map_to_vec<Type> realised_types;
		map_to_vec<NodeStructs::Interface> interfaces;

		map_to_vec<NodeStructs::Template> templates;

		map_to_vec<NodeStructs::Block> blocks;
		std::map<std::string, NodeStructs::Typename> aliases;
		map_to_vec<NodeStructs::Enum> enums;

		std::map<std::string, NameSpace> namespaces;
		map_to_vec<Builtin> builtins;
		caesium_source_location info = rule_info_stub<NameSpace>();
	};*/

	struct MetaType {
		using vt = Variant<
			PrimitiveType, // ex. type(1)
			Type, // ex. type Dog -> type(Dog{})

			FunctionType, // ex. Bool has_bone(...) -> type(has_bone)
			InterfaceType, // ex. interface Animal -> type(Animal)
			NamespaceType, // ex. namespace std -> type(std)
			UnionType, // ex. type A, type B -> type(A | B)
			TemplateType, // ex. template X -> type(X)
			Builtin,
			EnumType, // ex. enum E -> type(E)
			EnumValueType, // ex enum E: A, B... -> type(E::A)
			OptionalType, // A?
			AggregateType, // type({A, B, C})

			VectorType, // type(Vector<Int>)
			SetType, // type(Set<Int>)
			MapType, // type(Map<Int, Int>)
			TypeListType, // type(type_list<Int, String, Cat>)

			CompileTimeType // type of a variable declared with #type var = ...
		> ;
		NonCopyableBox<vt> type;
	};

	struct Builtin {
		// lang
		struct builtin_compile_time_error { static constexpr char name[]{ "compile_time_error" }; };
		struct builtin_typeof { static constexpr char name[]{ "typeof" }; };
		struct builtin_type_list { static constexpr char name[]{ "type_list" }; };
		struct builtin_exit { static constexpr char name[]{ "exit" }; };

		struct builtin_vector { static constexpr char name[]{ "Vector" }; };
		struct builtin_set { static constexpr char name[]{ "Set" }; };
		struct builtin_map { static constexpr char name[]{ "Map" }; };
		struct builtin_union { static constexpr char name[]{ "Union" }; };
		

		//// these are done wrong
		//// lib
		//struct builtin_print { static constexpr char name[]{ "print" }; };
		//struct builtin_println { static constexpr char name[]{ "println" }; };
		//struct builtin_file { static constexpr char name[]{ "file" }; };
		//struct builtin_directory { static constexpr char name[]{ "directory" }; };
		//struct builtin_push { static constexpr char name[]{ "push" }; };
		//struct builtin_size { static constexpr char name[]{ "size" }; };
		//struct builtin_insert { static constexpr char name[]{ "insert" }; };


		caesium_lib::variant::type<
			builtin_compile_time_error,
			builtin_typeof,
			builtin_type_list,
			builtin_exit,

			builtin_vector,
			builtin_set,
			builtin_map,
			builtin_union/*,

			builtin_print,
			builtin_println,
			builtin_file,
			builtin_directory,
			builtin_push,
			builtin_size,
			builtin_insert*/
		> builtin;
	};

	struct TemplateType {
		caesium_lib::string::type name;
	};

	struct Argument {
		NodeStructs::ArgumentCategory category;
		MetaType type;
	};

	struct AggregateType {
		caesium_lib::string::type name;
		std::vector<Argument> arguments;
	};

	struct UnionType {
		caesium_lib::string::type name;
		std::vector<MetaType> arguments;
	};

	struct VectorType {
		caesium_lib::string::type name;
		MetaType value_type;
	};

	struct SetType {
		caesium_lib::string::type name;
		MetaType value_type;
	};

	struct MapType {
		caesium_lib::string::type name;
		MetaType key_type;
		MetaType value_type;
	};

	struct TypeListType {
		std::vector<MetaType> types;
	};

	struct void_t {};
	struct empty_optional_t {};

	// these types also hold their value for compile-time stuff
	struct PrimitiveType {

		template <typename T>
		struct NonValued {};
		template <typename T>
		struct Valued {
			T value;
		};
		using vt = Variant<
			NonValued<std::string>,
			NonValued<double>,
			NonValued<int>,
			NonValued<bool>,
			NonValued<void_t>,
			NonValued<char>,
			NonValued<empty_optional_t>,
			Valued<std::string>,
			Valued<double>,
			Valued<int>,
			Valued<bool>,
			Valued<void_t>,
			Valued<char>,
			Valued<empty_optional_t>
		>;
		vt value;
	};

	struct EnumType {
		caesium_lib::string::type name;
		std::reference_wrapper<const NodeStructs::Enum> enum_;
	};

	struct EnumValueType {
		std::string value_name;
		std::string enum_name;
		std::reference_wrapper<const NodeStructs::Enum> enum_;
	};

	struct OptionalType {
		caesium_lib::string::type name;
		MetaType value_type;
	};

	struct CompileTimeType {
		caesium_lib::string::type name;
		MetaType type;
	};

	struct Parameter {
		MetaType type;
		NodeStructs::ParameterCategory category;
	};

	struct Function {
		caesium_lib::string::type name;
		MetaType returnType;
		std::vector<Parameter> parameters;
		caesium_source_location info = rule_info_stub<Function>();
	};

	struct MemberVariable {
		caesium_lib::string::type name;
		MetaType type;
	};

	struct Type {
		caesium_lib::string::type name;
		std::vector<MemberVariable> member_variables;
		caesium_source_location info = rule_info_stub<Type>();
	};

	struct Interface {
		caesium_lib::string::type name;
		std::vector<MemberVariable> member_variables;
		caesium_source_location info = rule_info_stub<Interface>();
	};

	struct InterfaceType {
		caesium_lib::string::type name;
		std::reference_wrapper<const Interface> interface;
	};

	struct NamespaceType {
		caesium_lib::string::type name;
		std::reference_wrapper<const NodeStructs::NameSpace> name_space;
	};

	struct FunctionType {
		caesium_lib::string::type name;
		// std::vector<std::reference_wrapper<const Function>> overload_set;
	};
}
