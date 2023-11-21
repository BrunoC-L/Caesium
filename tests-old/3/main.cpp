#include "header.h"
struct A {
A() {
Set<Pair<Int, Int>> a = {};
Bool b = false;
Bool c = true;
for (auto&& [i, e] : a) {
if (b) return ;
if (c) break;
}
};
A(const A&) = default;
A& operator=(const A&) = default;
A(A&&) = default;
A& operator=(A&&) = default;
~A() = default;
};

struct Main {
Int main(Vector<String> args) {
return 0;
};
};
int main(int argc, char** argv) {
std::vector<std::string> args {};
for (int i = 0; i < argc; ++i)
    args.push_back(std::string(argv[i]));
return Main{}.main(std::move(args));
};
