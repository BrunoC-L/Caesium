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

static void transpile(const std::vector<NodeStructs::File>& project, const std::filesystem::path& folder) {
	std::ofstream h(folder.generic_string() + "/header.h", std::ios::trunc);
	std::ofstream cpp(folder.generic_string() + "/main.cpp", std::ios::trunc);
	if (!h.is_open())
		throw std::exception();
	if (!cpp.is_open())
		throw std::exception();
	auto k = transpile(project);
	if (k.has_value()) {
		auto&& [_h, _cpp] = k.value();
		h << _h;
		cpp << _cpp;
	}
	else
		std::cout << k.error().content;
}

static auto as_vec(std::filesystem::directory_iterator&& it) {
	return std::vector(std::ranges::begin(it), std::ranges::end(it));
}

int main(int argc, char** argv) {
	{
		std::vector<NodeStructs::Typename> vec;
		vec.push_back(NodeStructs::Typename{ NodeStructs::BaseTypename{ "B" } });
		vec.push_back(NodeStructs::Typename{ NodeStructs::BaseTypename{ "A" } });
		auto test1 = NodeStructs::BaseTypename{ "A" } <=> NodeStructs::BaseTypename{ "B" };
		auto test2  = NodeStructs::BaseTypename{ "B" } <=> NodeStructs::BaseTypename{ "A" };
		std::sort(vec.begin(), vec.end());
	}
	{
		std::vector<NodeStructs::BaseTypename> vec;
		vec.push_back({ "B" });
		vec.push_back({ "A" });
		std::sort(vec.begin(), vec.end());
	}



	std::cout << std::boolalpha;

	if (!testParse())
		return 1;
	std::cout << colored_text("All parse tests passed\n", output_stream_colors::green) << "\n\n";

	if (!testTranspile())
		return 1;
	std::cout << colored_text("All transpile tests passed\n", output_stream_colors::green) << "\n\n";

	return 0;

	for (int i = 1; i < argc; ++i)
		for (const auto& folder : std::filesystem::directory_iterator(argv[i]))
			if (folder.is_directory() && !folder.path().stem().generic_string().starts_with("."))
				transpile(
					as_vec(std::filesystem::directory_iterator(folder))
						| LIFT_FILTER_TRAIL(.path().extension() == ".caesium")
						| LIFT_TRANSFORM_TRAIL(.path())
						| LIFT_TRANSFORM(caesium2AST)
						| to_vec(),
					folder);
	return 0;
}
