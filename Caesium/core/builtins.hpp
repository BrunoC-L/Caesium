#pragma once
#include "toCPP.hpp"
#include "../utility/as_vec.hpp"
#include "../utility/as_map.hpp"

struct builtins {
	//NodeStructs::Function entries_dir = {
	//	.name = "entries",
	//	.name_space = std::nullopt,
	//	.returnType = make_typename(
	//		NodeStructs::TemplatedTypename{
	//			.type = make_typename(NodeStructs::BaseTypename{ "Vector" }, NodeStructs::Reference{}, rule_info_language_element("Vector")),
	//			.templated_with = as_vec(
	//				NodeStructs::WordTypenameOrExpression{ make_typename(
	//					NodeStructs::UnionTypename{
	//						as_vec(
	//							make_typename(NodeStructs::NamespacedTypename{
	//								.name_space = make_typename(NodeStructs::BaseTypename{ "filesystem" }, NodeStructs::Reference{}, rule_info_language_element("filesystem")),
	//								.name_in_name_space = "file"
	//							}, NodeStructs::Reference{}, rule_info_language_element("filesystem::file")),
	//							make_typename(NodeStructs::NamespacedTypename{
	//								.name_space = make_typename(NodeStructs::BaseTypename{ "filesystem" }, NodeStructs::Reference{}, rule_info_language_element("filesystem")),
	//								.name_in_name_space = "directory"
	//							}, NodeStructs::Reference{}, rule_info_language_element("filesystem::directory"))
	//						)
	//					},
	//					NodeStructs::Reference{},
	//					rule_info_language_element("filesystem::file | filesystem::directory")
	//				) }
	//			)
	//		},
	//		NodeStructs::Reference{},
	//		rule_info_language_element("Vector<filesystem::file | filesystem::directory>")
	//	),
	//	.parameters = as_vec(
	//		NodeStructs::FunctionParameter{
	//			.typename_ = make_typename(NodeStructs::NamespacedTypename{
	//				.name_space = make_typename(NodeStructs::BaseTypename{ "filesystem" }, NodeStructs::Reference{}, rule_info_language_element("filesystem")),
	//				.name_in_name_space = "directory"
	//			}, NodeStructs::Reference{}, rule_info_language_element("filesystem::directory")),
	//		//.category = NodeStructs::ParameterCategory{ NodeStructs::Reference{}),
	//		.name = "dir"
	//	}
	//),
	//.statements = {}
	//};

	//NodeStructs::Function entries_str = {
	//	.name = "entries",
	//	.name_space = std::nullopt,
	//	.returnType = make_typename(
	//		NodeStructs::TemplatedTypename{
	//			.type = make_typename(NodeStructs::BaseTypename{ "Vector" }, NodeStructs::Reference{}, rule_info_language_element("Vector")),
	//			.templated_with = as_vec(
	//				NodeStructs::WordTypenameOrExpression{ make_typename(
	//					NodeStructs::UnionTypename{
	//						as_vec(
	//							make_typename(NodeStructs::NamespacedTypename{
	//								.name_space = make_typename(NodeStructs::BaseTypename{ "filesystem" }, NodeStructs::Reference{}, rule_info_language_element("filesystem")),
	//								.name_in_name_space = "file"
	//							}, NodeStructs::Reference{}, rule_info_language_element("filesystem::file")),
	//							make_typename(NodeStructs::NamespacedTypename{
	//								.name_space = make_typename(NodeStructs::BaseTypename{ "filesystem" }, NodeStructs::Reference{}, rule_info_language_element("filesystem")),
	//								.name_in_name_space = "directory"
	//							}, NodeStructs::Reference{}, rule_info_language_element("filesystem::directory"))
	//						)
	//					},
	//					NodeStructs::Reference{},
	//					rule_info_language_element("filesystem::file | filesystem::directory")
	//				) }
	//			)
	//		},
	//		NodeStructs::Reference{},
	//		rule_info_language_element("Vector<filesystem::file | filesystem::directory>")
	//	),
	//	.parameters = as_vec(
	//	NodeStructs::FunctionParameter{
	//		.typename_ = make_typename(NodeStructs::BaseTypename{ "String" }, NodeStructs::Reference{}, rule_info_language_element("String")),
	//		//.category = NodeStructs::ParameterCategory{ NodeStructs::Reference{}),
	//		.name = "dir"
	//		}
	//	),
	//	.statements = {}
	//};

	//Namespace filesystem_ns = {
	//	.name = "filesystem",
	//	.functions = {},//as_map(std::pair{ entries_str.name, as_vec(copy(entries_str), copy(entries_dir)) }),
	//	.aliases = {},/*as_map(
	//		std::pair{
	//			std::string{ "file" },
	//			make_typename(
	//				NodeStructs::BaseTypename{ "builtin_filesystem_file" },
	//				NodeStructs::Reference{},
	//				rule_info_language_element("filesystem::file")
	//			)
	//		},
	//		std::pair{
	//			std::string{ "directory" },
	//			make_typename(
	//				NodeStructs::BaseTypename{ "builtin_filesystem_directory" },
	//				NodeStructs::Reference{},
	//				rule_info_language_element("filesystem::directory")
	//			)
	//		}
	//	),*/
	//	.info = rule_info_language_element("filesystem")
	//};
};
