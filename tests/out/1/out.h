#pragma once
#include <memory>


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

struct F {
};

struct E {
	using G = A;
	using T = F;
};

struct R : public F<V, F<U, V>>, public F<E<B>, A<B>> {
	using C = A<B>;
	using K = F<B, C>;
	E<K>::T<B, C> member1;
	E<K>::G method1(K k, C u) {
		Set<int> someContainer;
		for (auto& i : someContainer) {
		}
		int arr;
		for (auto& i : arr) {
		}
		Map<int, std::string> m;
		for (auto& forstatementvar : m.entries(normal()[][x, z, y])) {
			auto& [k, v] = forstatementvar;
			++k;
			k++;
			--k;
			k--;
			k+=1;
			k-=1;
			k*1;
			k+1;
			k-1;
			1%k;
			k[k][k, k.k()[k]]+=k;
		}
	}
};

