#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

#include "structured/structurizer.hpp"
#include "core/toCPP.hpp"
#include "first_diff.hpp"
#include "utility/replace_all.hpp"
#include "utility/default_includes.hpp"

expected<std::string> open_read(const std::filesystem::path& file) {
	std::ifstream f(file);
	if (!f.is_open()) {
		std::stringstream ss;
		ss << "Unable to open file :`" << file << "`";
		return error{
			"user error",
			std::move(ss).str()
		};
	}
	std::string program;
	std::getline(f, program, '\0');
	program = replace_all(std::move(program), "\r", "");
	return std::move(program);
}

std::string error_for_user(const parse_error& e) {
	size_t line = 1;
	{
		for (auto index = 0; index != e.beg_offset; ++index)
			if (e.vec[index].first == NEWLINE)
				++line;
	}
	std::stringstream ss;
	ss << "Unable to parse "
		<< e.name_of_rule
		<< "\nin file '" << e.file_name
		<< "'\non line " << line
		<< "\nnear: \n";
	for (auto index = e.beg_offset; index != e.vec.size(); ++index)
		if (e.vec[index].first == NEWLINE)
			break;
		else
			ss << e.vec[index].second;
	ss << "\n";
	auto res = ss.str();
	return res;
}

std::optional<NodeStructs::File> create_file_struct(const std::string& folder_name, std::string_view caesiumProgram, std::string_view file_name) {
	std::vector<TokenValue> tokens(Tokenizer{ std::string{ caesiumProgram } }.read());
	Iterator it = { .vec = tokens, .index = 0 , .line = 0, .col = 0, .file_name = std::string{ file_name } };
	auto file = grammar::File(0);
	try {
		bool nodeBuilt = build(file, it);
		bool programReadEntirely = it.index == it.vec.size();
		while (!programReadEntirely && (it.vec[it.index].first == NEWLINE || it.vec[it.index].first == END))
			programReadEntirely = ++it.index == it.vec.size();

		if (nodeBuilt && programReadEntirely)
			return getStruct(std::string{ file_name }, tokens, file);
		else {
			std::cout << folder_name
				<< " built: " << colored_text_from_bool(nodeBuilt)
				<< ", entirely: " << colored_text_from_bool(programReadEntirely) << "\n";

			std::cout << caesiumProgram << "\n\n";
			unsigned index = 0;
			while (index != it.index) {
				std::cout << it.vec[index].second << " ";
				++index;
			}
			std::cout << "\n";
			return std::nullopt;
		}
	}
	catch (const parse_error& e) {
		throw std::runtime_error(error_for_user(e));
	}
}

bool handle_expected_success_parse_error(const std::filesystem::path& folder, const std::runtime_error& e) {
	auto folder_name = folder.filename().string();
	std::string_view err = e.what();
	auto e_file = std::ofstream{ folder / "produced_error.txt" };
	e_file << err;
	std::cout << folder_name << " transpiled: " << colored_text_from_bool(false) << "\n";
	return false;
}

bool handle_expected_error_parse_error(const std::filesystem::path& folder, const std::runtime_error& e) {
	auto folder_name = folder.filename().string();
	auto expected_error_opt = open_read(folder / "expected_error.txt");
	if (!expected_error_opt.has_error())
		NOT_IMPLEMENTED_BUT_PROBABLY_ERROR;
	const auto& expected_error = expected_error_opt.value();

	std::string_view error = e.what();
	auto first_diff_error = first_diff(error, expected_error);
	bool error_ok = error.size() == expected_error.size() && error.size() == first_diff_error;
	if (!error_ok) {
		std::cout << folder_name << " transpiled: " << colored_text_from_bool(false) << "\n";
		auto e_file = std::ofstream{ folder / "produced_error.txt" };
		e_file << error;
	}
	return error_ok;
}

bool test_transpile_error(const std::filesystem::path& folder) {
	auto folder_name = folder.filename().string();
	std::vector<NodeStructs::File> vec;
	for (const auto& file : std::filesystem::directory_iterator{ folder })
		if (file.path().extension() == ".caesium") {
			auto caesium_opt = open_read(file.path());
			if (caesium_opt.has_error())
				return false;
			try {
				auto file_opt = create_file_struct(folder.filename().string(), caesium_opt.value(), file.path().filename().string());
				if (!file_opt.has_value())
					return false;
				vec.push_back(std::move(file_opt).value());
			}
			catch (const std::runtime_error& e) {
				return handle_expected_error_parse_error(folder, e);
			}
		}

	if (vec.size() == 0) {
		std::cout << "Test folder missing caesium file: " << folder << "\n";
		return false;
	}

	auto produced_file_or_error = transpile(vec);

	if (produced_file_or_error.has_value()) {
		std::cout << folder_name << " transpiled: " << colored_text_from_bool(false) << "\n";
		/*auto h_file = std::ofstream{ folder / "produced.hpp" };
		h_file << header;*/
		auto cpp_file = std::ofstream{ folder / "produced.cpp" };
		cpp_file << produced_file_or_error.value();
		return false;
	}
	else {
		auto expected_error_opt = open_read(folder / "expected_error.txt");
		if (!expected_error_opt.has_value())
			NOT_IMPLEMENTED;
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
	std::vector<NodeStructs::File> vec{};
	for (const auto& file : std::filesystem::directory_iterator{ folder })
		if (file.path().extension() == ".caesium") {
			auto caesium_opt = open_read(file.path());
			if (caesium_opt.has_error())
				return false;
			try {
				auto file_opt = create_file_struct(folder.filename().string(), caesium_opt.value(), file.path().filename().string());
				if (!file_opt.has_value())
					return false;
				vec.push_back(std::move(file_opt).value());
			}
			catch (const std::runtime_error& e) {
				return handle_expected_success_parse_error(folder, e);
			}
		}

	if (vec.size() == 0) {
		std::cout << "Test folder missing caesium file: " << folder << "\n";
		return false;
	}
	std::variant<transpile_t, parse_error> produced_file_or_error = [&]() -> std::variant<transpile_t, parse_error> {
		try {
			return transpile(vec);
		}
		catch (parse_error e) {
			return std::move(e);
		}
	}();

	if (std::holds_alternative<transpile_t>(produced_file_or_error) && std::get<transpile_t>(produced_file_or_error).has_value()) {
		auto cpp_opt = open_read(folder / "expected.cpp");
		if (cpp_opt.has_error())
			NOT_IMPLEMENTED;
		const auto& expected_cpp = cpp_opt.value();

		const auto& cpp = std::get<transpile_t>(produced_file_or_error).value();

		auto first_diff_cpp = first_diff(cpp, expected_cpp);
		bool cpp_ok = cpp.size() == expected_cpp.size() && cpp.size() == first_diff_cpp;

		if (!cpp_ok) {
			std::cout << folder_name << " transpiled: " << colored_text_from_bool(false) << "\n";
			print_first_diff(expected_cpp, cpp, first_diff_cpp);
			auto cpp_file = std::ofstream{ folder / "produced.cpp" };
			cpp_file << cpp;
		}
		return cpp_ok;
	}
	else if (std::holds_alternative<transpile_t>(produced_file_or_error)) {
		auto err = std::move(std::get<transpile_t>(produced_file_or_error)).error().message;
		auto e_file = std::ofstream{ folder / "produced_error.txt" };
		e_file << err;
		std::cout << folder_name << " transpiled: " << colored_text_from_bool(false) << "\n";
		return false;
	}
	else {
		const parse_error& e = std::get<parse_error>(produced_file_or_error);
		auto err = "error on rule: " + e.name_of_rule;
		auto e_file = std::ofstream{ folder / "produced_error.txt" };
		e_file << error_for_user(e);
		std::cout << folder_name << " transpiled: " << colored_text_from_bool(false) << "\n";
		return false;
	}
}

bool test_transpile_folder(const std::filesystem::path& folder) {
	std::cout << "Transpile Test " << folder << "\n";

	{
		std::remove((folder / "defaults.hpp").string().c_str());
		std::ofstream defaults{ folder / "defaults.hpp" };
		defaults << default_includes;
		defaults.close();
	}

	std::remove((folder / "produced_error.txt").string().c_str());
	std::remove((folder / "produced.cpp").string().c_str());

	for (const auto& file : std::filesystem::directory_iterator{ folder })
		if (file.path().filename() == "expected_error.txt")
			return test_transpile_error(folder);
		else if (file.path().filename() == "expected.cpp")
			return test_transpile_no_error(folder);

	std::cout << "Test missing `expected_error.txt` or `expected.cpp`, folder was : '" << folder << "'\n";
	return false;
}

bool test_transpile_all_folders(const std::filesystem::directory_iterator& base_folder) {
	bool compilation_success = true;

	for (const auto& folder : base_folder)
		if (folder.is_directory() && !folder.path().stem().generic_string().starts_with(".") && folder.path().stem().generic_string() != "_current")
			compilation_success &= test_transpile_folder(folder);

	return compilation_success;
}
