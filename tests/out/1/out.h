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
	A<B> a;
	B::C b;
	A<B>::C c;
	A<B::C> d;
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
struct R : F<V, F>, F<E<B>, A<B>> {
	using C = A<B>;
	using K = F<B, C>;
	E<K>::T<B, C> member1;
	E<K>::G method1() {
	}
};

