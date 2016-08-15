#pragma once
#include <vector>
#include <valuelib/debug/print.hpp>

namespace value { namespace debug {
    
    template<class V, class A>
    struct printer<std::vector<V, A>>
    {
        decltype(auto) operator()(std::ostream& os, std::vector<V, A> const& c) const
        {
            os << "[";
            auto sep = " ";
            auto post = "";
            for (auto&& i : c)
            {
                os << sep << value::debug::print(i);
                sep = ", ";
                post = " ";
            }
            return os << post << ']';
        }
    };
    
    
}}