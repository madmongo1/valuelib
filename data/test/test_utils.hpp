#pragma once

#include <valuelib/immutable/string.hpp>

extern void emit(const char*, std::size_t) noexcept;

template<std::size_t Length>
constexpr void emit(value::immutable::string_type<Length> s) noexcept
{
	emit(s.c_str(), s.size());
}