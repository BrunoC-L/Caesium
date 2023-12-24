template <typename, template <typename...> typename>
struct re_template;

template <typename... Ts, template <typename...> typename A, template <typename...> typename U>
struct re_template<A<Ts...>, U> {
	using type = U<Ts...>;
};
