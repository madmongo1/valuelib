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
        operator std::string() const;

        std::ostream& write(std::ostream& os) const;
    private:
        ptr_type _ptr;
    };

    inline bool operator==(const std::string& l, const demangled_string& r) {
        return std::strcmp(l.c_str(), r.c_str()) == 0;
    }

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
    
    template<class T>
    demangled_string demangle()
    {
        return demangle(typeid(T));
    }
    
    std::string strip_nested(const char* demangled_name);
    std::string strip_nested(demangled_string demangled_name);
    std::string strip_nested(std::string demangled_name);
    
    

    

}}