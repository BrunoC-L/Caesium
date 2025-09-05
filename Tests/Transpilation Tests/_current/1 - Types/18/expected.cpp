#include "defaults.hpp"

struct Cat;
struct Dog;
struct CatsAndDogs;

struct Cat {
	String name;
};

struct Dog {
	String name;
};

struct CatsAndDogs {
	Vector_Cat_ cats;
	Vector_Dog_ dogs;
};


using Union_filesystem__directory_or_filesystem__file_ = Union<filesystem__directory, filesystem__file>;
using Vector_Cat_ = Vector<Cat>;
using Vector_Dog_ = Vector<Dog>;
using Vector_String_ = Vector<String>;
CatsAndDogs add(CatsAndDogs&& cats_and_dogs, Cat&& cat);
Int _redirect_main(const Vector_String_& s);

CatsAndDogs add(CatsAndDogs&& cats_and_dogs, Cat&& cat) {
	push(cats_and_dogs.cats, std::move(cat));
	return cats_and_dogs;
}
Int _redirect_main(const Vector_String_& s) {
	CatsAndDogs a = CatsAndDogs{Vector_Cat_{}, Vector_Dog_{}};
	CatsAndDogs b = add(std::move(a), Cat{String{"cat1"}});
	CatsAndDogs c = add(std::move(b), Cat{String{"cat2"}});
	CatsAndDogs d = add(std::move(c), Cat{String{"cat3"}});
	return 0;
}

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
}
