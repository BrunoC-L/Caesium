#pragma once
#include <map>

void emplace(auto&& m) {}

void emplace(auto&& m, auto e, auto&&... es) {
	auto [k, v] = std::move(e);
	m.emplace(std::move(k), std::move(v));
	emplace(m, std::forward<decltype(es)>(es)...);
}

template <typename K, typename V>
std::map<K, V, decltype([](const K& l, const K& r) { return l <=> r == std::weak_ordering::less; })> as_map(std::pair<K, V> e, auto&&... es) {
	std::map<K, V, decltype([](const K& l, const K& r) { return l <=> r == std::weak_ordering::less; })> res;
	emplace(res, std::move(e));
	emplace(res, std::forward<decltype(es)>(es)...);
	return res;
}
