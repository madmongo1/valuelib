#pragma once

#include <utility>

namespace value { namespace pointers {
    
    template<class Candidate>
    struct pointer_check
    {
        template<class Ptr> static auto test(Ptr& ptr) -> decltype((*ptr), void(), std::true_type());
        template<class Ptr> static auto test(...) -> decltype(std::false_type());
        constexpr static bool value = decltype(test<Candidate>(std::declval<Candidate>()))::value;
    };
    
    template<class T> constexpr bool CanBeDereferenced = pointer_check<T>::value;
	
}}