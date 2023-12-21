#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

#include "structurizer.h"
#include "grammar.h"
#include "toCpp.h"
#include "test_transpile.hpp"

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

	return test_transpile_error(-1, caesium_opt.value(), error_opt.value());
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

	auto cpp_opt = open_read(header);
	if (!cpp_opt.has_value())
		throw;

	return test_transpile_no_error(-1, caesium_opt.value(), header_opt.value(), cpp_opt.value());
}

bool test_transpile_file_no_error(
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
			return test_transpile_file_no_error(folder, caesium_file, file.path());

	std::cout << "Test folder missing `expected_error.txt` or `expected.hpp`: " << folder << "\n";
	return false;
}

bool test_transpile_file(const std::filesystem::path& folder) {
	for (const auto& file : std::filesystem::directory_iterator{ folder })
		if (file.path().extension() == ".caesium")
			return test_transpile_file(folder, file.path());

	std::cout << "Test folder missing caesium file: " << folder << "\n";
	return 1;
}
