#pragma once

#include <valuelib/data/type_traits.hpp>

namespace value { namespace data { namespace sql {

	template<class Dialect>
	struct dialect_base
	{
		static constexpr auto identifier() { return Dialect::identifier(); }

	};
	template<class T, typename = void > struct is_dialect : std::false_type {};
	template<class T> 
	struct is_dialect<
	T, 
	std::enable_if_t< ::value::data::is_derived_from_template_v<T, dialect_base> >
	>
	: std::true_type {};

}}}