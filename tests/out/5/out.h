#pragma once
#include <memory>

#include "OS.h"

template <typename D> 
struct B {
};

struct C {
	std::shared_ptr<B> b;
};

