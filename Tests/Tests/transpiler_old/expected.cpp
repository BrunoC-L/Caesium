#include "defaults.hpp"



Bool test_parse();
Bool test_transpile_all_folders(const Vector<Variant<builtin_filesystem_directory, builtin_filesystem_file>>& folders);
Bool test_transpile_folder(const builtin_filesystem_directory& dir);
Int _redirect_main(const Vector<String>& args);

Bool test_parse() {
	return False;
};
Bool test_transpile_all_folders(const Vector<Variant<builtin_filesystem_directory, builtin_filesystem_file>>& folders) {
	for (auto&& entry : folders) {
		const Variant<builtin_filesystem_directory, builtin_filesystem_file>& matchval2 = entry;
		if (std::holds_alternative<builtin_filesystem_directory>(matchval2)) {
			const builtin_filesystem_directory& dir = std::get<builtin_filesystem_directory>(matchval2);
			return test_transpile_folder(dir);
		} else
		if (std::holds_alternative<builtin_filesystem_file>(matchval2)) {
			const builtin_filesystem_file& f = std::get<builtin_filesystem_file>(matchval2);
		} else
			throw;
	}
	return True;
};
Bool test_transpile_folder(const builtin_filesystem_directory& dir) {
	return False;
};
Int _redirect_main(const Vector<String>& args) {
	Int argc = args.size();
	Int bad_exit = 1;
	if (argc == 1) {
		(Void)(std::cout << String("\"") + String{"Expected a test folder"} + String("\"") << "\n");
		return bad_exit;
	}
	if (argc > 2) {
		(Void)(std::cout << String("\"") + String{"Expected a single argument"} + String("\"") << "\n");
		return bad_exit;
	}
	if (!test_parse()) {
		return bad_exit;
	}
	(Void)(std::cout << String("\"") + String{"All parse tests passed"} + String("\"") << "\n");
	if (!test_transpile_all_folders(filesystem__entries(String{"C:/"}))) {
		return bad_exit;
	}
	(Void)(std::cout << String("\"") + String{"All transpile tests passed"} + String("\"") << "\n");
	return 0;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
