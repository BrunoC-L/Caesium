#pragma once

class T {
	T t;
};

template <typename T> 
class F {
};

template <typename T> 
class U {
};

class V {
};

template <template <typename V> typename U> 
class K {
};

template <typename T, template <typename V> typename U> 
class G {
};

class A {
	B::T<U, V> member1;
	E<F<H, I>>::G method1();
};

