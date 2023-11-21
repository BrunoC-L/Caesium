#pragma once

#include <ranges>
#include <vector>
#include <iostream>

namespace std {
    namespace ranges {

        namespace detail_to {
            template <template <typename> typename Container>
            struct to_helper {};
        }

        template <template <typename> typename Container>
        detail_to::to_helper<Container> to() {
            return {};
        }

        template <template <typename> typename Container, std::ranges::range R>
        auto operator|(R&& r, detail_to::to_helper<Container>) {
            using T = std::ranges::range_value_t<R>;
            return Container<T>{ r.begin(), r.end() };
        }

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
