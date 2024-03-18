#pragma once
#include "defaults.hpp"

Int _redirect_main(const Vector<String>& args);
Bool test_transpile_all_folders(const Vector<Variant<filesystem__directory, filesystem__file>>& folders);
Bool test_transpile_folder(const filesystem__directory& dir);
Bool test_parse();
