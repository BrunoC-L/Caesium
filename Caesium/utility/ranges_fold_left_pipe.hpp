#pragma once

#include <ranges>
#include <vector>
#include <iostream>

namespace std {
    namespace ranges {
        template <typename F>
        struct fold_left_ {
            F f;
        };

        template <typename F, std::ranges::range R>
        auto operator|(R&& r, std::ranges::fold_left_<F>&& f) {
            return std::ranges::fold_left_first(std::forward<R>(r), std::move(f.f));
        }
    }
}
