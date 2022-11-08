#pragma once
#include <memory>


struct A {
	std::shared_ptr<B> b;
	std::shared_ptr<C> c;
	std::shared_ptr<int> x;
	std::shared_ptr<int> y;
	std::shared_ptr<A> a() {
	}
	std::shared_ptr<void> a() {
	}
	std::shared_ptr<void> a() {
	}
};

