#include "test_parse.hpp"
#include "test_structurize.hpp"
#include "test_template_selection.hpp"
#include "test_transpile_files.hpp"

bool ignore_caesium_source_location_contents_for_comparison = true;

int main(int argc, char** argv) {
	constexpr auto bad_exit = 1;

	std::cout << std::boolalpha;

	if (!test_parse())
		return bad_exit;
	std::cout << colored_text("All parse tests passed\n", output_stream_colors::green);

	ignore_caesium_source_location_contents_for_comparison = false;
	if (!test_structurize_equals())
		return bad_exit;
	std::cout << colored_text("All structurize tests passed\n", output_stream_colors::green);

	ignore_caesium_source_location_contents_for_comparison = true;

	if (!test_template_selection())
		return bad_exit;
	std::cout << colored_text("All template selection tests passed\n", output_stream_colors::green);

	if (argc == 1) {
		std::cerr << colored_text("Expected a test folder argument. DEV TODO MESSAGE\n", output_stream_colors::red);
		return 0;
	}

	else {
		auto _current_if_any_tests_are_in_current_folder = [&](std::filesystem::path target_dir) -> std::filesystem::path {
			auto _current_folder_path = target_dir / "_current";
			if (std::filesystem::exists(_current_folder_path) && std::filesystem::is_directory(_current_folder_path))
				for (const auto& folder : std::filesystem::directory_iterator{ _current_folder_path })
					if (folder.is_directory())
						return _current_folder_path;
			return target_dir;
		}(std::filesystem::path{ argv[1] });
		
		if (!test_transpile_all_folders(std::filesystem::directory_iterator{ _current_if_any_tests_are_in_current_folder }))
			return bad_exit;
		std::cout << colored_text("All transpile tests passed\n", output_stream_colors::green);
	}

	return 0;
}
