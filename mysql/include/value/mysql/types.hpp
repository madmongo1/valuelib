#pragma once

#include <string>
#include <utility>
#include <type_traits>

namespace value { namespace mysql {

    struct host_tag {};
    struct port_tag {};
    struct username_tag {};
    struct password_tag {};
    struct schema_tag {};
    struct unix_socket_tag {};
    struct client_options_tag {};
    
    template<class T, class Tag>
    struct tagged_value : T
    {
        using T::T;
        using tag_type = Tag;
        using underlying_type = T;
    };
    
    template<class Type, class Tag>
    struct tagged_integral
    {
        using value_type = Type;
        using tag_type = Tag;
        
        constexpr tagged_integral() : _value() {}
        constexpr tagged_integral(value_type v) : _value(v) {}
        
        constexpr operator const value_type() const {
            return _value;
        }
        
        constexpr auto value() const {
            return _value;
        }
        
        friend bool operator<(const tagged_integral& l, const tagged_integral& r)
        {
            return l.value() < r.value();
        }
        
        friend bool operator==(const tagged_integral& l, const tagged_integral& r)
        {
            return l.value() == r.value();
        }
        
        friend std::ostream& operator<<(std::ostream& os, const tagged_integral& ti)
        {
            return os << ti.value();
        }
        
    private:
        value_type _value;
    };
    
    
    using host = tagged_value<std::string, host_tag>;
    using port = tagged_integral<int, port_tag>;
    using username = tagged_value<std::string, username_tag>;
    using password = tagged_value<std::string, password_tag>;
    using schema = tagged_value<std::string, schema_tag>;
    using unix_socket = tagged_value<std::string, unix_socket_tag>;
    using client_options = tagged_integral<unsigned long, client_options_tag>;

    template<class T> struct is_connection_argument : std::false_type {};
    template<> struct is_connection_argument<host> : std::true_type {};
    template<> struct is_connection_argument<port> : std::true_type {};
    template<> struct is_connection_argument<username> : std::true_type {};
    template<> struct is_connection_argument<password> : std::true_type {};
    template<> struct is_connection_argument<schema> : std::true_type {};
    template<> struct is_connection_argument<unix_socket> : std::true_type {};
    template<> struct is_connection_argument<client_options> : std::true_type {};
    
    template<class T> using Reduce = std::remove_cv_t<std::remove_reference_t<T>>;

    template<class T>
    static constexpr bool IsConnectionArgument = is_connection_argument<Reduce<T>>::value;

    template<class...Args> struct are_all_connection_arguments;
    
    template<>
    struct are_all_connection_arguments<> : std::true_type {};
    
    template<class Arg>
    struct are_all_connection_arguments<Arg>
    : std::integral_constant<bool, is_connection_argument<Arg>::value> {};
    
    template<class Arg, class...Rest>
    struct are_all_connection_arguments<Arg, Rest...>
    : std::integral_constant<bool, is_connection_argument<Arg>::value and are_all_connection_arguments<Rest...>::value> {};
    
    template<class...Args>
    static constexpr bool AreAllConnectionArguments = are_all_connection_arguments<Reduce<Args>...>::value;
    
    

}}