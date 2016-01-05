#pragma once

#include <string>

namespace value { namespace mysql {

    struct host_tag {};
    struct port_tag {};
    struct username_tag {};
    struct password_tag {};
    struct schema_tag {};
    struct unix_socket_tag {};
    
    template<class T, class Tag>
    struct tagged_value : T
    {
        using T::T;
    };
    
    template<class Tag>
    struct tagged_int
    {
        constexpr tagged_int(int v) : _value(v) {}
        
        constexpr operator const int() const {
            return _value;
        }
        
    private:
        int _value;
    };
    
    using host = tagged_value<std::string, host_tag>;
    using port = tagged_int<port_tag>;
    using username = tagged_value<std::string, username_tag>;
    using password = tagged_value<std::string, password_tag>;
    using schema = tagged_value<std::string, schema_tag>;
    using unix_socket = tagged_value<std::string, unix_socket_tag>;

}} 