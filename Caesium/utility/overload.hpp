#pragma once
#include <sstream>
#include <stacktrace>
#include <source_location>

template<typename... Ts> struct overload : Ts... { using Ts::operator()...; };
template<typename... Ts> overload(Ts...) -> overload<Ts...>; // to help IDE

// https://stackoverflow.com/a/24000041/10945691
namespace detail {
	template<unsigned... digits>
	struct to_chars { static const char value[]; };

	template<unsigned... digits>
	constexpr char to_chars<digits...>::value[] = { ('0' + digits)..., 0 };

	template<unsigned rem, unsigned... digits>
	struct explode : explode<rem / 10, rem % 10, digits...> {};

	template<unsigned... digits>
	struct explode<0, digits...> : to_chars<digits...> {};
}

template<unsigned num>
struct num_to_string : detail::explode<num> {};

// std::string{ "Overload set is missing support for a type held in the variant. see "};
// + std::string{ std::source_location::current().file_name() } + std::string{ ":" } + std::to_string(std::source_location::current().line())
	// + std::string{ " for instantiation of " } + std::string{ std::source_location::current().function_name() }

struct constexpr_buffer {
	char* data;
	size_t _size;
	constexpr size_t size() {
		return _size;
	}
};

/*
	static constexpr std::string m1 = "Overload set is missing support for a type held in the variant. see ";\
	static constexpr std::string m2 = m1 + std::source_location::current().file_name();\
	static constexpr std::string m3 = m2 + ":";\
	static constexpr std::string m4 = m3 + num_to_string<std::source_location::current().line()>::value;\
	static constexpr std::string m5 = m4 + " during instantiation of ";\
	static constexpr std::string m6 = m5 + std::source_location::current().function_name();\
	constexpr constexpr_buffer x{ m1.c_str(), m1.size() }; \
*/

#ifdef __GNUC__
	#define overload_default_error [&](const auto& e) {\
	static_assert(false, std::string{ std::source_location::current().function_name() });\
	}
#endif

#ifdef _MSC_VER

	#if __cpp_static_assert == 202306L
		error, update msvc overload_default_error to use above solution
	#endif

	#define overload_default_error [&](const auto& e) {\
	static_assert(\
		!sizeof(std::remove_cvref_t<decltype(e)>*),\
		"Overload set is missing support for a type held in the variant.");\
	}

#endif

#if !defined(_MSC_VER) && !defined(__GNUC__)
error
#endif
