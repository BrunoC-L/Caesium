class T {
};
template <typename T> class F {
};
template <typename T> class U {
};
class V {
};
template <template <typename V> typename U> class K {
};
template <typename T, template <typename V> typename U> class G {
};
class A : public G<V, F> {
};
