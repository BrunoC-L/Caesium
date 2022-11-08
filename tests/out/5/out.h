#pragma once
#include <memory>

#include "OS.h"

template <typename D> 
class B {
};

class C {
	std::shared_ptr<B> b;
};

