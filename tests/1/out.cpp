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
using C = B;
};struct B {
using C = B;
};struct T {
A<B> a;
B::C b;
A<B>::C c;
A<B::C> d;
};struct F {
};struct E {
using G = A;
using T = F;
};struct R {
using C = A<B>;
using K = F<B, C>;
E<K>::T<B, C> member1;
E<K>::G method1(K kC u, ) {
};;
};Int main() {
Set<Int> someContainer = {};
for (auto&& i : someContainer) {
}
Int u = {};
u=u * u;
u == u;
Vector<Int> arr = {};
for (auto&& i : arr) {
}
for (auto&& k : arr) {
++k;
k++;
--k;
k--;
k+=1;
k-=1;
k * 1;
k + 1;
k - 1;
1 % k;
}
Bool x = false;
Bool k = true;
Int y = -1;
Int u = 1;
if (k) return ([&] () { if (x) return y; else return u; }());
Map<Int, String> m = {};
for (auto&& [k, v] : m) {
if (k == 1) {
u=k + k;
} else {u=k - k;
}}
m.at(u);
};