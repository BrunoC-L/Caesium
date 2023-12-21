#pragma once

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>; // to help IDE

#define overload_default_error [&](const auto& e) {\
static_assert(\
	!sizeof(std::remove_cvref_t<decltype(e)>*),\
	"Overload set is missing support for a type held in the variant."\
	);\
/* requires P2741R3 user-generated static_assert messages
static_assert(
	false,
	std::format("Overload set is missing support for a type held in the variant. see {}.", std::source_location::current())
);*/\
}
