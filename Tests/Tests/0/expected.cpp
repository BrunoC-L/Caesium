#include "defaults.hpp"



using Vector_String_ = Vector<String>;
struct T {
    Int a;
}
Int _redirect_main(const Vector_String_& args);

Int _redirect_main(const Vector_String_& args) {
    T t = { 3 };
    return t.a;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
