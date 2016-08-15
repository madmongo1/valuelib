#pragma once
#include <valuelib/debug/print.hpp>
#include <chrono>
#include <iomanip>

namespace value { namespace debug {

    template<>
    struct printer<std::chrono::milliseconds>
    {
        decltype(auto) operator()(std::ostream& os, std::chrono::milliseconds const& duration) const
        {
            std::string s = std::to_string(duration.count()) + "ms";
            return os << std::quoted(s);
        }
    };
    
    template<>
    struct printer<std::chrono::seconds>
    {
        decltype(auto) operator()(std::ostream& os, std::chrono::seconds const& duration) const
        {
            return os << std::quoted(std::to_string(duration.count()) + "s");
        }
    };
    
    
}}