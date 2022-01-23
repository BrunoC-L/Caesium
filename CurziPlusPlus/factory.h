#pragma once
#include "json.h"
#include <sstream>

class CPPelement;

using stream = struct {
	std::stringstream header;
	std::stringstream cpp;
};

class CPPelementFactory {
public:
	std::unique_ptr<CPPelement> create(const JSON& data, stream& out) const;
};
