#pragma once
#include <iostream>
#include "core/toCPP.hpp"
#include "structured/structurizer.hpp"
#include "utility/colored_text.hpp"

static std::string test_file_name_stub() {
	return "test_file_name_stub";
}

template <bool expected_to_built, typename... Ts>
bool test_parse(int line, int n_indent, std::string program) {
	auto tokens = Tokenizer(program).read();
	Iterator it = { .vec = tokens, .index = 0 , .line = 0, .col = 0, .file_name = test_file_name_stub() };
	And node = [&]() {
		if constexpr (expected_to_built)
			return And<Ts...>(n_indent);
		else
			// if it is expected to fail, try to parse END to make sure it has to get the entire input
			// otherwise they might succeed by skipping tokens at the end with Star or Opts
			return And<Ts..., Token<END>>(n_indent);
	}();
	try {
		bool nodeBuilt = build(node, it);

		bool programReadEntirely = it.index == it.vec.size();
		while (!programReadEntirely && (it.vec[it.index].first == NEWLINE || it.vec[it.index].first == END))
			programReadEntirely = ++it.index == it.vec.size();

		if ((nodeBuilt && programReadEntirely) != expected_to_built) {
			std::cout << "LINE " << line << (line < 100 ? " : " : ": ")
				<< "built: " << colored_text_from_bool(nodeBuilt)
				<< ", entirely: " << colored_text_from_bool(programReadEntirely) << "\n";

			std::cout << program << "\n\n";
			unsigned index = 0;
			while (index != it.index)
				std::cout << it.vec[index++].second << " ";
			std::cout << "\n";
			return false;
		}
	}
	catch (const parse_error& e) {
		std::stringstream ss;
		ss << "Unable to parse in test "
			<< e.name_of_rule
			<< "\nContent was: \n";
		auto index = e.beg_offset;
		while (index != it.index)
			ss << it.vec[index++].second;
		ss << "\n";
		std::cout << ss.str();
		return false;
	}
	return true;
}

template <typename... Ts>
bool test_parse_correct(auto&&... es) {
	return test_parse<true, Ts...>(std::forward<decltype(es)>(es)...);
}

template <typename... Ts>
bool test_parse_incorrect(auto&&... es) {
	return test_parse<false, Ts...>(std::forward<decltype(es)>(es)...);
}

bool test_parse();
