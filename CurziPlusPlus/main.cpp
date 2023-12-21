#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>

#include "grammar.h"
#include "structurizer.h"
#include "testParse.h"
#include "toCpp.h"
#include "testTranspile.h"

static NodeStructs::File caesium2AST(const std::filesystem::path& fileName) {
	std::ifstream caesium(fileName);
	if (!caesium.is_open())
		throw std::exception();
	std::string program;
	std::getline(caesium, program, '\0');
	File file(0);
	std::forward_list<TOKENVALUE> tokens(Tokenizer(program).read());
	Grammarizer g(tokens);
	if (build_optional_primitive(file, &g)) {
		std::cout << fileName << ": built\n";
		return getStruct(file, fileName.stem().generic_string() + ".caesium");
	}
	else {
		std::cout << fileName << ": not built\n";
		testParse<File>(__LINE__, 0, program);
		throw;
	}
}

std::optional<std::ofstream> open(const std::filesystem::path& folder, std::string_view filename) {
	auto filepath = folder.generic_string() + filename.data();
	std::ofstream f(filepath, std::ios::trunc);
	if (!f.is_open()) {
		std::cout << "Unable to open" << filepath << "\n";
		return std::nullopt;
	}
	else
		return f;
}

static bool transpile(const std::vector<NodeStructs::File>& project, const std::filesystem::path& folder) {
	auto h_opt = open(folder, "/header.h");
	if (!h_opt.has_value())
		return false;
	auto& h = h_opt.value();

	auto cpp_opt = open(folder, "/main.cpp");
	if (!cpp_opt.has_value())
		return false;
	auto& cpp = cpp_opt.value();

	auto compilation_result = transpile(project);
	if (compilation_result.has_value()) {
		auto& [_h, _cpp] = compilation_result.value();
		h << _h;
		cpp << _cpp;
		return true;
	}
	else {
		std::cout << compilation_result.error().content << "\n";
		return false;
	}
}

static auto as_vec(std::filesystem::directory_iterator&& it) {
	return std::vector(std::ranges::begin(it), std::ranges::end(it));
}

int main(int argc, char** argv) {

	std::cout << std::boolalpha;

	if (!testParse())
		return 1;
	std::cout << colored_text("All parse tests passed\n", output_stream_colors::green) << "\n\n";

	if (!testTranspile())
		return 1;
	std::cout << colored_text("All transpile tests passed\n", output_stream_colors::green) << "\n\n";

	bool compilation_success = true;

	for (int i = 1; i < argc; ++i)
		for (const auto& folder : std::filesystem::directory_iterator(argv[i]))
			if (folder.is_directory() && !folder.path().stem().generic_string().starts_with("."))
				compilation_success &=
					transpile(
						as_vec(std::filesystem::directory_iterator(folder))
							| LIFT_FILTER_TRAIL(.path().extension() == ".caesium")
							| LIFT_TRANSFORM_TRAIL(.path())
							| LIFT_TRANSFORM(caesium2AST)
							| to_vec(),
						folder);
	return compilation_success ? 0 : 1;
}
