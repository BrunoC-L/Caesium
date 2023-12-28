#pragma once
#include <string_view>
#include <iostream>

static inline size_t first_diff(std::string_view s1, std::string_view s2) {
	auto first_diff_ = size_t{ 0 };
	while (first_diff_ < s1.size() && first_diff_ < s2.size() && s1.at(first_diff_) == s2.at(first_diff_))
		++first_diff_;
	return first_diff_;
}

static inline void print_first_diff(std::string_view s1, std::string_view s2, size_t first_diff) {
	size_t beg = first_diff > 20 ? first_diff - 20 : 0;
	auto max = std::min(s1.length(), s2.length());
	size_t end = std::min(first_diff + 20, max);
	{
		std::cout << "<==";
		auto it = beg;
		while (it != end) {
			std::cout << s1.at(it);
			++it;
		}
		std::cout << "==>\n";
	}
	{
		std::cout << "<==";
		auto it = beg;
		while (it != end) {
			std::cout << s2.at(it);
			++it;
		}
		std::cout << "==>\n";
	}
}
