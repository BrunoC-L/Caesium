#include <iostream>
#include <fstream>
#include <algorithm>

#include "test_parse.hpp"
#include "test_transpile.hpp"
#include "test_transpile_files.hpp"

int main(int argc, char** argv) {

	if (argc == 1) {
		std::cout << "Expected a test folder\n";
		return 1;
	}

	if (argc > 2) {
		std::cout << "Expected a single argument\n";
		return 1;
	}

	std::cout << std::boolalpha;

	if (!testParse())
		return 1;
	std::cout << colored_text("All parse tests passed\n", output_stream_colors::green);

	if (!testTranspile())
		return 1;
	std::cout << colored_text("All transpile tests passed\n", output_stream_colors::green);

	bool compilation_success = true;

	for (int i = 1; i < argc; ++i)
		for (const auto& folder : std::filesystem::directory_iterator(argv[i]))
			if (folder.is_directory() && !folder.path().stem().generic_string().starts_with("."))
				compilation_success &= test_transpile_file(folder);
	if (!compilation_success)
		return 1;
	std::cout << colored_text("All file transpile tests passed\n", output_stream_colors::green);

	return 0;
}
