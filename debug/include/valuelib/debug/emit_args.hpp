#pragma once

#include <valuelib/debug/print.hpp>

namespace value { namespace debug {
    
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

}}