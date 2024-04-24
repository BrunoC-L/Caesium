static constexpr auto default_includes = 
R"DELIM(#pragma once
#include <utility>
#include <iostream>
#include <string>
#include <optional>
#include <variant>
#include <vector>
#include <filesystem>
using Int = int;
using Bool = bool;
using Void = void;
using Floating = double;
template <typename First, typename Second> using Pair = std::pair<First, Second>;
#include <variant>
template <typename... Ts> using Variant = std::variant<Ts...>;
#include <vector>
template <typename T> using Vector = std::vector<T>;
#include <string>
using String = std::string;
using Char = char;
#include <unordered_set>
template <typename T> using Set = std::unordered_set<T>;
#include <set>
template <typename T> using TreeSet = std::set<T>;
#include <unordered_map>
template <typename K, typename V> using Map = std::unordered_map<K, V>;
static constexpr bool True = true;
static constexpr bool False = false;
template<typename... Ts> struct overload : Ts... { using Ts::operator()...; };
template <typename T>
Vector<T>& push(Vector<T>& vec, auto e) { vec.push_back(std::move(e)); return vec; }

constexpr int size(const std::string& str) {
	return (int)str.size();
}

constexpr bool equals(const std::string& str, const std::string& other) {
	return str == other;
}

constexpr bool begins_with(const std::string& str, std::string&& substr) {
	return std::string_view(str).starts_with(substr);
}

constexpr bool ends_with(const std::string& str, std::string&& substr) {
	return std::string_view(str).ends_with(substr);
}

constexpr bool contains(const std::string& str, auto&& substr) {
	auto res = str.find(substr);
	return res != std::string::npos;
}

constexpr std::strong_ordering compare(const std::string& str, const std::string& other) {
	return str <=> other;
}

constexpr std::strong_ordering compare(const std::string& str, const auto& other) {
	return str <=> other;
}

constexpr std::optional<std::string::const_iterator> find(const std::string& str, auto&& substr) {
	if (auto res = str.find(substr); res != std::string::npos)
		return str.begin() + res;
	else
		return std::nullopt;
}

constexpr std::string join(std::string str1, std::string str2) {
	str1 += std::move(str2);
	return std::move(str1);
}

constexpr std::string join(std::string str1, std::string str2, auto&&... strs) {
	return join(join(std::move(str1), std::move(str2)), std::move(strs)...);
}

struct builtin_filesystem_file {
	std::filesystem::directory_entry e;
};

struct builtin_filesystem_directory {
	std::filesystem::directory_entry e;
};

Vector<Variant<builtin_filesystem_file, builtin_filesystem_directory>> filesystem__entries(const builtin_filesystem_directory& dir) {
	Vector<Variant<builtin_filesystem_file, builtin_filesystem_directory>> res{};
	for (const auto& file_or_folder : std::filesystem::directory_iterator(dir.e.path()))
		if (file_or_folder.is_directory())
			res.push_back(builtin_filesystem_directory{ file_or_folder });
		else
			res.push_back(builtin_filesystem_directory{ file_or_folder });
	return res;
}

Vector<Variant<builtin_filesystem_file, builtin_filesystem_directory>> filesystem__entries(const std::string& folder_name) {
	Vector<Variant<builtin_filesystem_file, builtin_filesystem_directory>> res{};
	for (const auto& file_or_folder : std::filesystem::directory_iterator(folder_name))
		if (file_or_folder.is_directory())
			res.push_back(builtin_filesystem_directory{ file_or_folder });
		else
			res.push_back(builtin_filesystem_directory{ file_or_folder });
	return res;
}

)DELIM";
