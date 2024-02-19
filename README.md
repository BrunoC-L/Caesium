# Caesium

A programming language based on modern C++ concepts which integrates library elements into the language.
The only design downside of Caesium is the impossibility of dynamic extension, such as plugins, as the compiler requires the entire source to apply its magic.

## C++ STL elements integrated as Caesium Language Elements

- union types
  - [optional](https://en.cppreference.com/w/cpp/utility/optional) & `optional.transform(...)`
  - [variant](https://en.cppreference.com/w/cpp/utility/variant) & [visit](https://en.cppreference.com/w/cpp/utility/variant/visit)
  - [expected](https://en.cppreference.com/w/cpp/utility/expected) & `expected.transform(...)`,  `expected.transform_error(...)`

- functional concepts
  - [ranges](https://en.cppreference.com/w/cpp/ranges)
  - [transform](https://en.cppreference.com/w/cpp/ranges/transform_view)
  - [filter](https://en.cppreference.com/w/cpp/ranges/filter_view)

- data structures
  - [vector](https://en.cppreference.com/w/cpp/container/vector)
  - [set](https://en.cppreference.com/w/cpp/container/unordered_set)
  - [map](https://en.cppreference.com/w/cpp/container/unordered_map)

- values
  - [move](https://en.cppreference.com/w/cpp/utility/move)
  - [references as values](https://en.cppreference.com/w/cpp/utility/functional/reference_wrapper)
  - [unique_ptr*](https://en.cppreference.com/w/cpp/memory/unique_ptr)
    - * Uniqueness is associated to a type by disabling copy, while pointers are implemented with Boxing.

## Removing Branches From Generated Code

Typical programming patterns with expected, optionals and variants involve duplicate branching.
The following examples apply only to branching code and in the event where these kinds of value are placed in a data structure, removal of branching is not possible.

### Binary Branches

In C++, using optional patterns such as `std::optional<some_type> it = find_if(collection, predicate); if (it.has_value()) { ... } else { ... }` cause branching twice.
This is due to a loss of information.
At the time of returning the optional from `find_if`, the presence is known, but, once placed in an optional, the knowledge of presence is lost.
Caesium inlines the call to `find_if` and transpiles the `if-else` branches directly in place of the return statements of the call to `find_if`.

The same can be done with functions returning a boolean, such as `bool exists = find_if(collection, predicate); if (exists) { ... } else { ... }`.

### Variants, Pattern Matching

Similar examples can be found with `std::variant`, some may be extremely similar such as `std::variant<Empty, Value>` but even better are cases with a larger cardinality.
Let us use: `std::variant<Apple, Pear, Grape, Cat, Dog, Box> get_object_type(const std::string& str);` which may be called `auto object_t = get_object_type("Apple");` and then used `std::visit(overload(...), object_t);`.
This can be inlined by replacing the respective `if (str == "Apple") return Apple{};` with a call to the overload set directly such as `if (str == "Apple") overload_set(Apple{});`;

### Composed Cost

Typically, you may end up with types such as `std::expected<std::optional<std::variant<...>>>` which leads to excessive branching.
In the event where entire call hierarchies can be inlined, a compromise will be made between branch removal and binary size.

## Types

Caesium is a types first language.
In order to write objectively correct and reusable code, the caesium approach is to completely dissociate types from functions.

Types may only hold data members and type aliases, also known as type traits (in C++, `using value_type = T;`)
This means no member functions, constructors or destructors.

Maintaining type invariants should be done in functions creating objects of the given type, writers are to assume they know the invariants the type should hold when creating it directly.
Types are instanciated using a construction operator `Type{ arguments... }`, as opposed to a function call `make_my_type( arguments... )`.

To associate functions to types, for usage such as `house.is_inhabited_by(person)`, define a function `is_inhabited_by` with a `House` and a `Person` parameter, which will allow the `is_inhabited_by` function to be called like a member function of `House`.

## Functions

Functions in Caesium are a different than other languages, as they require more input from the programmer to describe the argument passing behaviour.
Specifically, this is in hopes of completely getting rid of accidental copying and correctly overload for references and values.
Parameters are described using either reference `ref`, mutable reference `ref!` or value `val`, for example: `Void f(String ref name): ...`.
Most function parameters should be values, the need for mutability or reference should be reserved for parameters which are expected to live in a data structure.
Arguments are described using either `ref`, `ref!`, `copy` or `move`, for example: `String name = "Caesium"` can be used like `f(ref! name)`.

Const variables may not be mutably referenced, but may be moved.
This is unlike C++ where const disables move.
copy and move arguments only bind to value parameters, while reference and mutable reference arguments bind only to reference and mutable reference parameters.
For metaprogramming approach where values or reference are acceptable, it is possible to accept `ref | val` parameters, but callers may not use `ref | copy` arguments.

## Templates

Templates in Caesium are similar to C++, but not as much of a part of the type system.
In C++, it is possible to achieve aggressive [CTAD](https://en.cppreference.com/w/cpp/language/class_template_argument_deduction), because templates are just restrictive enough to apply deduction.
In Caesium, templates are black boxes until instantiated, this allows for more powerful metaprogramming techniques, but less interoperability with other language features.

Specifically, templates are defined with a name and a list of parameters, which can then be specialized [similarly to C++](https://en.cppreference.com/w/cpp/language/partial_specialization).
The content of each instance does not have to match.
For example, `template f<T>` may hold a type, while `template f<T=Void>` may hold a function.
It gets even more complicated, as no constraints are applied to template arguments.
Note that these features are great for making things very complicated and should not be part of a public API.

Typically, C++ template arguments are types, but C++ also supports non type template parameters for constexpr constructible types.
Caesium accepts, types, expressions, string literals, numbers and arbitrary code in backticks.
This allows for very simple policy based design ([watch this, for example at 18:00](https://www.youtube.com/watch?v=HdzwvY8Mo-w&t=3564s&ab_channel=TheDLanguageFoundation)).
Expression templates can be used for optional variable declaration, which is not possible in C++.
Variable declarations are one example, but class member declarations are another example of an otherwise impossible task in most languages.

## References, Views, Lifetimes

Similarly to Rust, Caesium allows for references if the compiler can determine the lifetime of the referenced object to be greater than that of its reference.
Unlike C++, types with reference member variables may be copied, these are also known as views in C++ terminology, which are typically passed by value.

## Long Term View on Parallelism

Caesium will never allow for user crafted data structures, other than by composing existing data structures.
All the code storing or acessing data at runtime can be deduced at compile time, leading to interesting approaches to safety of parallel execution and concurrent access of data.
Caesium will offer parallel concurent data structures and algorithms which can be validated at compile time.
Code with synchronization overhead will never be generated unless specified, but the compiler will not allow for unsafe code to be generated.
In the event where the programmer allows for overhead and the compiler detects no need for synchronization, a warning will be emitted and the no overhead version will be generated.
