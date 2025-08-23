#include "defaults.hpp"



using Union_filesystem__directory_or_filesystem__file_ = Union<filesystem__directory, filesystem__file>;
using Vector_String_ = Vector<String>;
using Vector_Union_filesystem__directory_or_filesystem__file__ = Vector<Union_filesystem__directory_or_filesystem__file_>;
Bool test_parse();
Vector_Union_filesystem__directory_or_filesystem__file__ entries(const String& directory_path);
Bool test_transpile_folder(const filesystem__directory& dir);
Bool test_transpile_all_folders(const Vector_Union_filesystem__directory_or_filesystem__file__& folders);
Int _redirect_main(const Vector_String_& args);

Bool test_parse() {
	return False;
}
Vector_Union_filesystem__directory_or_filesystem__file__ entries(const String& directory_path) {
}
Bool test_transpile_folder(const filesystem__directory& dir) {
	return False;
}
Bool test_transpile_all_folders(const Vector_Union_filesystem__directory_or_filesystem__file__& folders) {
	for (auto&& entry : folders) {
		const Union_filesystem__directory_or_filesystem__file_& matchval1 = entry;
		if (std::holds_alternative<filesystem__directory>(matchval1)) {
			const filesystem__directory& dir = std::get<filesystem__directory>(matchval1);
			return test_transpile_folder(dir);
		} else
		if (std::holds_alternative<filesystem__file>(matchval1)) {
			const filesystem__file& f = std::get<filesystem__file>(matchval1);
		} else
			NOT_IMPLEMENTED;
	}
	return True;
}
Int _redirect_main(const Vector_String_& args) {
	Int argc = args.size();
	Int bad_exit = 1;
	if (argc == 1) {
		(Void)(std::cout << String{"Expected a test folder"} << "\n");
		return bad_exit;
	}
	if (argc > 2) {
		(Void)(std::cout << String{"Expected a single argument"} << "\n");
		return bad_exit;
	}
	if (!test_parse()) {
		return bad_exit;
	}
	(Void)(std::cout << String{"All parse tests passed"} << "\n");
	if (!test_transpile_all_folders(entries(String{"C:/"}))) {
		return bad_exit;
	}
	(Void)(std::cout << String{"All transpile tests passed"} << "\n");
	return 0;
}

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
}
