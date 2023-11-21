#include "header.h"
struct Dog {
};

struct Cat {
};

Vector<Cat> cats() {
};
Vector<Dog> dogs() {
};
struct Main {
Int main(Vector<String> args) {
println(1);
return 7;
};
};
int main(int argc, char** argv) {
std::vector<std::string> args {};
for (int i = 0; i < argc; ++i)
    args.push_back(std::string(argv[i]));
return Main{}.main(std::move(args));
};
