#pragma once
#include <string>

// based on https://stackoverflow.com/a/24315631/10945691
static std::string replace_all(std::string str, const std::string& from, const std::string& to, auto&&... args) {
	if constexpr (sizeof...(args) > 0)
		str = replace_all(std::move(str), std::forward<decltype(args)>(args)...);
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}
