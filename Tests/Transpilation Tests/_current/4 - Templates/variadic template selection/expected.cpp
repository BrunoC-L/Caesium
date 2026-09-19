#include "defaults.hpp"

struct A_Int_;
struct A_Int_String_;
struct A_Int_String_Floating_;

struct A_Int_ {
	Int member_0_A;
};

struct A_Int_String_ {
	Int member_0_B;
	String member_1_B;
};

struct A_Int_String_Floating_ {
	Int member_0_C;
	String member_1_C;
	Floating member_2_C;
};


using Union_filesystem__directory_or_filesystem__file_ = Union<filesystem__directory, filesystem__file>;
using Vector_String_ = Vector<String>;
Int _redirect_main(const Vector_String_& args);

Int _redirect_main(const Vector_String_& args) {
	A_Int_ a = A_Int_{1};
	A_Int_String_ b = A_Int_String_{1, String{"hello"}};
	A_Int_String_Floating_ c = A_Int_String_Floating_{1, String{"hello"}, 3};
}

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
}
