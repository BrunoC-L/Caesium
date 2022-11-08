#pragma once
#include <memory>


template <typename B> 
struct A {
	using C = B;
};

struct B {
	using C = B;
};

struct T {
	std::shared_ptr<A<std::shared_ptr<B>>> a;
	std::shared_ptr<B::C> b;
	std::shared_ptr<A<std::shared_ptr<B>>::C> c;
	std::shared_ptr<A<std::shared_ptr<B::C>>> d;
};

template <typename A, typename B> 
struct F {
};

template <typename A> 
struct E {
	using G = A;
	using T = F;
};

template <typename V, typename F> 
struct R : F<std::shared_ptr<V>, std::shared_ptr<F>>, F<std::shared_ptr<E<std::shared_ptr<B>>>, std::shared_ptr<A<std::shared_ptr<B>>>> {
	using C = A<std::shared_ptr<B>>;
	using K = F<std::shared_ptr<B>, std::shared_ptr<C>>;
	std::shared_ptr<E<std::shared_ptr<K>>::T<std::shared_ptr<B>, std::shared_ptr<C>>> member1;
	std::shared_ptr<E<std::shared_ptr<K>>::G> method1() {
	}
};

