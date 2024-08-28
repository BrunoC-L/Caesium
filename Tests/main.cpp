#include "test_parse.hpp"
#include "test_structurize.hpp"
#include "test_transpile_files.hpp"
#include "test_template_selection.hpp"

int main(int argc, char** argv) {
	constexpr auto bad_exit = 1;

	if (argc == 1) {
		std::cerr << colored_text("Expected a test folder\n", output_stream_colors::red);
		return bad_exit;
	}

	if (argc > 2) {
		std::cerr << colored_text("Expected a single argument\n", output_stream_colors::red);
		return bad_exit;
	}

	std::cout << std::boolalpha;

	if (!test_parse())
		return bad_exit;
	std::cout << colored_text("All parse tests passed\n", output_stream_colors::green);

	if (!test_structurize_equals())
		return bad_exit;
	std::cout << colored_text("All structurize tests passed\n", output_stream_colors::green);

	if (!test_template_selection())
		return bad_exit;
	std::cout << colored_text("All template selection tests passed\n", output_stream_colors::green);

	if (!test_transpile_all_folders(std::filesystem::directory_iterator{ argv[1] }))
		return bad_exit;
	std::cout << colored_text("All transpile tests passed\n", output_stream_colors::green);

	return 0;
}
