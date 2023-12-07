#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>

#include "grammar.h"
#include "structurizer.h"
#include "testParse.h"
#include "toCpp.h"
#include "testTranspile.h"

NodeStructs::File caesium2AST(const std::filesystem::path& fileName) {
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
		throw std::exception();
	}
}

void transpile(const std::vector<NodeStructs::File>& project, const std::filesystem::path& folder) {
	std::ofstream h(folder.generic_string() + "/header.h", std::ios::trunc);
	std::ofstream cpp(folder.generic_string() + "/main.cpp", std::ios::trunc);
	if (!h.is_open())
		throw std::exception();
	if (!cpp.is_open())
		throw std::exception();
	auto k = transpile(project);
	if (k.has_value()) {
		auto [_h, _cpp] = k.value();
		h << std::move(_h);
		cpp << std::move(_cpp);
	}
	else
		std::cout << k.error().content;
}

auto as_vec(std::filesystem::directory_iterator&& it) {
	return std::vector(std::ranges::begin(it), std::ranges::end(it));
}

int main(int argc, char** argv) {

	{
		std::cout << std::boolalpha;
		testParse();
		//testTranspile();
		std::cout << "\n\n";
	}

	for (int i = 1; i < argc; ++i)
		for (const auto& folder : std::filesystem::directory_iterator(argv[i]))
			if (folder.is_directory() && !folder.path().stem().generic_string().starts_with("."))
				transpile(
					as_vec(std::filesystem::directory_iterator(folder))
						| std::views::filter([](const auto& file) { return file.path().extension() == ".caesium"; })
						| std::views::transform([](const auto& file) { return caesium2AST(file.path()); })
						| std::ranges::to<std::vector>(),
					folder);
	return 0;
}
