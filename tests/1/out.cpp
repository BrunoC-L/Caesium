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

struct Main {
void f() {
List<Int> l = {};
l.push(3);
Int a = l.pop();
};
struct A {
using C = B;
};

struct B {
using C = B;
};

struct T {
A<B> a;
B::C b;
A<B>::C c;
A<B::C> d;
};

struct F {
};

struct E {
using G = A;
using T = F;
};

Int main(Vector<String> args) {
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
};
int main(int argc, char** argv) {
std::vector<std::string> args {};
for (int i = 0; i < argc; ++i)
    args.push_back(std::string(argv[i]));return Main{}.main(std::move(args));
};
