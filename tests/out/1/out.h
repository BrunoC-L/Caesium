#pragma once
#include <memory>


template <typename B> 
class A {
};

class B {
};

class T {
	std::shared_ptr<A<std::shared_ptr<B>>> t;
};

template <typename A, typename B> 
class F {
};

template <typename A> 
class E {
};

class A {
	std::shared_ptr<E<std::shared_ptr<K>>::T<std::shared_ptr<B>, std::shared_ptr<C>>> member1;
	std::shared_ptr<E<std::shared_ptr<K>>::G> method1();
};

