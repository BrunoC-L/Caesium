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
A a() {
if (false) return A{};
while (true) {
Set<A> b = {};
Bool c = false;
Bool d = True;
for (auto&& [a] : b) {
if (d) break;
}
for (auto&& [a] : b) {
return ([&] () { if (c) return a; else return b; }());
}
a=([&] () { if (e) return f; else return a; }());
if (b == b) return A();
}};
struct A {
A(C c) {
return a;
};
A(const A&) = default;
A& operator=(const A&) = default;
A(A&&) = default;
A& operator=(A&&) = default;
~A() = default;
};

Int main(Vector<String> args) {
return 0;
};
};
int main(int argc, char** argv) {
std::vector<std::string> args {};
for (int i = 0; i < argc; ++i)
    args.push_back(std::string(argv[i]));return Main{}.main(std::move(args));
};
