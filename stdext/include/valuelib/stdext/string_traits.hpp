#pragma once

#include <utility>
#include <type_traits>
#include <string>

namespace value {
	namespace stdext {

		template<class T> 
		struct is_model_of_string 
		: std::false_type
		{
		};

		template<class Char, class Traits, class Allocator>
		struct is_model_of_string<std::basic_string<Char, Traits, Allocator>>
		: std::true_type
		{
		};

		template<class T>
		static constexpr bool IsModelOfString = is_model_of_string<T>::value;

		template<class T>
		struct deduce_value_type {
			using type = std::decay_t<T>;
		};

		template<class T> using ValueType = typename deduce_value_type<T>::type;

		template<bool condition> using EnableIf = std::enable_if_t<condition>;



	}
}