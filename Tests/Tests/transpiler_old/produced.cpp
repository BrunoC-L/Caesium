#include "defaults.hpp"



Vector<Variant<directory, file>> entries(const String& dir);
Bool test_parse();
Bool test_transpile_all_folders(const Vector<Variant<directory, file>>& folders);
Bool test_transpile_folder(const directory& dir);
Int _redirect_main(const Vector<String>& args);

Vector<Variant<directory, file>> entries(const String& dir) {
};
Bool test_parse() {
	return False;
};
Bool test_transpile_all_folders(const Vector<Variant<directory, file>>& folders) {
	for (auto&& dir : folders) {
		const Variant<directory, file>& matchval1 = dir;
		if (std::holds_alternative<directory>(matchval1)) {
			const directory& d = std::get<directory>(matchval1);
			return test_transpile_folder(d);
		} else
		if (std::holds_alternative<file>(matchval1)) {
			const file& f = std::get<file>(matchval1);
		} else
			throw;
	}
	return True;
};
Bool test_transpile_folder(const directory& dir) {
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
	if (!test_transpile_all_folders(entries(String{"C:/"}))) {
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
