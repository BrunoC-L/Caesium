#pragma once
#include <ostream>
#include <sstream>

namespace output_stream_colors {
	constexpr auto black  = "\033[0;30m";
	constexpr auto red    = "\033[0;31m";
	constexpr auto green  = "\033[0;32m";
	constexpr auto yellow = "\033[0;33m";
	constexpr auto blue   = "\033[0;34m";
	constexpr auto purple = "\033[0;35m";
	constexpr auto cyan   = "\033[0;36m";
	constexpr auto white  = "\033[0;37m";
	constexpr auto reset = "\033[0m";
}

std::string colored_text(auto&& text, auto&& color) {
	std::stringstream ss;
	ss << color << text << output_stream_colors::reset;
	return std::move(ss).str();
}

std::string colored_text(bool b, auto&& color) {
	return colored_text(b ? "true" : "false", std::forward<decltype(color)>(color));
}

std::string colored_text_with_bool(auto&& text, bool b) {
	return colored_text(text, b ? output_stream_colors::green : output_stream_colors::red);
}

std::string colored_text_from_bool(bool b) {
	return colored_text_with_bool(b, b);
}
