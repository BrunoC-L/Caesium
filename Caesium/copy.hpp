#pragma once

template <typename T>
T copy0(const T& x) {
	return {};
}

template <typename T>
T copy1(const T& x) {
	const auto& [a] = x;
	return {
		copy(a),
	};
}

template <typename T>
T copy2(const T& x) {
	const auto& [a, b] = x;
	return {
		copy(a),
		copy(b),
	};
}

template <typename T>
T copy3(const T& x) {
	const auto& [a, b, c] = x;
	return {
		copy(a),
		copy(b),
		copy(c),
	};
}

template <typename T>
T copy4(const T& x) {
	const auto& [a, b, c, d] = x;
	return {
		copy(a),
		copy(b),
		copy(c),
		copy(d),
	};
}

template <typename T>
T copy5(const T& x) {
	const auto& [a, b, c, d, e] = x;
	return {
		copy(a),
		copy(b),
		copy(c),
		copy(d),
		copy(e),
	};
}

template <typename T>
T copy6(const T& x) {
	const auto& [a, b, c, d, e, f] = x;
	return {
		copy(a),
		copy(b),
		copy(c),
		copy(d),
		copy(e),
		copy(f),
	};
}

template <typename T>
T copy11(const T& x) {
	const auto& [a, b, c, d, e, f, g, h, i, j, k] = x;
	return {
		copy(a),
		copy(b),
		copy(c),
		copy(d),
		copy(e),
		copy(f),
		copy(g),
		copy(h),
		copy(i),
		copy(j),
		copy(k),
	};
}

template <typename T>
T copy12(const T& x) {
	const auto& [a, b, c, d, e, f, g, h, i, j, k, l] = x;
	return {
		copy(a),
		copy(b),
		copy(c),
		copy(d),
		copy(e),
		copy(f),
		copy(g),
		copy(h),
		copy(i),
		copy(j),
		copy(k),
		copy(l),
	};
}

template <typename T>
T copy13(const T& x) {
	const auto& [a, b, c, d, e, f, g, h, i, j, k, l, m] = x;
	return {
		copy(a),
		copy(b),
		copy(c),
		copy(d),
		copy(e),
		copy(f),
		copy(g),
		copy(h),
		copy(i),
		copy(j),
		copy(k),
		copy(l),
		copy(m),
	};
}

template <typename T, typename compare>
struct copy_t<std::set<T, compare>> {
	static std::set<T, compare> copy(const std::set<T, compare>& s) {
		std::set<T, compare> res;
		for (const auto& e : s)
			res.insert(::copy(e));
		return res;
	}
};

template <typename K, typename V, typename compare>
struct copy_t<std::map<K, V, compare>> {
	static std::map<K, V, compare> copy(const std::map<K, V, compare>& m) {
		std::map<K, V, compare> res;
		for (const auto& [k, v] : m)
			res.emplace(::copy(k), ::copy(v));
		return res;
	}
};

template <typename T>
struct copy_t<NonCopyableBox<T>> {
	static NonCopyableBox<T> copy(const NonCopyableBox<T>& box) {
		return NonCopyableBox<T>{ ::copy(box.get()) };
	}
};

template <typename T>
struct copy_t<std::reference_wrapper<T>> {
	static std::reference_wrapper<T> copy(const std::reference_wrapper<T>& t) {
		return { t.get() };
	}
};

template <typename T, typename U>
struct copy_t<std::pair<T, U>> {
	static std::pair<T, U> copy(const std::pair<T, U>& x) {
		return copy2(x);
	}
};

#define COPY_N(N, T) template <> struct copy_t<T> { static T copy(const T& e) { return copy##N (e); } };
ExpandAll(COPY_N); // defined from include order cmp.hpp

template <typename T>
struct copy_t<Realised::PrimitiveType::NonValued<T>> {
	static Realised::PrimitiveType::NonValued<T> copy(const Realised::PrimitiveType::NonValued<T>& v) {
		return Realised::PrimitiveType::NonValued<T>{};
	}
};

template <typename T>
struct copy_t<Realised::PrimitiveType::Valued<T>> {
	static Realised::PrimitiveType::Valued<T> copy(const Realised::PrimitiveType::Valued<T>& v) {
		return Realised::PrimitiveType::Valued<T>{ ::copy(v.value) };
	}
};

template <>
struct copy_t<Realised::PrimitiveType> {
	static Realised::PrimitiveType copy(const Realised::PrimitiveType& p) {
		return Realised::PrimitiveType{ ::copy(p.value) };
	}
};

COPY_N(1, Realised::MetaType);

template <>
struct copy_t<Realised::NameSpace> {
	static Realised::NameSpace copy(const Realised::NameSpace& p) {
		NOT_IMPLEMENTED; // return Realised::NameSpace{ ::copy(p.value) };
	}
};

COPY_N(1, Realised::Builtin);
