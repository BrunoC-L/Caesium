#include <utility>
#include <iostream>
using Int = int;
using Bool = bool;
using Void = void;
using Floating = double;
template <typename First, typename Second> using Pair = std::pair<First, Second>;
#include <variant>
template <typename... Ts> using Variant = std::variant<Ts...>;
#include <vector>
template <typename T> using Vector = std::vector<T>;
#include <string>
using String = std::string;
#include <unordered_set>
template <typename T> using Set = std::unordered_set<T>;
#include <set>
template <typename T> using TreeSet = std::set<T>;
#include <unordered_map>
template <typename K, typename V> using Map = std::unordered_map<K, V>;
static constexpr bool True = true;
static constexpr bool False = false;
template<typename... Ts> struct overload : Ts... { using Ts::operator()...; };
Void push(auto&& vec, auto&& e) { vec.push_back(e); }

Int _redirect_main(const Vector<String>& args);
Bool test_transpile_all_folders(const std__filesystem__directory_iterator& folder);
struct directory_iterator;
Bool test_parse();
