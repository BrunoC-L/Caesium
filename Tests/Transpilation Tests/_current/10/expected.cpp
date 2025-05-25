#include "defaults.hpp"

struct A;
struct B;

struct A {
};

struct B {
};


using Union_A_B_ = Union<A, B>;
using Vector_String_ = Vector<String>;
Int _redirect_main(const Vector_String_& s);

Int _redirect_main(const Vector_String_& s) {
	Union_A_B_ union_A_B_A = { A{} }; 
	Union_A_B_ union_B_A_A = { A{} }; 
	Union_A_B_ union_A_B_B = { B{} }; 
	Union_A_B_ union_B_A_B = { B{} }; 
	Union_A_B_ or_A_B_A = { A{} }; 
	Union_A_B_ or_B_A_A = { A{} }; 
	Union_A_B_ or_A_B_B = { B{} }; 
	Union_A_B_ or_B_A_B = { B{} }; 
}

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
}
