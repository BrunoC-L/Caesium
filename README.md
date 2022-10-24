# CurziPlusPlus

Transpiler To Write C++ Faster using python-like syntax/grammar

A language that eases some aspects of writing c++, while maintaining type safety, though it limits what is possible by a fair amount.

The produced code is not meant to be used to interface with, just to run.

## Syntax
```
Python-like:
    indented:
        grammar
```
## Types

Type A:
    i64 id
    String name

Type B:
    i64 id
    String firstName
    String lastName

A & B behave like structs at first glance, but are transpiled to class with getters

Type C = A | B -> `using C = std::variant<A, B>`

the use of variant is inside the language using `|`

Classes are a subset of types, so all classes are types but not all types are classes, but in the end they are all transpiled to C++ classes.

Classes can extend classes or types

Class D extends B:
    i64 id(): return ...
    String firstName(): return ...
    String lastName(): return ...

methods act like `std::function` so you can do this for example

void subscribe(()->void callback): ...
subscribe(object.method)

but under the hood what happens is `subscribe(object.method)` becomes `subscribe([&object](){object.method();})`

## Allocation and referencing

All object instantiations are done through `std::make_shared`
All classes inherit from base class `Object` which inherits from `std::enable_shared_from_this<Object>`
All method calls either pass a `shared_ptr` by value or reference
Calls from inside the class make use of `shared_from_this` to get a `shared_ptr` to themselves
Most functions only require a reference so passing a reference to an object or a `shared_ptr` makes little difference in performance, but calling `shared_from_this` will still take a little time
Every method requiring at least one use of `shared_from_this` will have the call made at the start of the method and the result stored for further use, avoiding multiple calls being made

variable instantiation hides shared pointers
T t = T(...)
const T t = T(...)
const T t = copy u.get_t()

functions calls also hide shared pointers

| Meaning           | Function Definition | Function Call | C++ representation                |
|-------------------|---------------------|---------------|-----------------------------------|
| Const Reference   | f(T t)              | f(t)          | const std::shared_ptr<const T>& t |
| Reference         | f(T! t)             | f(!t)         | std::shared_ptr<const T>& t       |
| Copy              | f(copy T t)         | f(copy t)     | t->clone()                        |

Essentially the definition is like c++ with either `const T&`, `T&` or `T`, except you also need to explicitely write `copy` or `!`

C++ Lambda captures can be a pain, for example, all by reference captures `[&]` lead to various problems.

Lambda captures will capture variables individually, no default capture mode like `[=]` or `[&]`.

lambdas are declared like in typescript: `(T t) => ...` except they also require a capture (can omit if empty) so `[!t](...) => ...`
