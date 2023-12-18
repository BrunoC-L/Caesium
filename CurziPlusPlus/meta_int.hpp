#pragma once

namespace meta_int {

	template <int v>
	struct Int;

	namespace _Int_PlusMinus {
		template <typename T, typename U>
		struct Plus;

		template <int a, int b>
		struct Plus<Int<a>, Int<b>> {
			using type = Int<a + b>;
		};
		template <typename T, typename U>
		struct Minus;

		template <int a, int b>
		struct Minus<Int<a>, Int<b>> {
			using type = Int<a - b>;
		};
	}

	template <typename T, typename U>
	using Plus = _Int_PlusMinus::Plus<T, U>::type;

	template <typename T, typename U>
	using Minus = _Int_PlusMinus::Minus<T, U>::type;

	template <int v>
	struct Int {

		static constexpr int value = v;

		using type = Int<v>;

		template <typename T>
		using Plus = Plus<type, T>;

		template <typename T>
		using Minus = Minus<type, T>;

		using Opposite = Int<-v>;
	};

	namespace named_values {
		using MinusOne = Int<-1>;
		using Zero = Int<0>;
		using One = Int<1>;
		using Two = Int<2>;
		using Three = Int<3>;
		using Four = Int<4>;
		using Five = Int<5>;
		using Six = Int<6>;
		using Seven = Int<7>;
		using Eight = Int<8>;
		using Nine = Int<9>;
	}
}
