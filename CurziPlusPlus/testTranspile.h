#pragma once
#include <iostream>
#include "toCpp.h"
#include "structurizer.h"
#include "colored_text.hpp"

size_t first_diff(std::string_view s1, std::string_view s2) {
	auto first_diff_ = 0;
	for (first_diff_ = 0; first_diff_ < s1.size(); ++first_diff_) {
		if (first_diff_ >= s2.size())
			break;
		if (s1.at(first_diff_) != s2.at(first_diff_))
			break;
	}
	return first_diff_;
}

std::optional<std::expected<std::pair<std::string, std::string>, user_error>> create_file(int line, std::string_view caesiumProgram) {
	std::forward_list<TOKENVALUE> tokens(Tokenizer{ std::string{ caesiumProgram } }.read());
	Grammarizer g(tokens);
	auto file = File(0);
	{
		bool nodeBuilt = file._value.build(&g);
		bool programReadEntirely = g.it == g.tokens.end();
		while (!programReadEntirely && (g.it->first == NEWLINE || g.it->first == END))
			programReadEntirely = ++g.it == g.tokens.end();

		if (!(nodeBuilt && programReadEntirely)) {
			std::cout << "LINE " << line << (line < 100 ? " : " : ": ")
				<< "built: " << colored_text_from_bool(nodeBuilt)
				<< ", entirely: " << colored_text_from_bool(programReadEntirely) << "\n";

			std::cout << caesiumProgram << "\n\n";
			Grammarizer g2(tokens);
			while (g2.it != g.it) {
				std::cout << g2.it->second << " ";
				++g2.it;
			}
			std::cout << "\n";
			return std::nullopt;
		}
	}
	NodeStructs::File f = getStruct(file, "no_name_test_file");
	return transpile(std::vector{ std::move(f) });
}

bool test_transpile_no_error(int line, std::string_view caesiumProgram, std::string_view expected_header, std::string_view expected_cpp) {
	auto opt = create_file(line, caesiumProgram);
	if (!opt.has_value())
		return false;
	auto x = opt.value();

	if (x.has_value()) {
		auto [header_, cpp] = std::move(x).value();

		static constexpr size_t L = std::string_view{ default_includes }.length();
		int u = L;
		std::string_view header{ header_.begin() + L, header_.begin() + header_.size() };

		auto first_diff_header = first_diff(header, expected_header);
		bool header_ok = header.size() == expected_header.size() && header.size() == first_diff_header;

		auto first_diff_cpp = first_diff(cpp, expected_cpp);
		bool cpp_ok = cpp.size() == expected_cpp.size() && cpp.size() == first_diff_cpp;


		bool ok = header_ok && cpp_ok;
		if (!ok) {
			std::cout << "LINE " << line << (line < 100 ? " : " : ": ") << "transpiled: " << colored_text_from_bool(ok) << "\n";
			std::cout << colored_text("input:\n", output_stream_colors::blue) << caesiumProgram << "\n\n";
		}
		if (!header_ok) {
			std::cout << colored_text("\nexpected header:\n", output_stream_colors::blue) << expected_header << "\n\n";
			std::cout << colored_text("produced header:\n", output_stream_colors::blue) << header << "\n\n";
		}
		if (!cpp_ok) {
			std::cout << colored_text("expected cpp:\n", output_stream_colors::blue) << expected_cpp << "\n\n";
			std::cout << colored_text("produced cpp:\n", output_stream_colors::blue) << cpp << "\n\n";
		}
		return ok;
	}
	else {
		auto err = std::move(x).error().content;
		std::cout << "LINE " << line << (line < 100 ? " : " : ": ") << "transpiled: " << colored_text_from_bool(false) << "\n";
		std::cout << colored_text("input:\n", output_stream_colors::blue) << caesiumProgram << "\n\n";
		std::cout << colored_text("expected header:\n", output_stream_colors::blue) << expected_header << "\n\n";
		std::cout << colored_text("expected cpp:\n", output_stream_colors::blue) << expected_cpp << "\n\n";
		std::cout << colored_text("produced error:\n", output_stream_colors::blue) << colored_text_with_bool(err, false) << "\n\n";
		return false;
	}
}

bool test_transpile_error(int line, std::string_view caesiumProgram, std::string_view expected_error) {
	auto opt = create_file(line, caesiumProgram);
	if (!opt.has_value())
		return false;
	auto x = opt.value();

	if (x.has_value()) {
		throw;
	}
	else {
		auto error = std::move(x).error().content;

		auto first_diff_error = first_diff(error, expected_error);
		bool error_ok = error.size() == expected_error.size() && error.size() == first_diff_error;
		if (!error_ok) {
			std::cout << "LINE " << line << (line < 100 ? " : " : ": ") << "transpiled: " << colored_text_from_bool(false) << "\n";
			std::cout << colored_text("input:\n", output_stream_colors::blue) << caesiumProgram << "\n\n";
			std::cout << colored_text("expected error:\n", output_stream_colors::blue) << expected_error << "\n\n";
			std::cout << colored_text("produced error:\n", output_stream_colors::blue) << colored_text_with_bool(error, false) << "\n\n";
		}
		return error_ok;
	}
}

std::string include_header = "#include \"header.h\"\n";

auto add_to_main_cpp = [](std::string s) {
	return "struct Main {\n"
		"Int main(Vector<String> s) {\n"
		+ s +
		"};\n"
		"};\n"
		"\n"
		"int main(int argc, char** argv) {\n"
		"	std::vector<std::string> args {};\n"
		"	for (int i = 0; i < argc; ++i)\n"
		"		args.push_back(std::string(argv[i]));\n"
		"	return Main{}.main(std::move(args));\n"
		"};\n";
};

struct test_transpile_no_error_t {
	int line;
	std::string_view caesium;
	std::string_view header;
	std::string_view cpp;

	operator bool() {
		return test_transpile_no_error(line, caesium, header, cpp);
	}
};

struct test_transpile_error_t {
	int line;
	std::string_view caesium;
	std::string_view error;

	operator bool() {
		return test_transpile_error(line, caesium, error);
	}
};

bool testTranspile() {
	std::cout << "TRANSPILE TESTS\n";
	bool ok = true;
	ok &= test_transpile_no_error_t{
		.line = __LINE__,
		.caesium = "Int main(Vector<String> ref s):\n",
		.header = "",
		.cpp = include_header + add_to_main_cpp("")
	};
	ok &= test_transpile_error_t{
		.line = __LINE__,
		.caesium = "Int main(Vector<String> ref s):\n\ta\n",
		.error = "Undeclared variable `a`"
	};
	ok &= test_transpile_no_error_t{
		.line = __LINE__,
		.caesium = "Int main(Vector<String> ref s):\n\tInt a = {}\n",
		.header = "",
		.cpp = include_header + add_to_main_cpp("Int a = {};\n")
	};
	ok &= test_transpile_no_error_t{
		.line = __LINE__,
		.caesium = "type A:\n"
		"Int main(Vector<String> ref s):\n"
		"	A a = {}\n",
		.header = "struct A;\n",
		.cpp = include_header + "struct A {\n};\n\n" + add_to_main_cpp("A a = {};\n")
	};
	ok &= test_transpile_no_error_t{
		.line = __LINE__,
		.caesium = "type A:\n"
		"Int main(Vector<String> ref s):\n"
		"	A a1 = {}\n"
		"	A a2 = a1\n"
		"	a1 = a2\n",
		.header = "struct A;\n",
		.cpp = include_header + "struct A {\n};\n\n" + add_to_main_cpp("A a1 = {};\nA a2 = a1;\na1 = a2;\n")
	};
	ok &= test_transpile_no_error_t{
		.line = __LINE__,
		.caesium =
		"type A:\n"
		"type B:\n"
		"Int main(Vector<String> ref s):\n"
		"	A | B var = {}\n"
		,.header =
		"struct A;\n"
		"struct B;\n"
		,.cpp =
		include_header +
		"struct A {\n};\n\n" +
		"struct B {\n};\n\n" +
		add_to_main_cpp(
			"std::variant<A, B> var = {};\n"
		)
	};
	ok &= test_transpile_no_error_t{
		.line = __LINE__,
		.caesium =
		"type A:\n"
		"type B:\n"
		"Int main(Vector<String> ref s):\n"
		"	A | B v1 = {}\n"
		"	B | A v2 = v1\n"
		,.header =
		"struct A;\n"
		"struct B;\n"
		,.cpp =
		include_header +
		"struct A {\n};\n\n" +
		"struct B {\n};\n\n" +
		add_to_main_cpp(
			"std::variant<A, B> v1 = {};\n"
			"std::variant<A, B> v2 = v1;\n"
		)
	};
	ok &= test_transpile_no_error_t{
		.line = __LINE__,
		.caesium =
		"type A:\n"
		"type B:\n"
		"type C:\n"
		"Int main(Vector<String> ref s):\n"
		"	A | B v1 = {}\n"
		"	B | A | C v2 = v1\n"
		,.header =
		"struct A;\n"
		"struct B;\n"
		"struct C;\n"
		,.cpp =
		include_header +
		"struct A {\n};\n\n" +
		"struct B {\n};\n\n" +
		"struct C {\n};\n\n" +
		add_to_main_cpp(
			"std::variant<A, B> v1 = {};\n"
			"std::variant<A, B, C> v2 = v1;\n"
		)
	};

	return ok;
}
