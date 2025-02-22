#pragma once
#include <ostream>
#include <sstream>

struct output_stream_colors {
#define self_t(name, expr) decltype(expr) name = expr
	constexpr static self_t(black, "\033[0;30m");
	constexpr static self_t(red, "\033[0;31m");
	constexpr static self_t(green, "\033[0;32m");
	constexpr static self_t(yellow, "\033[0;33m");
	constexpr static self_t(blue, "\033[0;34m");
	constexpr static self_t(purple, "\033[0;35m");
	constexpr static self_t(cyan, "\033[0;36m");
	constexpr static self_t(white, "\033[0;37m");
	constexpr static self_t(reset, "\033[0m");
#undef self_t
};

static std::string colored_text(auto&& text, auto&& color) {
	std::stringstream ss;
	ss << color << text << output_stream_colors::reset;
	return std::move(ss).str();
}

static std::string colored_text(bool b, auto&& color) {
	return colored_text(b ? "true" : "false", std::forward<decltype(color)>(color));
}

static std::string colored_text_with_bool(auto&& text, bool b) {
	return colored_text(text, b ? output_stream_colors::green : output_stream_colors::red);
}

static std::string colored_text_from_bool(bool b) {
	return colored_text_with_bool(b, b);
}
