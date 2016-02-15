#pragma once
#include <memory>
#include <ostream>
#include <typeinfo>
#include <typeindex>

namespace value { namespace debug {

    struct demangled_string
    {
        using ptr_type = std::unique_ptr<char, void(*)(void*)>;
        demangled_string(ptr_type&& ptr) noexcept;
        const char* c_str() const;

        std::ostream& write(std::ostream& os) const;
    private:
        ptr_type _ptr;
    };

    inline std::ostream& operator<<(std::ostream& os, const demangled_string& str)
    {
        return str.write(os);
    }
    
    inline std::string operator+ (std::string l, const demangled_string& r) {
        return l + r.c_str();
    }
    
    demangled_string demangle(const char* name);
    demangled_string demangle(const std::type_info& type);
    demangled_string demangle(std::type_index type);

    template<class T>
    demangled_string derived_classname(T* p) {
        return demangle(typeid(*p));
    }
    
    template<class T>
    demangled_string this_classname(T*) {
        return demangle(typeid(T));
    }
    
    

    

}}