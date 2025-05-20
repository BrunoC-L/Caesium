#pragma once
#include "toCPP.hpp"
#include "../utility/as_vec.hpp"
#include "../utility/as_map.hpp"

struct caesium_standard_library_builtins {
	//Realised::Function realised_entries_dir = {
	//	.name = "entries",
	//	.returnType = make_type(
	//		Realised::VectorType{
	//			.name = "Vector_TODORIGHTFORMATWITHARGS?", // todo TODO
	//			.value_type = make_type(
	//				Realised::UnionType{
	//					.name = "Union_TODORIGHTFORMATWITHARGS", // todo TODO
	//					.arguments = as_vec(
	//						make_type(Realised::Builtin{ Realised::Builtin::builtin_file{} }),
	//						make_type(Realised::Builtin{ Realised::Builtin::builtin_directory{} })
	//					)
	//				}
	//			)
	//		}
	//	),
	//	.parameters = as_vec(make_parameter(
	//		make_type(Realised::Builtin{ Realised::Builtin::builtin_directory{} }),
	//		NodeStructs::ParameterCategory{ NodeStructs::Reference{} }
	//	)),
	//	.info = rule_info_stub_no_throw(),
	//};
	//Realised::Function realised_entries_str = {
	//	.name = "entries",
	//	.returnType = make_type(
	//		Realised::VectorType{
	//			.name = "Vector_TODORIGHTFORMATWITHARGS?", // todo TODO
	//			.value_type = make_type(
	//				Realised::UnionType{
	//					.name = "Union_TODORIGHTFORMATWITHARGS", // todo TODO
	//					.arguments = as_vec(
	//						make_type(Realised::Builtin{ Realised::Builtin::builtin_file{} }),
	//						make_type(Realised::Builtin{ Realised::Builtin::builtin_directory{} })
	//					)
	//				}
	//			)
	//		}
	//	),
	//	.parameters = as_vec(make_parameter(
	//		make_type(Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<std::string>{} }),
	//		NodeStructs::ParameterCategory{ NodeStructs::Reference{} }
	//	)),
	//	.info = rule_info_stub_no_throw(),
	//};

	NodeStructs::Function entries_dir = {
		.name = "entries",
		.name_space = std::nullopt,
		.returnType = make_typename(
			NodeStructs::TemplatedTypename{
				.type = make_typename(NodeStructs::BaseTypename{ "Vector" }, NodeStructs::Reference{}, rule_info_language_element("Vector")),
				.templated_with = as_vec(
					NodeStructs::WordTypenameOrExpression{ make_typename(
						NodeStructs::UnionTypename{
							as_vec(
								make_typename(NodeStructs::NamespacedTypename{
									.name_space = make_typename(NodeStructs::BaseTypename{ "filesystem" }, NodeStructs::Reference{}, rule_info_language_element("filesystem")),
									.name_in_name_space = "file"
								}, NodeStructs::Reference{}, rule_info_language_element("filesystem::file")),
								make_typename(NodeStructs::NamespacedTypename{
									.name_space = make_typename(NodeStructs::BaseTypename{ "filesystem" }, NodeStructs::Reference{}, rule_info_language_element("filesystem")),
									.name_in_name_space = "directory"
								}, NodeStructs::Reference{}, rule_info_language_element("filesystem::directory"))
							)
						},
						NodeStructs::Reference{},
						rule_info_language_element("filesystem::file | filesystem::directory")
					) }
				)
			},
			NodeStructs::Reference{},
			rule_info_language_element("Vector<filesystem::file | filesystem::directory>")
		),
		.parameters = as_vec(
			NodeStructs::FunctionParameter{
				.typename_ = make_typename(NodeStructs::NamespacedTypename{
					.name_space = make_typename(NodeStructs::BaseTypename{ "filesystem" }, NodeStructs::Reference{}, rule_info_language_element("filesystem")),
					.name_in_name_space = "directory"
				}, NodeStructs::Reference{}, rule_info_language_element("filesystem::directory")),
			//.category = NodeStructs::ParameterCategory{ NodeStructs::Reference{}),
			.name = "dir"
			}
		),
		.statements = {},
		.info = rule_info_stub_no_throw()
	};

	NodeStructs::Function entries_str = {
		.name = "entries",
		.name_space = std::nullopt,
		.returnType = make_typename(
			NodeStructs::TemplatedTypename{
				.type = make_typename(NodeStructs::BaseTypename{ "Vector" }, NodeStructs::Reference{}, rule_info_language_element("Vector")),
				.templated_with = as_vec(
					NodeStructs::WordTypenameOrExpression{ make_typename(
						NodeStructs::UnionTypename{
							as_vec(
								make_typename(NodeStructs::NamespacedTypename{
									.name_space = make_typename(NodeStructs::BaseTypename{ "filesystem" }, NodeStructs::Reference{}, rule_info_language_element("filesystem")),
									.name_in_name_space = "file"
								}, NodeStructs::Reference{}, rule_info_language_element("filesystem::file")),
								make_typename(NodeStructs::NamespacedTypename{
									.name_space = make_typename(NodeStructs::BaseTypename{ "filesystem" }, NodeStructs::Reference{}, rule_info_language_element("filesystem")),
									.name_in_name_space = "directory"
								}, NodeStructs::Reference{}, rule_info_language_element("filesystem::directory"))
							)
						},
						NodeStructs::Reference{},
						rule_info_language_element("filesystem::file | filesystem::directory")
					) }
				)
			},
			NodeStructs::Reference{},
			rule_info_language_element("Vector<filesystem::file | filesystem::directory>")
		),
		.parameters = as_vec(
		NodeStructs::FunctionParameter{
			.typename_ = make_typename(NodeStructs::BaseTypename{ "String" }, NodeStructs::Reference{}, rule_info_language_element("String")),
			//.category = NodeStructs::ParameterCategory{ NodeStructs::Reference{}),
			.name = "dir"
			}
		),
		.statements = {},
		.info = rule_info_stub_no_throw()
	};

	/*Namespace filesystem_ns = {
		.name = "filesystem",
		.functions = as_map_cmp<default_less_than>(
			std::pair{
				entries_str.name,
				as_vec(copy(entries_str), copy(entries_dir))
			}
		),
		.aliases = as_map(
			std::pair{
				std::string{ "file" },
				make_typename(
					NodeStructs::BaseTypename{ "filesystem__file" },
					NodeStructs::Reference{},
					rule_info_language_element("filesystem::file")
				)
			},
			std::pair{
				std::string{ "directory" },
				make_typename(
					NodeStructs::BaseTypename{ "filesystem__directory" },
					NodeStructs::Reference{},
					rule_info_language_element("filesystem::directory")
				)
			}
		),
		.info = rule_info_language_element("filesystem")
	};*/
};
