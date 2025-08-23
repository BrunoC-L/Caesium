#pragma once
#include <sstream>
#include <stacktrace>
#include <source_location>
#include "compiler.hpp"

template<typename... Ts> struct overload : Ts... { using Ts::operator()...; };
template<typename... Ts> overload(Ts...) -> overload<Ts...>; // to deduce members as inheritance parents?

// the following is a macro the provide a simpler error to the user
// when they are missing an option in their overload set

#if defined(msvc)
#if __cpp_static_assert == 202306L
error, update msvc overload_default_error to use better message
#endif

#define overload_default_error [&](const auto& e) {\
	static_assert(\
		!sizeof(std::remove_cvref_t<decltype(e)>*),\
		"Overload set is missing support for a type held in the variant.");\
	}
#else

	// need constexpr std::format to use this... can't make use of std::string like this at compile time for now
	// std::string{ "Overload set is missing support for a type held in the variant. see "};
	// + std::string{ std::source_location::current().file_name() } + std::string{ ":" } + std::to_string(std::source_location::current().line())
	// + std::string{ " for instantiation of " } + std::string{ std::source_location::current().function_name() }

#define overload_default_error [&](const auto& e) {\
		static_assert(false, std::string{ std::source_location::current().function_name() });\
	}
#endif
