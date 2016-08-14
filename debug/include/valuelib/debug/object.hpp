#pragma once

#include <valuelib/debug/print.hpp>
#include <ostream>
#include <tuple>

namespace value { namespace debug {
  
    // nvp
    
    template<class N, class V>
    struct nvp_type
    {
        friend std::ostream& operator<<(std::ostream& os, const nvp_type& nvp)
        {
            return os << nvp.name << ": " << print(nvp.value);
        }
        
        N const& name;
        V const& value;
    };
    
    template<class N, class V>
    struct printer<nvp_type<N, V>>
    {
        void operator()(std::ostream& os, nvp_type<N, V> const& nvp)
        {
            os << nvp;
        }
    };
    
    template<class N, class T>
    auto nvp(N const& name, T const& value)
    {
        return nvp_type<N, T> { name, value };
    }
    
    // object representation
    
    
    template<class Pairs>
    struct object_type
    {
        template<std::size_t I>
        static const char* sep(std::integral_constant<std::size_t, I>)
        {
            return ", ";
        }
        
        static const char* sep(std::integral_constant<std::size_t, 0>)
        {
            return " ";
        }
        
        template<class Stream, class Tuple, std::size_t...Is>
        static void emit(Stream& os, Tuple const& t, std::index_sequence<Is...>)
        {
            os << '{';
            using expand = int[];
            void(expand{0,
                ((os << sep(std::integral_constant<std::size_t, Is>())),
                 (os << print(std::get<Is>(t))),
                 0)...
            });
            os << " }";
        }
        
        std::ostream& operator()(std::ostream& os) const
        {
            emit(os, _pairs, std::make_index_sequence<std::tuple_size<Pairs>::value>());
            return os;
        }
        
        friend std::ostream& operator<<(std::ostream& os, const object_type& obj)
        {
            return obj(os);
        }
        
        Pairs _pairs;
    };
    
    template<class Name, class Value>
    auto object_tuple(Name const& name, Value const& value)
    {
        return std::make_tuple(nvp(name, value));
    }
    
    template<class Name, class Value, class...Pairs>
    auto object_tuple(Name const& name, Value const& value, Pairs const&... rest)
    {
        return std::tuple_cat(std::make_tuple(nvp(name, value)),
                              object_tuple(rest...));
    }
    
    template<class Name, class Value, class...Pairs>
    auto object(Name const& name, Value const& value, Pairs const&... rest)
    {
        auto tuple = object_tuple(name, value, rest...);
        using tuple_type = decltype(tuple);
        return object_type<tuple_type> { std::move(tuple) };
    }

}}