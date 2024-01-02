#pragma once
#include <functional>

struct OnExit {
	std::function<void()> f;
	~OnExit() {
		f();
	}
};
