#include "valuelib/debug/demangle.hpp"
#include <cxxabi.h>
#include <cassert>
#include <cstring>
#include <regex>

namespace value { namespace debug {
  
    demangled_string::demangled_string(ptr_type&& ptr) noexcept
    : _ptr(std::move(ptr))
    {}
    
    std::ostream& demangled_string::write(std::ostream& os) const
    {
        if (_ptr) {
            return os << _ptr.get();
        }
        else {
            return os << "{nullptr}";
        }
    }
    
    const char* demangled_string::c_str() const
    {
        if (!_ptr)
        {
            throw std::logic_error("demangled_string - zombie object");
        }
        else {
            return _ptr.get();
        }
    }
    
    demangled_string::operator std::string() const {
        return std::string(c_str());
    }
    
    
    demangled_string demangle(const char* name)
    {
        using namespace std::string_literals;

        int status = -4;

        demangled_string::ptr_type ptr {
            abi::__cxa_demangle(name, nullptr, nullptr, &status),
            std::free
        };
        
        if (status == 0) return { std::move(ptr) };

        switch(status)
        {
            case -1: throw std::bad_alloc();
            case -2: {
                std::string msg = "invalid mangled name~";
                msg += name;
                auto p = (char*)std::malloc(msg.length() + 1);
                strcpy(p, msg.c_str());
                return demangled_string::ptr_type { p, std::free };
            }
            case -3:
                assert(!"invalid argument sent to __cxa_demangle");
                throw std::logic_error("invalid argument sent to __cxa_demangle");
            default:
                assert(!"PANIC! unexpected return value");
                throw std::logic_error("PANIC! unexpected return value");
        }
    }

    demangled_string demangle(const std::type_info& type)
    {
        return demangle(type.name());
    }
    
    demangled_string demangle(std::type_index type)
    {
        return demangle(type.name());
    }
    
    std::string strip_nested(const std::string& s)
    {
        static const std::regex libcplusplus("^std::__nested<(.*)>$");
        static const auto boost = std::regex("^boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<(.*)> >$");
        std::smatch match;
        if (std::regex_match(s, match, libcplusplus))
        {
            return match[1].str();
        }
        else if (std::regex_match(s, match, boost))
        {
            return match[1].str();

        }
        else
        {
            return s;
        }
    }

}}