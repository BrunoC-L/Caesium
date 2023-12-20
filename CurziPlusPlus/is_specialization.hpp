#pragma once

template <typename T, template <typename...> typename Template>
struct is_specialization : std::false_type {};

template <template <typename...> typename Template, typename... Args>
struct is_specialization<Template<Args...>, Template> : std::true_type {};

template <typename T, template <int...> typename Template>
struct is_specialization_int : std::false_type {};

template <template <int> typename Template, int Arg>
struct is_specialization_int<Template<Arg>, Template> : std::true_type {};
