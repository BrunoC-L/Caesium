#pragma once

//////////////////////////////////////////////////////////////////////////
// modified original found on stackoverflow                             //
// https://stackoverflow.com/questions/58808030/range-view-to-stdvector //
//                                                                      //
// incorrect but good try:                                              //
// https://stackoverflow.com/users/923945/shaulf                        //
// https://godbolt.org/z/8bcn8qh8f                                      //
//////////////////////////////////////////////////////////////////////////

#include <ranges>
#include <vector>
#include <iostream>
namespace rng = std::ranges;

namespace detail {
    template <template <typename> typename Container>
    struct to_helper {};

    template <template <typename> typename Container, rng::range R>
    auto operator|(R&& r, to_helper<Container>) {
        using T = std::ranges::range_value_t<R>;
        return Container<T>{ r.begin(), r.end() };
    }
}
namespace std {
    namespace ranges {
        template <template <typename> typename Container>
        auto to() {
            return detail::to_helper<Container>{};
        }
    }
}