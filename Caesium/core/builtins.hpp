#pragma once
#include "toCpp.hpp"
#include "../utility/as_vec.hpp"
#include "../utility/as_map.hpp"

struct builtins {
	NodeStructs::Builtin builtin_variant = { "Variant" };

	//vec
	NodeStructs::Builtin builtin_vector = { "Vector" };
	NodeStructs::Builtin builtin_push = { "push" };
	NodeStructs::Builtin builtin_size = { "size" };

	// set, map
	NodeStructs::Builtin builtin_set = { "Set" };
	NodeStructs::Builtin builtin_map = { "Map" };
	NodeStructs::Builtin builtin_insert = { "insert" };

	NodeStructs::Builtin builtin_print = { "print" };
	NodeStructs::Builtin builtin_println = { "println" };

	NodeStructs::Builtin builtin_file = { "builtin_filesystem_file" };
	NodeStructs::Builtin builtin_directory = { "builtin_filesystem_directory" };

	NodeStructs::Function entries_dir = {
		.name = "entries",
		.name_space = std::nullopt,
		.returnType = NodeStructs::Typename{ NodeStructs::TemplatedTypename {
			.type = NodeStructs::Typename{ NodeStructs::BaseTypename{ "Vector" } },
			.templated_with = as_vec(
				NodeStructs::Typename{ NodeStructs::UnionTypename{
					as_vec(
						NodeStructs::Typename{ NodeStructs::NamespacedTypename{
							.name_space = NodeStructs::Typename{ NodeStructs::BaseTypename{ "filesystem" } },
							.name_in_name_space = "file"
						} },
						NodeStructs::Typename{ NodeStructs::NamespacedTypename{
							.name_space = NodeStructs::Typename{ NodeStructs::BaseTypename{ "filesystem" } },
							.name_in_name_space = "directory"
						} }
					)
				} }
			)
		} },
		.parameters = as_vec(
			NodeStructs::FunctionParameter{
				.typename_ = NodeStructs::Typename{ NodeStructs::NamespacedTypename{
					.name_space = NodeStructs::Typename{ NodeStructs::BaseTypename{ "filesystem" } },
					.name_in_name_space = "directory"
				} },
				.category = NodeStructs::ParameterCategory{ NodeStructs::Reference{} },
				.name = "dir"
			}
		),
		.statements = {}
	};

	NodeStructs::Function entries_str = {
		.name = "entries",
		.name_space = std::nullopt,
		.returnType = NodeStructs::Typename{ NodeStructs::TemplatedTypename {
			.type = NodeStructs::Typename{ NodeStructs::BaseTypename{ "Vector" } },
			.templated_with = as_vec(
				NodeStructs::Typename{ NodeStructs::UnionTypename{
					as_vec(
						NodeStructs::Typename{ NodeStructs::NamespacedTypename{
							.name_space = NodeStructs::Typename{ NodeStructs::BaseTypename{ "filesystem" } },
							.name_in_name_space = "file"
						} },
						NodeStructs::Typename{ NodeStructs::NamespacedTypename{
							.name_space = NodeStructs::Typename{ NodeStructs::BaseTypename{ "filesystem" } },
							.name_in_name_space = "directory"
						} }
					)
				} }
			)
		} },
		.parameters = as_vec(
			NodeStructs::FunctionParameter{
				.typename_ = NodeStructs::Typename{ NodeStructs::BaseTypename{ "String" } },
				.category = NodeStructs::ParameterCategory{ NodeStructs::Reference{} },
				.name = "dir"
			}
		),
		.statements = {}
	};
	
	Namespace filesystem_ns = {
		.name = "filesystem",
		.functions = as_map(std::pair{ entries_str.name, as_vec(copy(entries_str), copy(entries_dir)) }),
		.aliases = as_map(
			std::pair{ std::string{ "file" }, NodeStructs::Typename{ NodeStructs::BaseTypename{ "builtin_filesystem_file" } } },
			std::pair{ std::string{ "directory" }, NodeStructs::Typename{ NodeStructs::BaseTypename{ "builtin_filesystem_directory" } } }
		),
	};
};
