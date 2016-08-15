#pragma once
#include <valuelib/debug/demangle.hpp>
#include <typeinfo>
#include <typeindex>

namespace value { namespace debug {
    
    template<>
    struct printer<demangled_string>
    {
        decltype(auto) operator()(std::ostream& os, demangled_string const& s) const
        {
            return os << std::quoted(std::string(s));
        }
    };
  
    template<>
    struct printer<std::type_info>
    {
        decltype(auto) operator()(std::ostream& os, std::type_info const& ti) const
        {
            return os << std::quoted(std::string(demangle(ti)));
        }
    };
    
    template<>
    struct printer<std::type_index>
    {
        decltype(auto) operator()(std::ostream& os, std::type_index const& ti) const
        {
            return os << std::quoted(std::string(demangle(ti)));
        }
    };

}}