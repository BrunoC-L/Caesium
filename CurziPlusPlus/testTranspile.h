//#pragma once
//#include <iostream>
//#include "toCpp.h"
//#include "structurizer.h"
//
//void testTranspile(int line, std::string caesiumProgram, std::string cppProgram) {
//
//	constexpr auto green = "\033[1;32m";
//	constexpr auto red = "\033[1;31m";
//	constexpr auto reset = "\033[0m";
//
//	std::forward_list<TOKENVALUE> tokens(Tokenizer(caesiumProgram).read());
//	Grammarizer g(tokens);
//	auto file = File(0);
//	{
//		bool nodeBuilt = file._value.build(&g);
//		bool programReadEntirely = g.it == g.tokens.end();
//		while (!programReadEntirely && (g.it->first == NEWLINE || g.it->first == END))
//			programReadEntirely = ++g.it == g.tokens.end();
//		auto nodeBuiltColor = nodeBuilt ? green : red;
//		auto programReadEntirelyColor = programReadEntirely ? green : red;
//		std::cout << "LINE " << line << (line < 100 ? " : " : ": ")
//			<< "built: " << nodeBuiltColor << nodeBuilt << reset
//			<< ", entirely: " << programReadEntirelyColor << programReadEntirely << reset << "\n";
//		bool res = nodeBuilt && programReadEntirely;
//		if (!res) {
//			std::cout << caesiumProgram << "\n\n";
//			Grammarizer g2(tokens);
//			while (g2.it != g.it) {
//				std::cout << g2.it->second << " ";
//				++g2.it;
//			}
//			std::cout << "\n";
//			return;
//		}
//	}
//	{
//		const NodeStructs::File& f = getStruct(file);
//		std::stringstream h;
//		std::stringstream cpp;
//		toCPP{}.transpile(h, cpp, f);
//		auto cppProduced = h.str();
//		std::string_view withoutIncludes{ cppProduced.begin() + toCPP::default_includes.length(), cppProduced.begin() + cppProduced.size() };
//		withoutIncludes.remove_prefix(std::min(withoutIncludes.find_first_not_of('\n'), withoutIncludes.size()));
//		withoutIncludes.remove_suffix(withoutIncludes.size() - std::max(withoutIncludes.find_last_not_of('\n'), size_t{ 0 }));
//		std::string_view withoutIncludesExpected = cppProgram;
//		withoutIncludesExpected.remove_prefix(std::min(withoutIncludesExpected.find_first_not_of('\n'), withoutIncludesExpected.size()));
//		withoutIncludesExpected.remove_suffix(withoutIncludesExpected.size() - std::max(withoutIncludesExpected.find_last_not_of('\n'), size_t{ 0 }));
//		bool transpiled = withoutIncludes == withoutIncludesExpected;
//		auto transpiledColor = transpiled ? green : red;
//		std::cout << "LINE " << line << (line < 100 ? " : " : ": ")
//			<< "transpiled: " << transpiledColor << transpiled << reset;
//		if (!transpiled) {
//			std::cout << "\ninput:" << caesiumProgram << "\n\n";
//			std::cout << "\ncreated:" << withoutIncludes << "\n\n";
//			std::cout << "\nexpected:" << withoutIncludesExpected << "\n\n";
//		}
//	}
//}
//void testTranspile() {
//	std::cout << "TRANSPILE TESTS\n";
//	testTranspile(__LINE__,
//		"type A:\n",
//		"struct A {\n};\n"
//	);
//}
