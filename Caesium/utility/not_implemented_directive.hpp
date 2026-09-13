#pragma once
#include <iostream>
#include <sstream>
#include <stacktrace>

// throw makes the compiler happy to know functions dont go past NOT_IMPLEMENTED
// create the stack trace here and not inside not_implemented so that the last frame is where the macro is, not inside not_implemented
#define NOT_IMPLEMENTED                      (throw std::runtime_error(not_implemented(std::stacktrace::current())))
#define NOT_IMPLEMENTED_BUT_PROBABLY_ERROR   NOT_IMPLEMENTED
#define NOT_IMPLEMENTED_BUT_PROBABLY_CORRECT NOT_IMPLEMENTED

inline std::string not_implemented(const std::stacktrace& stacktrace) {
	std::stringstream ss;

	ss << "compiler error: encountered an unimplemented feature or unimplemented error handling at:\n" << stacktrace << "\n";
	auto str = ss.str();
	std::cerr << str;
	return str;
}
