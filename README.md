# Caesium

A programming language based on modern C++ concepts which integrates library elements into the language.

## C++ STL elements integrated as Caesium Language Elements
- [optional](https://en.cppreference.com/w/cpp/utility/optional)
- [variant](https://en.cppreference.com/w/cpp/utility/variant) & [visit](https://en.cppreference.com/w/cpp/utility/variant/visit)
- [expected](https://en.cppreference.com/w/cpp/utility/expected)
- 
- [ranges](https://en.cppreference.com/w/cpp/ranges)
- [transform](https://en.cppreference.com/w/cpp/ranges/transform_view)
- [filter](https://en.cppreference.com/w/cpp/ranges/filter_view)
- 
- [vector](https://en.cppreference.com/w/cpp/container/vector)
- [set](https://en.cppreference.com/w/cpp/container/unordered_set)
- [map](https://en.cppreference.com/w/cpp/container/unordered_map)

## Removing Branches From Generated Code

Typical programming patterns with expected, optionals and variants involve duplicate branching.
The following examples apply only to branching code and in the event where these kinds of value are placed in a data structure, removal of branching is not possible.

### Optional, Expected and Booleans

For example, using optional patterns such as `std::optional<some_type> it = find_if(collection, predicate); if (it.has_value()) { ... } else { ... }`.
This leads to a loss of information, as at the time of returning the optional from `find_if`, the value is known, but once placed in an optional the knowledge of whether or not the optional is empty is lost.
Caesium inlines the call to find and transpiles the `if-else` branches directly in place of the return statements of the call to `find_if`.

The same can be done with functions returning a boolean, such as `bool exists = find_if(collection, predicate); if (exists) { ... } else { ... }`.

### Variant

Similar examples can be found with `std::variant`, some may be extremely similar such as `std::variant<Empty, Value>` but even better are cases with a larger cardinality.
Let us use: `std::variant<Apple, Pear, Grape, Cat, Dog, Box> get_object_type(const std::string& str);` which may be called `auto object_t = get_object_type("Apple");` and then used `std::visit(overload(...), object_t);`.
This can be inlined by replacing the respective `if (str == "Apple") return Apple{};` with a call to the overload set directly such as `if (str == "Apple") overload_set(Apple{});`;

### Composed Cost

Typically, you may end up with types such as `std::expected<std::optional<std::variant<...>>>` which leads to excessive branching.
In the event where entire call hierarchies can be inlined, a choice will have to be made between branch removal and binary size.
