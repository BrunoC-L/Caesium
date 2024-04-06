#pragma once
#include "toCpp.hpp"

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

	NodeStructs::Builtin builtin_file = { "file" };
	NodeStructs::Builtin builtin_directory = { "directory" };

	NodeStructs::Function entries_dir = {
		"entries",
		std::nullopt,
		NodeStructs::Typename{ NodeStructs::TemplatedTypename {
			.type = NodeStructs::Typename{ NodeStructs::BaseTypename{ "Vector" } },
			.templated_with = std::vector{
				NodeStructs::Typename{ NodeStructs::UnionTypename{
					std::vector<NodeStructs::Typename>{
						NodeStructs::Typename{ NodeStructs::BaseTypename{ "file" } },
						NodeStructs::Typename{ NodeStructs::BaseTypename{ "directory" } }
					}
				} }
			}
		} },
		{
			NodeStructs::FunctionParameter{
				NodeStructs::Typename{ NodeStructs::Typename{ NodeStructs::BaseTypename{ "directory" } } },
				NodeStructs::ParameterCategory{ NodeStructs::Reference{} },
				std::string{ "dir" }
			}
		},
		{}
	};

	NodeStructs::Function entries_str = {
		"entries",
		std::nullopt,
		NodeStructs::Typename{ NodeStructs::TemplatedTypename {
			.type = NodeStructs::Typename{ NodeStructs::BaseTypename{ "Vector" } },
			.templated_with = std::vector{
				NodeStructs::Typename{ NodeStructs::UnionTypename{
					std::vector<NodeStructs::Typename>{
						NodeStructs::Typename{ NodeStructs::BaseTypename{ "file" } },
						NodeStructs::Typename{ NodeStructs::BaseTypename{ "directory" } }
					}
				} }
			}
		} },
		{
			NodeStructs::FunctionParameter{
				NodeStructs::Typename{ NodeStructs::BaseTypename{ "String" } },
				NodeStructs::ParameterCategory{ NodeStructs::Reference{} },
				std::string{ "dir" }
			}
		},
		{}
	};
	
	NodeStructs::NameSpace filesystem_ns = {
		.name = "filesystem",
		.name_space = std::nullopt,
		.functions = { entries_dir, entries_str },
		.types = {},
	};
};