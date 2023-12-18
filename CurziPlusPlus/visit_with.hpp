//#include <variant>
//#include "is_specialization.hpp"
//#include "meta_int.hpp"
//
//template <std::size_t... indices>
//auto compute_fs_natural_order_tuple(auto&& arg, auto&& refs_tuple, std::index_sequence<indices...>) {
//    return compute_fs_reverse_order(arg, std::move(std::get<sizeof...(indices) - indices - 1>(refs_tuple))...);
//}
//
//template <typename sequence, typename value>
//struct append;
//
//template <std::size_t... indices, size_t N>
//struct append<std::index_sequence<indices...>, meta_int::Int<N>> : public std::index_sequence<indices..., N> {};
//
//template <typename sequence, typename value, typename predicate>
//struct append_if;
//
//template <std::size_t... indices, size_t N>
//struct append_if<std::index_sequence<indices...>, meta_int::Int<N>, std::true_type> : public std::index_sequence<indices..., N> {};
//
//template <std::size_t... indices, size_t N>
//struct append_if<std::index_sequence<indices...>, meta_int::Int<N>, std::false_type> : public std::index_sequence<indices...> {};
//
//template <typename sequence, typename value, typename predicate>
//struct prepend_if;
//
//template <std::size_t... indices, size_t N>
//struct prepend_if<std::index_sequence<indices...>, meta_int::Int<N>, std::true_type> : public std::index_sequence<N, indices...> {};
//
//template <std::size_t... indices, size_t N>
//struct prepend_if<std::index_sequence<indices...>, meta_int::Int<N>, std::false_type> : public std::index_sequence<indices...> {};
//
//template <typename...>
//struct select_variant_type_indices;
//
//template <>
//struct select_variant_type_indices<> : public std::index_sequence<> {};
//
//template <typename T, typename... Ts>
//struct select_variant_type_indices<T, Ts...> : public prepend_if<select_variant_type_indices<Ts...>, T, is_specialization<T, std::variant>> {};
//
//template <typename... Ts>
//auto visit_with_non_variants(Ts&&... ts) {
//	using x = select_variant_type_indices<Ts...>;
//	return std::visit(std::forward<Ts>(ts)...);
//};
