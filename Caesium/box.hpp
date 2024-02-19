#pragma once
#include <memory>
#include "is_specialization.hpp"

template <typename T>
struct Box {
	static_assert(!is_specialization<T, Box>::value, "Box<Box<T>> may not behave as expected, place your box type inside an other type to box it.");
public:
	const T& get() const& {
		return *ptr;
	}

	T& get() & {
		return *ptr;
	}

	T&& get() && {
		return std::move(*ptr);
	}

	operator T& () & {
		return *ptr;
	}

	operator const T& () const & {
		return *ptr;
	}

	operator const T () && {
		return std::move(*ptr);
	}

	// ptr never empty
	Box() = delete;

	Box& operator=(const Box&) = delete;
	Box& operator=(Box&&) = default;

	// construct from T& or U&
	Box(const T& t) : ptr(std::make_unique<T>(t)) {}
	template <typename U>
	Box(const U& u) : ptr(std::make_unique<T>(u)) {}

	// construct from T&& or U&&
	Box(T&& t) : ptr(std::make_unique<T>(std::move(t))) {}
	template <typename U>
	Box(U&& u) : ptr(std::make_unique<T>(std::move(u))) {}

	// copy constructor
	Box(const Box& other) : Box(other.get()) {};
	
	// template copy constructor
	template <typename U>
	Box(const Box<U>& other) : Box(other.get()) {};

	// move constructor
	Box(Box&& other) : Box(std::move(other).get()) {};

	// template move constructor
	template <typename U>
	Box(Box<U>&& other) : Box(std::move(other).get()) {};

	/*// operator= same type&
	Box& operator=(const Box& other) noexcept {
		ptr = std::make_unique<T>(other.get());
		return *this;
	}

	// operator= template&
	template <typename U>
	Box& operator=(const Box<U>& other) noexcept {
		ptr = std::make_unique<T>(other.get());
		return *this;
	}

	// operator= same type&&
	Box& operator=(Box&& other) noexcept {
		ptr = std::move(other.ptr);
		return *this;
	}

	// operator= template&&
	template <typename U>
	Box& operator=(Box<U>&& other) noexcept {
		ptr = std::make_unique<T>(std::move(other).get());
		return *this;
	}*/

	std::weak_ordering operator<=>(const Box& other) const noexcept {
		return (*ptr) <=> (*other.ptr);
	};

	~Box() = default;
protected:
	std::unique_ptr<T> ptr;
};
