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
#include <utility>
#include <mutex>
#include <thread>
#include <iomanip>
#include <cassert>
#include <valuelib/debug/demangle.hpp>
#include <valuelib/debug/unwrap.hpp>
#include <boost/log/trivial.hpp>
#include <typeinfo>
#include <typeindex>
#include <chrono>

namespace value { namespace debug {
    
    struct sentinel_type {};
    static constexpr sentinel_type sentinel{};
    
    struct classname_type {
        constexpr classname_type(const char* name) : _name(name) {}
        const char* _name;
        
        friend std::ostream& operator<<(std::ostream& os, const classname_type& cn) {
            return os << cn._name;
        }
        
    };
    
    template<class Object>
    classname_type classname(Object&)
    {
        static const auto name = classname_type(demangle(typeid(Object)).c_str());
        return name;
    }
    
    template<class Object>
    classname_type classname(Object*p)
    {
        return classname(*p);
    }
    
    struct method {
        constexpr method(const char* name) : _name(name) {}
        const char* _name;
        
        friend std::ostream& operator<<(std::ostream& os, const method& m) {
            return os << m._name;
        }
    };
    
    template<class Type>
    struct printable
    {
        auto classname() const { return ::value::debug::classname(this); }
        
    };
    
    template<class Arg>
    struct arg_proxy
    {
        const char* name;
        Arg const& arg;
        friend auto debug_tuple(const arg_proxy& ap) { return std::make_tuple(std::tie(ap.name, ap.arg)); }
    };
    template<class Arg> auto make_arg_proxy(const char* name, const Arg& arg)
    {
        return arg_proxy<Arg> { name, arg };
    }
    
    template<class T>
    struct has_debug_tuple {
        template<class U> static auto test(const U*p) -> decltype(debug_tuple(*p), void(), std::true_type());
        template<class U> static auto test(...) -> decltype(std::false_type());
        static constexpr bool value = decltype(test<T>(nullptr))::value;
    };
    template<class T> static constexpr bool HasDebugTuple = has_debug_tuple<T>::value;

    //
    // client classes should override debug_print if they have special needs
    //
    template<class T>
    std::ostream& debug_print(std::ostream& os, const T& t) {
        return os << t;
    }
    
    inline std::ostream& debug_print(std::ostream& os, std::chrono::milliseconds ms)
    {
        return os << (std::to_string(ms.count()) + "ms");
    }
    
    inline std::ostream& debug_print(std::ostream& os, const std::chrono::seconds& s)
    {
        auto count = s.count();
        os << count;
        os << "s";
        return os;
    }
    
    inline
    std::ostream& debug_print(std::ostream& os, const std::type_info& info)
    {
        return os << value::debug::demangle(info);
    }

    inline
    std::ostream& debug_print(std::ostream& os, std::type_index info)
    {
        return os << value::debug::demangle(info);
    }
    
    inline
    std::ostream& debug_print(std::ostream& os, const std::exception_ptr& ep)
    {
        return os << value::debug::unwrap(ep);
    }
    
    namespace detail {
        template<class T, typename = void>
        struct debug_printer
        {
            debug_printer(const T& t) : _t(t) {};
            
            std::ostream& operator()(std::ostream& os) const {
                using ::value::debug::debug_print;
                return debug_print(os, _t);
            }
            const T& _t;
        };
    }
    
    template<class T>
    auto print(const T& t)
    {
        return detail::debug_printer<T>(t);
    }
    
    inline std::ostream& debug_print(std::ostream& os, const char* p)
    {
        return os << std::quoted(p);
    }
    
    inline std::ostream& debug_print(std::ostream& os, const std::string s)
    {
        return os << std::quoted(s);
    }
    

    namespace detail {
        
        template<class T>
        struct debug_printer<T, std::enable_if_t<HasDebugTuple<T>>>
        {
            debug_printer(const T& t) : _t(t) {};
            
            template<std::size_t N>
            static auto sep(std::integral_constant<std::size_t, N>) { return ", "; }
            static auto sep(std::integral_constant<std::size_t, 0>) { return ' '; }
            
            template<std::size_t N, class U>
            static void emit_item(std::ostream& os, std::integral_constant<std::size_t, N> i, const U& u)
            {
                using value::debug::print;
                os << sep(i);
                os << std::get<0>(u) << ": " << print(std::get<1>(u));
            }
            
            template<class Tuple, std::size_t...Is>
            static void emit_items(std::ostream& os, Tuple& tuple, std::index_sequence<Is...>)
            {
                using expand = int[];
                void(expand{0,
                    ((emit_item(os, std::integral_constant<std::size_t, Is>(), std::get<Is>(tuple))), 0)...
                });
            }
            
            std::ostream& operator()(std::ostream& os) const {
                os << "{";
                auto tuple = debug_tuple(_t);
                emit_items(os, tuple, std::make_index_sequence<std::tuple_size<decltype(tuple)>::value>());
                return os << " }";
            }
            const T& _t;
        };

        template<class T>
        std::ostream& operator<<(std::ostream& os, const debug_printer<T>& p) {
            return p(os);
        }


    }
    
    

    
    
    
    template<class Type>
    std::ostream& operator<<(std::ostream& os, const printable<Type>& p)
    {
    }
    
    
    
    namespace detail {
        template<size_t N>
        struct emit_comma
        {
            std::ostream& operator()(std::ostream& os) const { return os << ", "; }
            template<class T>
            std::ostream& operator()(std::ostream& os, const T& t) const {
                return emit_comma<N>()(os) << t;
            }
        };
        
        template<>
        inline
        std::ostream& emit_comma<0>::operator()(std::ostream& os) const { return os; }
        
    }
    struct tracer
    {
        struct common_data
        {
            using mutex_type = std::mutex;
            using lock_type = std::unique_lock<mutex_type>;
            
            lock_type get_lock() { return lock_type(_mutex); }
            
            std::mutex _mutex;
        };
        
        static common_data& get_common_data() {
            static common_data _data {};
            return _data;
        }
        
        
        template<class StringLike, class...Args>
        tracer(std::ostream& os, StringLike&& funcname, Args&&...args)
        {
            auto& d = depth();
            auto lock = get_common_data().get_lock();
            os
            << std::this_thread::get_id() << " : "
            << lead_in(d)
            << funcname << '(';
            emit_args(os,
                      std::make_index_sequence<sizeof...(args)>(),
                      std::forward<Args>(args)...)
            << ')' << std::endl;
            ++d;
        }
        template<class...Args>
        tracer(std::ostream& os, classname_type cn, method m, Args&&...args)
        {
            auto& d = depth();
            auto lock = get_common_data().get_lock();
            os
            << std::this_thread::get_id() << " : "
            << lead_in(d) << cn << "::" << m << '(';
            emit_args(os,
                      std::make_index_sequence<sizeof...(args)>(),
                      std::forward<Args>(args)...)
            << ')' << std::endl;
            ++d;
        }
        
        template<class...Args>
        tracer(std::ostream& os, method m, Args&&...args)
        {
            auto& d = depth();
            auto lock = get_common_data().get_lock();
            os
            << std::this_thread::get_id() << " : "
            << lead_in(d) << m << '(';
            emit_args(os,
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
                ret.insert(std::end(ret), model + ret.size(), model + model_size);
                return ret;
            }
            return { model + model_size - depth, model + model_size };
        }
        
        template<std::size_t...Is, class...Ts>
        static std::ostream& emit_args(std::ostream& os, std::index_sequence<Is...>, Ts&&...ts)
        {
            using expand = int[];
            void(expand{ 0, (detail::emit_comma<Is>()(os, ts),0)...});
            return os;
        }
        
        static std::size_t& depth()
        {
            static __thread std::size_t d = 0;
            return d;
        }
    };
    
    template<class Module>
    constexpr bool tracing_enabled(const Module& m)
    {
        return false;
    }
    
    namespace detail {
    template<class...Args>
    struct arg_emitter
    {
        arg_emitter(Args const &... args) : _data(args...) {}
        
        
        template<class T, std::size_t I>
        static void emit_arg(std::ostream& os, std::integral_constant<std::size_t, I>, const T& arg)
        {
            os << ", " << print(arg);
        }
        
        template<class T>
        static void emit_arg(std::ostream& os, std::integral_constant<std::size_t, 0>, const T& arg)
        {
            os << print(arg);
        }
        
        template<class Tuple, std::size_t...Is>
        static void emit(std::ostream& os, Tuple& t, std::index_sequence<Is...>)
        {
            using expand = int[];
            void(expand{ 0,
                ((emit_arg(os, std::integral_constant<std::size_t, Is>(), std::get<Is>(t))), 0)...
            });
        }
        
        void operator()(std::ostream& os) const
        {
            emit(os, _data, std::make_index_sequence<sizeof...(Args)>());
        }
        
        std::tuple<Args const&...> _data;
        
        friend std::ostream& operator<<(std::ostream& os, arg_emitter ae)
        {
            ae(os);
            return os;
        }
    };
    }
    
    template<class...Args>
    auto emit_args(const Args&...args)
    {
        return detail::arg_emitter<Args...>(args...);
    }

    
    struct conditional_tracer
    {
        constexpr conditional_tracer(bool active) : active(active) {}
        constexpr conditional_tracer(conditional_tracer&& r) : active(r.active) { r.active = false; }
        conditional_tracer& operator=(conditional_tracer&& r) { std::swap(active, r.active); return *this; }
        ~conditional_tracer() {
            if (active) {
                --depth();
            }
        }
        
        static
        std::string lead_in(std::size_t dist = depth())
        {
            constexpr const char model[] = "---------->";
            constexpr auto model_size = std::extent<decltype(model)>::value -1;
            if (dist > model_size)
            {
                std::string ret = std::to_string(dist);
                ret.insert(std::end(ret), model + ret.size(), model + model_size);
                return ret;
            }
            return { model + model_size - dist, model + model_size };
        }
        
        template<class Arg>
        decltype(auto) ret (Arg& arg)
        {
            if (active) {
                BOOST_LOG_TRIVIAL(trace) << lead_in(depth() - 1) << "returns: " << emit_args(arg);
            }
            return arg;
        }
        
        template<class Arg>
        decltype(auto) ret (Arg&& arg)
        {
            if (active) {
                BOOST_LOG_TRIVIAL(trace) << lead_in(depth() - 1) << "returns: " << emit_args(arg);
            }
            return std::move(arg);
        }
        
        template<class...Args>
        void say(const char* message, const Args&...args)
        {
            if (active) {
                BOOST_LOG_TRIVIAL(trace) << lead_in(depth()) << std::quoted(message) << " : " << emit_args(args...);
            }
        }
        


        bool active = false;
        static std::size_t& depth() {
            static __thread std::size_t _depth = 0;
            return _depth;
        }
    };

    template<class Module, class...Args>
    conditional_tracer trace(const Module& module, classname_type classname, method m, const Args&...args)
    {
        if (not tracing_enabled(module)) return { false };
        conditional_tracer tracer { true };
        auto& depth = tracer.depth();
        BOOST_LOG_TRIVIAL(trace) << tracer.lead_in(depth) << classname << "::" << m << '(' << emit_args(args...) << ')';
        ++depth;
        return tracer;
    }
    

    
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
