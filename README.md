# Caesium

A programming based on modern C++ concepts which integrates library elements into the language.

Notable Characteristics Include:
- Type Safety
- Metaprogramming
- Const Correctness Emphasis
- No Implicit Copies
- No Null Pointers
- No Exceptions

## C++ STL elements integrated as Caesium Language Elements
- [optional](https://en.cppreference.com/w/cpp/utility/optional)
- [ranges](https://en.cppreference.com/w/cpp/ranges), [transform](https://en.cppreference.com/w/cpp/ranges/transform_view), [filter](https://en.cppreference.com/w/cpp/ranges/filter_view), ...
- [variant](https://en.cppreference.com/w/cpp/utility/variant)
- [tuple](https://en.cppreference.com/w/cpp/utility/tuple)
- [vector](https://en.cppreference.com/w/cpp/container/vector)
- [set](https://en.cppreference.com/w/cpp/container/unordered_set)
- [map](https://en.cppreference.com/w/cpp/container/unordered_map)

## Removing Branches From Generated Code

Typical programming patterns with variants and optionals involve duplicte conditions.
For example, using optional patterns such as `std::optional<some_type> it = find_if(collection, predicate); if (it.has_value()) { ... } else { ... }`.
This leads to a loss of information, as at the time of returning the optional from `find_if`, the value is known, but once placed in an optional the knowledge of whether or not the optional is empty is lost.
Caesium inlines the call to find and transpiles the `if-else` branches directly in place of the return statements of the call to `find_if`.
