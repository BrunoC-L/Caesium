#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>

#include "structured/structurizer.hpp"
#include "core/toCpp.hpp"

static NodeStructs::File caesium2AST(const std::filesystem::path& file_name) {
	std::ifstream caesium(file_name);
	if (!caesium.is_open())
		throw;
	std::string program;
	std::getline(caesium, program, '\0');
	grammar::File file(0);
	auto tokens = Tokenizer(program).read();
	Iterator it = { .vec = tokens, .index = 0 , .line = 0, .col = 0, .file_name = file_name.stem().generic_string() };
	if (build(file, it)) {
		std::cout << file_name << ": built\n";
		return getStruct(file_name.stem().generic_string() + ".caesium", tokens, file);
	}
	else {
		std::cout << file_name << ": not built\n";
		throw;
	}
}

std::optional<std::ofstream> open(const std::filesystem::path& folder, std::string_view file_name) {
	auto filepath = folder.generic_string() + file_name.data();
	std::ofstream f(filepath, std::ios::trunc);
	if (!f.is_open()) {
		std::cout << "Unable to open" << filepath << "\n";
		return std::nullopt;
	}
	else
		return f;
}

static bool transpile(const std::vector<NodeStructs::File>& project, const std::filesystem::path& folder) {
	auto cpp_opt = open(folder, "/main.cpp");
	if (!cpp_opt.has_value())
		return false;
	auto& cpp = cpp_opt.value();

	auto compilation_result = transpile(project);
	if (compilation_result.has_value()) {
		cpp << compilation_result.value();
		return true;
	}
	else {
		std::cout << compilation_result.error().message << "\n";
		return false;
	}
}

static auto as_vec(std::filesystem::directory_iterator&& it) {
	return std::vector(std::ranges::begin(it), std::ranges::end(it));
}

int main(int argc, char** argv) {

	if (argc == 1) {
		std::cout << "Expected at least 1 folder to begin compilation from\n";
		return 1;
	}

	bool compilation_success = true;

	for (int i = 1; i < argc; ++i)
		for (const auto& folder : std::filesystem::directory_iterator(argv[i]))
			if (folder.is_directory() && !folder.path().stem().generic_string().starts_with("."))
				compilation_success &=
				transpile(
					as_vec(std::filesystem::directory_iterator(folder))
						| std::views::filter([&](auto&& e) { return e.path().extension() == ".caesium"; })
						| std::views::transform([&](auto&& e) { return e.path(); })
						| std::views::transform([&](auto&& e) { return caesium2AST(e); })
						| to_vec(),
					folder);

	return compilation_success ? 0 : 1;
}
