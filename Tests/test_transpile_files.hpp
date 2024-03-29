#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

#include "core/structurizer.hpp"
#include "core/grammar.hpp"
#include "core/toCpp.hpp"
#include "first_diff.hpp"

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

std::optional<NodeStructs::File> create_file_struct(const std::string& folder_name, std::string_view caesiumProgram, std::string_view filename) {
	std::vector<TOKENVALUE> tokens(Tokenizer{ std::string{ caesiumProgram } }.read());
	tokens_and_iterator g{ tokens, tokens.begin() };
	auto file = grammar::File(0);
	{
		bool nodeBuilt = build(file, g.it);
		bool programReadEntirely = g.it == g.tokens.end();
		while (!programReadEntirely && (g.it->first == NEWLINE || g.it->first == END))
			programReadEntirely = ++g.it == g.tokens.end();

		if (!(nodeBuilt && programReadEntirely)) {
			std::cout << folder_name
				<< " built: " << colored_text_from_bool(nodeBuilt)
				<< ", entirely: " << colored_text_from_bool(programReadEntirely) << "\n";

			std::cout << caesiumProgram << "\n\n";
			auto it = g.tokens.begin();
			while (it != g.it) {
				std::cout << it->second << " ";
				++it;
			}
			std::cout << "\n";
			return std::nullopt;
		}
	}
	return getStruct(file, filename);
}

bool test_transpile_error(const std::filesystem::path& folder) {
	auto folder_name = folder.filename().string();
	std::vector<NodeStructs::File> vec;
	for (const auto& file : std::filesystem::directory_iterator{ folder })
		if (file.path().extension() == ".caesium") {
			auto caesium_opt = open_read(file.path());
			if (!caesium_opt.has_value())
				return false;
			auto file_opt = create_file_struct(folder.filename().string(), caesium_opt.value(), file.path().filename().string());
			if (!file_opt.has_value())
				return false;
			vec.push_back(std::move(file_opt).value());
		}

	if (vec.size() == 0) {
		std::cout << "Test folder missing caesium file: " << folder << "\n";
		return false;
	}

	auto produced_file_or_error = transpile(vec);

	if (produced_file_or_error.has_value()) {
		auto [header, cpp] = std::move(produced_file_or_error).value();

		std::cout << folder_name << " transpiled: " << colored_text_from_bool(false) << "\n";
		/*auto h_file = std::ofstream{ folder / "produced.hpp" };
		h_file << header;*/
		auto cpp_file = std::ofstream{ folder / "produced.cpp" };
		cpp_file << cpp;
		return false;
	}
	else {
		auto expected_error_opt = open_read(folder / "expected_error.txt");
		if (!expected_error_opt.has_value())
			throw;
		const auto& expected_error = expected_error_opt.value();

		auto error = std::move(produced_file_or_error).error().message;
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

bool test_transpile_no_error(const std::filesystem::path& folder) {
	auto folder_name = folder.filename().string();
	std::vector<NodeStructs::File> vec;
	for (const auto& file : std::filesystem::directory_iterator{ folder })
		if (file.path().extension() == ".caesium") {
			auto caesium_opt = open_read(file.path());
			if (!caesium_opt.has_value())
				return false;
			auto file_opt = create_file_struct(folder.filename().string(), caesium_opt.value(), file.path().filename().string());
			if (!file_opt.has_value())
				return false;
			vec.push_back(std::move(file_opt).value());
		}

	if (vec.size() == 0) {
		std::cout << "Test folder missing caesium file: " << folder << "\n";
		return false;
	}

	auto produced_file_or_error = transpile(vec);

	if (produced_file_or_error.has_value()) {
		/*auto header_opt = open_read(folder / "expected.hpp");
		if (!header_opt.has_value())
			throw;
		const auto& expected_header = header_opt.value();*/

		auto cpp_opt = open_read(folder / "expected.cpp");
		if (!cpp_opt.has_value())
			throw;
		const auto& expected_cpp = cpp_opt.value();

		auto [header, cpp] = std::move(produced_file_or_error).value();

		/*auto first_diff_header = first_diff(header, expected_header);
		bool header_ok = header.size() == expected_header.size() && header.size() == first_diff_header;*/

		auto first_diff_cpp = first_diff(cpp, expected_cpp);
		bool cpp_ok = cpp.size() == expected_cpp.size() && cpp.size() == first_diff_cpp;

		bool ok = /*header_ok && */cpp_ok;
		if (!ok) {
			std::cout << folder_name << " transpiled: " << colored_text_from_bool(ok) << "\n";
			/*if (!header_ok) {
				auto h_file = std::ofstream{ folder / "produced.hpp" };
				h_file << header;
			}*/
			if (!cpp_ok) {
				auto cpp_file = std::ofstream{ folder / "produced.cpp" };
				cpp_file << cpp;
			}
		}
		return ok;
	}
	else {
		auto err = std::move(produced_file_or_error).error().message;
		auto e_file = std::ofstream{ folder / "produced_error.txt" };
		e_file << err;
		std::cout << folder_name << " transpiled: " << colored_text_from_bool(false) << "\n";
		return false;
	}
}

bool test_transpile_folder(const std::filesystem::path& folder) {
	std::remove((folder / "produced_error.txt").string().c_str());
	std::remove((folder / "produced.cpp").string().c_str());

	for (const auto& file : std::filesystem::directory_iterator{ folder })
		if (file.path().filename() == "expected_error.txt")
			return test_transpile_error(folder);
		else if (file.path().filename() == "expected.cpp")
			return test_transpile_no_error(folder);

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
