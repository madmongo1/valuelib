#pragma once

#include <valuelib/debug/print.hpp>
#include <valuelib/debug/unwrap.hpp>

namespace value { namespace debug {
    
    
    template<>
    struct printer<std::exception_ptr>
    {
        decltype(auto) operator()(std::ostream& os, std::exception_ptr const& ep) const
        {
            return os << "[" << unwrap(ep, csv_formatter()) << "]";
        }
    };
    
}}