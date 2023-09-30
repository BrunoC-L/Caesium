#include <utility>
using Int = int;
using Bool = bool;
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

struct A {
A() {
Set<Pair<Int, Int>> a = {};
Bool b = false;
Bool c = true;
for (auto&& [i, e] : a) {
if (b) return ;
if (c) break;
}
};;
A(const A&) = default;
A& operator=(const A&) = default;
A(A&&) = default;
A& operator=(A&&) = default;
~A() = default;
};Int main() {
return 0;
};