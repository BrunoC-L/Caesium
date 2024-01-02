#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

#include "structurizer.hpp"
#include "grammar.hpp"
#include "toCpp.hpp"
#include "first_diff.hpp"

std::optional<std::expected<std::pair<std::string, std::string>, user_error>> create_file(const std::string& folder_name, std::string_view caesiumProgram) {
	std::forward_list<TOKENVALUE> tokens(Tokenizer{ std::string{ caesiumProgram } }.read());
	tokens_and_iterator g{ tokens, tokens.begin() };
	auto file = File(0);
	{
		bool nodeBuilt = file.build(g);
		bool programReadEntirely = g.it == g.tokens.end();
		while (!programReadEntirely && (g.it->first == NEWLINE || g.it->first == END))
			programReadEntirely = ++g.it == g.tokens.end();

		if (!(nodeBuilt && programReadEntirely)) {
			std::cout << folder_name
				<< " built: " << colored_text_from_bool(nodeBuilt)
				<< ", entirely: " << colored_text_from_bool(programReadEntirely) << "\n";

			std::cout << caesiumProgram << "\n\n";
			auto save = g.it;
			g.it = g.tokens.begin();
			while (g.it != save) {
				std::cout << g.it->second << " ";
				++g.it;
			}
			std::cout << "\n";
			return std::nullopt;
		}
	}
	NodeStructs::File f = getStruct(file, "no_name_test_file");
	return transpile(std::vector{ std::move(f) });
}

bool test_transpile_no_error(const std::filesystem::path& folder, std::string_view caesiumProgram, std::string_view expected_header, std::string_view expected_cpp) {
	auto folder_name = folder.filename().string();
	auto optional_produced_file = create_file(folder_name, caesiumProgram);
	if (!optional_produced_file.has_value())
		return false;
	auto& produced_file_or_error = optional_produced_file.value();

	if (produced_file_or_error.has_value()) {
		auto [header, cpp] = std::move(produced_file_or_error).value();

		auto first_diff_header = first_diff(header, expected_header);
		bool header_ok = header.size() == expected_header.size() && header.size() == first_diff_header;

		auto first_diff_cpp = first_diff(cpp, expected_cpp);
		bool cpp_ok = cpp.size() == expected_cpp.size() && cpp.size() == first_diff_cpp;

		bool ok = header_ok && cpp_ok;
		if (!ok) {
			std::cout << folder_name << " transpiled: " << colored_text_from_bool(ok) << "\n";
			if (!header_ok) {
				auto h_file = std::ofstream{ folder / "produced.hpp" };
				h_file << header;
			}
			if (!cpp_ok) {
				auto cpp_file = std::ofstream{ folder / "produced.cpp" };
				cpp_file << cpp;
			}
		}
		return ok;
	}
	else {
		auto err = std::move(produced_file_or_error).error().content;
		auto e_file = std::ofstream{ folder / "produced_error.txt" };
		e_file << err;
		std::cout << folder_name << " transpiled: " << colored_text_from_bool(false) << "\n";
		return false;
	}
}

bool test_transpile_error(const std::filesystem::path& folder, std::string_view caesiumProgram, std::string_view expected_error) {
	auto folder_name = folder.filename().string();
	auto optional_produced_file = create_file(folder_name, caesiumProgram);
	if (!optional_produced_file.has_value())
		return false;
	auto produced_file_or_error = optional_produced_file.value();

	if (produced_file_or_error.has_value()) {
		auto [header, cpp] = std::move(produced_file_or_error).value();
		auto h_file = std::ofstream{ folder / "produced.hpp" };
		h_file << header;
		auto cpp_file = std::ofstream{ folder / "produced.cpp" };
		cpp_file << cpp;
		return false;
	}
	else {
		auto error = std::move(produced_file_or_error).error().content;
		auto first_diff_error = first_diff(error, expected_error);
		bool error_ok = error.size() == expected_error.size() && error.size() == first_diff_error;
		if (!error_ok) {
			std::cout << folder_name << " transpiled: " << colored_text_from_bool(false) << "\n";
			auto e_file = std::ofstream{ folder / "produced_error.txt" };
			e_file << error;
		}
		return error_ok;
	}
}

std::optional<std::string> open_read(const std::filesystem::path& file) {
	std::ifstream f(file);
	if (!f.is_open()) {
		std::cout << "Unable to open" << file << "\n";
		return std::nullopt;
	}
	std::string program;
	std::getline(f, program, '\0');
	return std::move(program);
}

bool test_transpile_file_error(
	const std::filesystem::path& folder,
	const std::filesystem::path& caesium_file,
	const std::filesystem::path& error_file
) {
	auto caesium_opt = open_read(caesium_file);
	if (!caesium_opt.has_value())
		throw;

	auto error_opt = open_read(error_file);
	if (!error_opt.has_value())
		throw;

	return test_transpile_error(folder, caesium_opt.value(), error_opt.value());
}

bool test_transpile_file_no_error(
	const std::filesystem::path& folder,
	const std::filesystem::path& caesium_file,
	const std::filesystem::path& header,
	const std::filesystem::path& cpp
) {
	auto caesium_opt = open_read(caesium_file);
	if (!caesium_opt.has_value())
		throw;

	auto header_opt = open_read(header);
	if (!header_opt.has_value())
		throw;

	auto cpp_opt = open_read(cpp);
	if (!cpp_opt.has_value())
		throw;

	return test_transpile_no_error(folder, caesium_opt.value(), header_opt.value(), cpp_opt.value());
}

bool test_transpile_file_no_error_step(
	const std::filesystem::path& folder,
	const std::filesystem::path& caesium_file,
	const std::filesystem::path& header
) {
	for (const auto& file : std::filesystem::directory_iterator{ folder })
		if (file.path().filename() == "expected.cpp")
			return test_transpile_file_no_error(folder, caesium_file, header, file.path());

	std::cout << "Test folder missing `expected.cpp`: " << folder << "\n";
	return false;
}

bool test_transpile_file(
	const std::filesystem::path& folder,
	const std::filesystem::path& caesium_file
) {
	for (const auto& file : std::filesystem::directory_iterator{ folder })
		if (file.path().filename() == "expected_error.txt")
			return test_transpile_file_error(folder, caesium_file, file.path());
		else if (file.path().filename() == "expected.hpp")
			return test_transpile_file_no_error_step(folder, caesium_file, file.path());

	std::cout << "Test folder missing `expected_error.txt` or `expected.hpp`: " << folder << "\n";
	return false;
}

bool test_transpile_folder(const std::filesystem::path& folder) {
	std::remove((folder / "produced_error.txt").string().c_str());
	std::remove((folder / "produced.hpp").string().c_str());
	std::remove((folder / "produced.cpp").string().c_str());
	for (const auto& file : std::filesystem::directory_iterator{ folder })
		if (file.path().extension() == ".caesium")
			return test_transpile_file(folder, file.path());

	std::cout << "Test folder missing caesium file: " << folder << "\n";
	return false;
}

bool test_transpile_all_folders(const std::filesystem::directory_iterator& base_folder) {
	bool compilation_success = true;

	for (const auto& folder : base_folder)
		if (folder.is_directory() && !folder.path().stem().generic_string().starts_with("."))
			compilation_success &= test_transpile_folder(folder);

	return compilation_success;
}
