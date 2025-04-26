#pragma once

#define ExpandAll(M) \
M(1, NodeStructs::WordTypenameOrExpression)\
M(3, NodeStructs::Typename)\
M(1, NodeStructs::OptionalTypename)\
M(1, NodeStructs::UnionTypename)\
M(1, NodeStructs::VariadicExpansionTypename)\
M(1, NodeStructs::BaseTypename)\
M(2, NodeStructs::NamespacedTypename)\
M(2, NodeStructs::TemplatedTypename)\
M(2, NodeStructs::Expression)\
M(2, NodeStructs::FunctionArgument)\
M(2, NodeStructs::ConditionalExpression)\
M(2, NodeStructs::OrExpression)\
M(2, NodeStructs::AndExpression)\
M(2, NodeStructs::EqualityExpression)\
M(2, NodeStructs::CompareExpression)\
M(2, NodeStructs::AdditiveExpression)\
M(2, NodeStructs::MultiplicativeExpression)\
M(2, NodeStructs::UnaryExpression)\
M(2, NodeStructs::CallExpression)\
M(2, NodeStructs::NamespaceExpression)\
M(2, NodeStructs::TemplateExpression)\
M(2, NodeStructs::ConstructExpression)\
M(2, NodeStructs::BracketAccessExpression)\
M(3, NodeStructs::PropertyAccessAndCallExpression)\
M(2, NodeStructs::PropertyAccessExpression)\
M(1, NodeStructs::ParenArguments)\
M(1, NodeStructs::BraceArguments)\
M(1, NodeStructs::BracketArguments)\
M(0, NodeStructs::Reference)\
M(0, NodeStructs::MutableReference)\
M(0, NodeStructs::Move)\
M(0, NodeStructs::Value)\
M(1, NodeStructs::RunTimeStatement)\
M(2, NodeStructs::VariableDeclaration)\
M(1, NodeStructs::Statement<function_context>)\
M(1, NodeStructs::Statement<type_context>)\
M(1, NodeStructs::Statement<top_level_context>)\
M(1, NodeStructs::CompileTimeStatement<function_context>)\
M(1, NodeStructs::CompileTimeStatement<type_context>)\
M(1, NodeStructs::CompileTimeStatement<top_level_context>)\
M(3, NodeStructs::VariableDeclarationStatement<function_context>)\
M(3, NodeStructs::VariableDeclarationStatement<type_context>)\
M(3, NodeStructs::VariableDeclarationStatement<top_level_context>)\
M(3, NodeStructs::IfStatement<function_context>)\
M(3, NodeStructs::IfStatement<type_context>)\
M(3, NodeStructs::IfStatement<top_level_context>)\
M(3, NodeStructs::ForStatement<function_context>)\
M(3, NodeStructs::ForStatement<type_context>)\
M(3, NodeStructs::ForStatement<top_level_context>)\
M(2, NodeStructs::IForStatement<function_context>)\
M(2, NodeStructs::IForStatement<type_context>)\
M(2, NodeStructs::IForStatement<top_level_context>)\
M(2, NodeStructs::WhileStatement<function_context>)\
M(2, NodeStructs::WhileStatement<type_context>)\
M(2, NodeStructs::WhileStatement<top_level_context>)\
M(2, NodeStructs::MatchCase<function_context>)\
M(2, NodeStructs::MatchCase<type_context>)\
M(2, NodeStructs::MatchCase<top_level_context>)\
M(2, NodeStructs::MatchStatement<function_context>)\
M(2, NodeStructs::MatchStatement<type_context>)\
M(2, NodeStructs::MatchStatement<top_level_context>)\
M(1, NodeStructs::BreakStatement<function_context>)\
M(1, NodeStructs::BreakStatement<type_context>)\
M(1, NodeStructs::BreakStatement<top_level_context>)\
M(2, NodeStructs::ReturnStatement<function_context>)\
M(2, NodeStructs::ReturnStatement<type_context>)\
M(2, NodeStructs::ReturnStatement<top_level_context>)\
M(2, NodeStructs::SwitchCase<function_context>)\
M(2, NodeStructs::SwitchCase<type_context>)\
M(2, NodeStructs::SwitchCase<top_level_context>)\
M(2, NodeStructs::SwitchStatement<function_context>)\
M(2, NodeStructs::SwitchStatement<type_context>)\
M(2, NodeStructs::SwitchStatement<top_level_context>)\
M(2, NodeStructs::Assignment<function_context>)\
M(2, NodeStructs::Assignment<type_context>)\
M(2, NodeStructs::Assignment<top_level_context>)\
M(1, NodeStructs::BlockStatement<function_context>)\
M(1, NodeStructs::BlockStatement<type_context>)\
M(1, NodeStructs::BlockStatement<top_level_context>)\
M(6, NodeStructs::Interface)\
M(6, NodeStructs::Type)\
M(3, NodeStructs::Alias)\
M(2, NodeStructs::MemberVariable)\
M(2, NodeStructs::Block)\
M(6, NodeStructs::Template)\
M(3, NodeStructs::Enum)\
M(1, NodeStructs::TemplateParameter)\
M(2, NodeStructs::TemplateParameterWithDefaultValue)\
M(1, NodeStructs::VariadicTemplateParameter)\
M(5, NodeStructs::Function)\
M(2, NodeStructs::FunctionParameter)\
M(11, NodeStructs::NameSpace)\
M(1, NodeStructs::Exists)\
M(3, NodeStructs::File)\
M(1, NodeStructs::Import)\
M(3, Realised::Type); \
M(1, Realised::FunctionType); \
M(2, Realised::InterfaceType); \
M(2, Realised::NamespaceType); \
M(1, Realised::TemplateType); \
M(2, Realised::EnumType); \
M(3, Realised::EnumValueType); \
M(2, Realised::OptionalType); \
M(2, Realised::AggregateType); \
M(2, Realised::UnionType); \
M(2, Realised::VectorType); \
M(2, Realised::SetType); \
M(3, Realised::MapType); \
M(1, Realised::TypeListType); \
M(2, Realised::Parameter); \
M(4, Realised::Function); \
M(2, Realised::MemberVariable); \
M(2, Realised::Argument); \
M(2, Realised::CompileTimeType); \
M(3, Realised::Interface);

#define CMP_N(N, T) inline std::strong_ordering operator<=>(const T& left, const T& right);
ExpandAll(CMP_N);
CMP_N(1, Realised::PrimitiveType);
CMP_N(1, Realised::MetaType);
CMP_N(13, Realised::NameSpace);
CMP_N(1, Realised::Builtin);
#undef CMP_N

template <typename T>
std::strong_ordering cmp(const T& a, const T& b) {
	if constexpr (is_specialization<T, std::vector>::value) {
		if (auto size_cmp = cmp(a.size(), b.size()); size_cmp != 0)
			return size_cmp;

		for (size_t i = 0; i < a.size(); ++i)
			if (auto v = cmp(a.at(i), b.at(i)); v != std::strong_ordering::equivalent)
				return v;

		return std::strong_ordering::equivalent;
	}
	else if constexpr (is_specialization<T, std::pair>::value) {
		if (auto first = cmp(a.first, b.first); first != 0)
			return first;
		return cmp(a.second, b.second);
	}
	else if constexpr (is_specialization<T, caesium_lib::optional::type>::value) {
		return cmp(a._value, b._value);
	}
	else if constexpr (is_specialization<T, std::optional>::value) {
		return a.has_value() && b.has_value() ? cmp(a.value(), b.value()) : cmp(a.has_value(), b.has_value());
	}
	else if constexpr (is_specialization<T, caesium_lib::variant::type>::value) {
		return cmp(a._value, b._value);
	}
	else if constexpr (is_specialization<T, std::variant>::value) {
		auto index_cmp = cmp(a.index(), b.index());
		if (index_cmp != 0)
			return index_cmp;
		return std::visit(
			[&](const auto& _a) {
				return cmp(_a, std::get<std::remove_cvref_t<decltype(_a)>>(b));
			},
			a
		);
	}
	else if constexpr (is_specialization<T, Box>::value) {
		return cmp(a.get(), b.get());
	}
	else if constexpr (is_specialization<T, NonCopyableBox>::value) {
		return cmp(a.get(), b.get());
	}
	else if constexpr (is_specialization<T, std::reference_wrapper>::value) {
		return cmp(a.get(), b.get());
	}
	else if constexpr (std::is_same_v<T, std::string>)
		return a <=> b;
	else if constexpr (std::is_same_v<T, int>)
		return a <=> b;
	else if constexpr (std::is_same_v<T, char>)
		return a <=> b;
	else if constexpr (std::is_same_v<T, bool>)
		return a <=> b;
	else if constexpr (std::is_same_v<T, Realised::void_t>)
		return a <=> b;
	else if constexpr (std::is_same_v<T, Realised::empty_optional_t>)
		return a <=> b;
	else if constexpr (std::is_same_v<T, rule_info>)
		return std::strong_ordering::equivalent;
	else if constexpr (std::is_same_v<T, double>) {
		std::partial_ordering partial = a <=> b;
		if (partial == partial.greater)
			return std::strong_ordering::greater;
		else if (partial == partial.less)
			return std::strong_ordering::less;
		else if (partial == partial.equivalent)
			return std::strong_ordering::equivalent;
		else
			NOT_IMPLEMENTED;
	}
	else if constexpr (is_specialization<T, Realised::PrimitiveType::NonValued>::value) {
		return std::strong_ordering::equivalent;
	}
	else if constexpr (is_specialization<T, Realised::PrimitiveType::Valued>::value) {
		return cmp(a.value, b.value);
	}
	else
		return a <=> b;
}

template <typename T>
std::strong_ordering cmp0(const T& x1, const T& x2) {
	return std::strong_ordering::equivalent;
}

template <typename T>
std::strong_ordering cmp1(const T& x1, const T& x2) {
	const auto& [m1] = x1;
	const auto& [n1] = x2;
	if (auto c = cmp(m1, n1); c != std::strong_ordering::equivalent)
		return c;
	return std::strong_ordering::equivalent;
}

template <typename T>
std::strong_ordering cmp2(const T& x1, const T& x2) {
	const auto& [m1, m2] = x1;
	const auto& [n1, n2] = x2;
	if (auto c = cmp(m1, n1); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m2, n2); c != std::strong_ordering::equivalent)
		return c;
	return std::strong_ordering::equivalent;
}

template <typename T>
std::strong_ordering cmp3(const T& x1, const T& x2) {
	const auto& [m1, m2, m3] = x1;
	const auto& [n1, n2, n3] = x2;
	if (auto c = cmp(m1, n1); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m2, n2); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m3, n3); c != std::strong_ordering::equivalent)
		return c;
	return std::strong_ordering::equivalent;
}

template <typename T>
std::strong_ordering cmp4(const T& x1, const T& x2) {
	const auto& [m1, m2, m3, m4] = x1;
	const auto& [n1, n2, n3, n4] = x2;
	if (auto c = cmp(m1, n1); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m2, n2); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m3, n3); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m4, n4); c != std::strong_ordering::equivalent)
		return c;
	return std::strong_ordering::equivalent;
}

template <typename T>
std::strong_ordering cmp5(const T& x1, const T& x2) {
	const auto& [m1, m2, m3, m4, m5] = x1;
	const auto& [n1, n2, n3, n4, n5] = x2;
	if (auto c = cmp(m1, n1); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m2, n2); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m3, n3); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m4, n4); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m5, n5); c != std::strong_ordering::equivalent)
		return c;
	return std::strong_ordering::equivalent;
}

template <typename T>
std::strong_ordering cmp6(const T& x1, const T& x2) {
	const auto& [m1, m2, m3, m4, m5, m6] = x1;
	const auto& [n1, n2, n3, n4, n5, n6] = x2;
	if (auto c = cmp(m1, n1); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m2, n2); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m3, n3); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m4, n4); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m5, n5); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m6, n6); c != std::strong_ordering::equivalent)
		return c;
	return std::strong_ordering::equivalent;
}

template <typename T>
std::strong_ordering cmp11(const T& x1, const T& x2) {
	const auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11] = x1;
	const auto& [n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11] = x2;
	if (auto c = cmp(m1, n1); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m2, n2); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m3, n3); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m4, n4); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m5, n5); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m6, n6); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m7, n7); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m8, n8); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m9, n9); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m10, n10); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m11, n11); c != std::strong_ordering::equivalent)
		return c;
	return std::strong_ordering::equivalent;
}

template <typename T>
std::strong_ordering cmp12(const T& x1, const T& x2) {
	const auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12] = x1;
	const auto& [n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12] = x2;
	if (auto c = cmp(m1, n1); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m2, n2); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m3, n3); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m4, n4); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m5, n5); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m6, n6); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m7, n7); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m8, n8); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m9, n9); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m10, n10); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m11, n11); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m12, n12); c != std::strong_ordering::equivalent)
		return c;
	return std::strong_ordering::equivalent;
}

template <typename T>
std::strong_ordering cmp13(const T& x1, const T& x2) {
	const auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13] = x1;
	const auto& [n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13] = x2;
	if (auto c = cmp(m1, n1); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m2, n2); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m3, n3); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m4, n4); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m5, n5); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m6, n6); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m7, n7); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m8, n8); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m9, n9); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m10, n10); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m11, n11); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m12, n12); c != std::strong_ordering::equivalent)
		return c;
	if (auto c = cmp(m13, n13); c != std::strong_ordering::equivalent)
		return c;
	return std::strong_ordering::equivalent;
}

#define CMP_N(N, T) inline std::strong_ordering operator<=>(const T& left, const T& right) { return cmp##N (left, right); }
ExpandAll(CMP_N);

inline std::strong_ordering operator<=>(const Realised::PrimitiveType& left, const Realised::PrimitiveType& right) {
	const auto& a = left.value._value;
	const auto& b = right.value._value;
	return cmp(a.index(), b.index());
}

inline std::strong_ordering operator<=>(const Realised::MetaType& left, const Realised::MetaType& right) {
	if (holds<Realised::EnumType>(left) &&
		holds<Realised::EnumValueType>(right)) {
		const auto& a = get<Realised::EnumType>(left);
		const auto& b = get<Realised::EnumValueType>(right);
		return cmp(a.enum_.get(), b.enum_.get());
	}
	if (holds<Realised::EnumValueType>(left) &&
		holds<Realised::EnumType>(right)) {
		const auto& a = get<Realised::EnumValueType>(left);
		const auto& b = get<Realised::EnumType>(right);
		return cmp(a.enum_.get(), b.enum_.get());
	}
	return cmp(left.type.get()._value, right.type.get()._value);
}

inline std::strong_ordering operator<=>(const Realised::NameSpace& left, const Realised::NameSpace& right) {
	NOT_IMPLEMENTED;
}

inline std::strong_ordering operator<=>(const Realised::Builtin& left, const Realised::Builtin& right) {
	NOT_IMPLEMENTED;
}
