/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   trace.hpp
 * Author: rhodges
 *
 * Created on 27 April 2016, 10:10
 */

#pragma once
#include <iostream>

namespace value { namespace debug {
    
    struct sentinel_type {};
    static constexpr sentinel_type sentinel{};

    namespace detail {
        template<size_t I> std::ostream& emit_comma(std::ostream& os) { return os << ", "; }
        template<> std::ostream& emit_comma<0>(std::ostream& os) { return os; }
        template<size_t I, class T> std::ostream& emit_comma(std::ostream& os, const T& t) {
            return emit_comma<I>(os) << t;
        }
        
    }
    struct tracer
    {
        template<class...Args>
        tracer(std::ostream& os, const char* funcname, Args&&...args)
        {
            auto& d = depth();
            os << lead_in(d) << funcname << '(';
            emit_args(std::cerr,
                      std::make_index_sequence<sizeof...(args)>(),
                      std::forward<Args>(args)...)
            << ')' << std::endl;
            ++d;
        }
        tracer(const tracer&) = delete;
        tracer& operator=(const tracer&) = delete;
        
        ~tracer()
        {
            --depth();
        }
        
        static
        std::string lead_in(std::size_t depth)
        {
            constexpr const char model[] = "---------->";
            constexpr auto model_size = std::extent<decltype(model)>::value -1;
            if (depth > model_size)
            {
                std::string ret = std::to_string(depth);
                ret.insert(std::end(ret), model + std::size(ret), model + model_size);
                return ret;
            }
            return { model + model_size - depth, model + model_size };
        }
        
        template<std::size_t...Is, class...Ts>
        static std::ostream& emit_args(std::ostream& os, std::index_sequence<Is...>, Ts&&...ts)
        {
            using expand = int[];
            void(expand{ 0, (detail::emit_comma<Is>(os, ts),0)...});
            return os;
        }
        
        static std::size_t& depth()
        {
            static __thread std::size_t d = 0;
            return d;
        }
    };
    
}}
/*
#define VALUE_DEBUG_CONCAT(a, b) a ## b
#define VALUE_DEBUG_UNIQUENAME(prefix) VALUE_DEBUG_CONCAT(prefix, __COUNTER__)
#define VALUE_DEBUG_VA_ARGS(...) , ##__VA_ARGS__

#define VALUE_DEBUG_TRACE(Func, ...) \
::value::debug::tracer VALUE_DEBUG_UNIQUENAME(value_debug_TRACER) { \
Func VALUE_DEBUG_VA_ARGS(__VA_ARGS__) , \
::value::debug::sentinel \
}

#define VALUE_DEBUG_TRACE0(Func, ...) \
value::debug::tracer VALUE_DEBUG_UNIQUENAME(value_debug_TRACER) \
{Func, \
value::debug::sentinel \
}
*/