#pragma once
#include "defaults.hpp"

Int _redirect_main(const Vector<String>& args);
Bool test_transpile_all_folders(const Vector<Variant<filesystem__directory, filesystem__file>>& folder);
Bool test_parse();
