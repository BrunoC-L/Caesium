#include "defaults.hpp"



Bool test_parse();
Bool test_transpile_all_folders(const Vector_union__builtin_filesystem_file_builtin_filesystem_directory_& folders);
Bool test_transpile_folder(const builtin_filesystem_directory& dir);
using Vector_String_ = Vector<String>;
using Vector_union__builtin_filesystem_file_builtin_filesystem_directory_ = Vector<union__builtin_filesystem_file_builtin_filesystem_directory>;
Int _redirect_main(const Vector_String_& args);

Bool test_parse() {
	return False;
};
Bool test_transpile_all_folders(const Vector_union__builtin_filesystem_file_builtin_filesystem_directory_& folders) {
	for (auto&& entry : folders) {
		const Union<builtin_filesystem_directory, builtin_filesystem_file>& matchval4 = entry;
		if (std::holds_alternative<builtin_filesystem_directory>(matchval4)) {
			const builtin_filesystem_directory& dir = std::get<builtin_filesystem_directory>(matchval4);
			return test_transpile_folder(dir);
		} else
		if (std::holds_alternative<builtin_filesystem_file>(matchval4)) {
			const builtin_filesystem_file& f = std::get<builtin_filesystem_file>(matchval4);
		} else
			throw;
	}
	return True;
};
Bool test_transpile_folder(const builtin_filesystem_directory& dir) {
	return False;
};
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
	if (!test_transpile_all_folders(filesystem__entries(String{"C:/"}))) {
		return bad_exit;
	}
	(Void)(std::cout << String{"All transpile tests passed"} << "\n");
	return 0;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
