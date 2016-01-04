#pragma once

#include "string_traits.hpp"
#include <string>
#include <memory>

namespace value {
	namespace stdext {

		template<class Ch, class Traits, class Allocator>
		struct shared_string
		{
			using by_value_type = std::basic_string<Ch, Traits, Allocator>;
			using pointer_type = std::shared_ptr<by_value_type>;

			using value_type = typename by_value_type::value_type;
			using size_type = typename by_value_type::size_type;

			shared_string(pointer_type ptr) : _ptr(std::move(ptr)) {}

            // find
            size_type find(const by_value_type& str, size_type pos = 0) const {
                return _ptr->find(str, pos);
            }
            
            size_type find(value_type ch, size_type pos = 0) const {
                return _ptr->find(ch, pos);
            }
            
            operator by_value_type&() { return *_ptr; }
            operator const by_value_type&() const { return *_ptr; }
            
            // operator==
            
            friend bool operator==(const shared_string& l, const by_value_type& r) {
                return static_cast<const by_value_type&>(l) == r;
            }
            
            friend bool operator==(const by_value_type& l, const shared_string& r) {
                return l == static_cast<const by_value_type&>(r);
            }
            
			pointer_type _ptr;
		};

		template<class Ch, class Traits, class Allocator>
		struct is_model_of_string< shared_string<Ch, Traits, Allocator> >
		: std::true_type
		{};

		template<class Ch, class Traits, class Allocator>
		auto make_shared_string(std::basic_string<Ch, Traits, Allocator> str)
		{
			return shared_string<Ch, Traits, Allocator>(std::make_shared<std::basic_string<Ch, Traits, Allocator>>(std::move(str)));
		}

		template<class Ch, class Traits, class Allocator>
		auto make_shared_string(std::shared_ptr<std::basic_string<Ch, Traits, Allocator>> ptr)
		{
			return shared_string<Ch, Traits, Allocator>(std::move(ptr));
		}
	}
}