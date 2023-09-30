#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include "grammar.h"
#include "toCpp.h"
#include "structurizer.h"
#include "testParse.h"
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
		return getStruct(file, fileName.stem().generic_string());
	}
	else {
		std::cout << fileName << ": not built\n";
		throw std::exception();
	}
}

void transpile(const std::vector<NodeStructs::File>& project, const std::filesystem::path& folder) {
	const std::string outFileNameNoExt = folder.generic_string() +"/out";
	std::ofstream h(outFileNameNoExt + ".h", std::ios::trunc);
	std::ofstream cpp(outFileNameNoExt + ".cpp", std::ios::trunc);
	if (!h.is_open())
		throw std::exception();
	if (!cpp.is_open())
		throw std::exception();
	cpp << transpile(project);
}

struct test {
	bool operator==(const test&) const = default;
};


int main(int argc, char** argv) {

	{
		std::cout << std::boolalpha;
		testParse();
		//testTranspile();
		std::cout << "\n\n";
	}

	for (int i = 1; i < argc; ++i)
		for (const auto& folder : std::filesystem::directory_iterator(argv[i]))
			if (folder.is_directory() && !folder.path().stem().generic_string().starts_with(".")) {
				std::vector<NodeStructs::File> project;
				for (const auto& file : std::filesystem::directory_iterator(folder))
					if (file.path().extension() == ".caesium")
						project.push_back(caesium2AST(file.path()));
				transpile(project, folder);
			}
	return 0;
}
