#pragma once

#include <ostream>
#include <iomanip>

namespace value { namespace debug {
    
    namespace detail {
        template<class T>
        struct is_ostreamable
        {
            template<class Y> static auto test(const Y* p) -> decltype(std::declval<std::ostream&>() << (*p), void(), std::true_type());
            template<class Y> static auto test(...) -> decltype(std::false_type());
            
            using type = decltype(test((const T*)0));
            static constexpr auto value = type::value;
        };
    }
    template<class T> using is_ostreamable = typename detail::is_ostreamable<T>::type;
    
    namespace detail {
        template<class T>
        struct has_as_object {
            template<class U> static auto test(const U*p) -> decltype(as_object(*p), void(), std::true_type());
            template<class U> static auto test(...) -> decltype(std::false_type());
            using type = decltype(test<T>(nullptr));
            static constexpr bool value = type::value;
        };
    }
    template<class T> using has_as_object = typename detail::has_as_object<T>::type;
    
    namespace detail {
        template<class T>
        struct has_debug_print {
            template<class U> static auto test(U*p) -> decltype(debug_print(std::declval<std::ostream&>(), *p), void(), std::true_type());
            template<class U> static auto test(...) -> decltype(std::false_type());
            using type = decltype(test<T>(nullptr));
            static constexpr bool value = type::value;
        };
    }
    template<class T> using has_debug_print = typename detail::has_debug_print<T>::type;
    
    
    
    template<class T>
    struct printer;
    
    template<class T>
    struct printer
    {
        template<class U>
        auto operator()(std::ostream& os, U const& u)
        -> std::enable_if_t<has_as_object<U>::value and not has_debug_print<U>::value, std::ostream&>
        {
            os << as_object(u);
            return os;
        }

        template<class U>
        auto operator()(std::ostream& os, U const& u)
        -> std::enable_if_t<has_as_object<U>::value and has_debug_print<U>::value, std::ostream&>
        {
            debug_print(os, u);
            return os;
        }

        template<class U>
        auto operator()(std::ostream& os, U const& u)
        -> std::enable_if_t<not has_as_object<U>::value and has_debug_print<U>::value, std::ostream&>
        {
            debug_print(os, u);
            return os;
        }

        template<class U>
        auto operator()(std::ostream& os, U const& u)
        -> std::enable_if_t<not has_as_object<U>::value and not has_debug_print<U>::value, std::ostream&>
        {
            os << std::addressof(u);
            return os;
        }
    };
    
    // specialise for straight-through types
    
    template<>
    struct printer<bool>
    {
        void operator()(std::ostream& os, bool b) const
        {
            os << std::boolalpha << b;
        }
    };
    
    template<>
    struct printer<int>
    {
        void operator()(std::ostream& os, int const& v) const
        {
            os << v;
        }
    };
    
    
    template<class T>
    struct printer_call
    {
        auto operator()(std::ostream& os) const
        -> std::ostream&
        {
            printer<T>()(os, _t);
            return os;
        }
        
        T const& _t;
        
        friend auto operator<<(std::ostream& os, printer_call const& tp)
        -> std::ostream&
        {
            return tp(os);
        }
    };
    
    template<class T>
    auto print(T const& t)
    {
        return printer_call<T> { t };
    }
    
    namespace detail {
        template<class T>
        struct is_printable
        {
            template<class U> static auto test(const U*p)
            -> decltype(std::declval<printer<T>>()(std::declval<std::ostream&>(), *p), void(), std::true_type());
            
            template<class U> static auto test(...) -> decltype(std::false_type());
            using type = decltype(test<T>(nullptr));
            static constexpr bool value = type::value;
        };
    }
    template<class T> using is_printable = typename detail::is_printable<T>::type;
    
    
    template<class T>
    struct printer<T*>
    {
        std::ostream& operator()(std::ostream& os, T* t) const
        {
            if (t)
            {
                printer<T> p;
                p(os, *t);
            }
            else
                os << "null";
            return os;
        }
    };
    
}}


#include <iomanip>
#include <string>

namespace value { namespace debug {
    
    template<class Ch, class Traits, class Alloc>
    struct printer<std::basic_string<Ch, Traits, Alloc>>
    {
        void operator()(std::ostream& os,
                        std::basic_string<Ch, Traits, Alloc> const& s) const
        {
            os << std::quoted(s);
        }
    };
    
    template<std::size_t N>
    struct printer<const char (&)[N]>
    {
        void operator()(std::ostream& os,
                        const char (&s)[N]) const
        {
            os << std::quoted(s);
        }
    };

    template<>
    struct printer<const char*>
    {
        void operator()(std::ostream& os,
                        const char* s) const
        {
            os << std::quoted(s);
        }
    };
    
    
}}

#include <tuple>
namespace value { namespace debug {
    template<class...Ts>
    struct printer<std::tuple<Ts...>>
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
            os << '[';
            using expand = int[];
            void(expand{0,
                ((os << sep(std::integral_constant<std::size_t, Is>())),
                 (os << print(std::get<Is>(t))),
                 0)...
            });
            os << " ]";
        }
        
        template<class sCh, class sTraits>
        void operator()(std::basic_ostream<sCh, sTraits>& os,
                        std::tuple<Ts...> const& s) const
        {
            emit(os, s, std::make_index_sequence<sizeof...(Ts)>());
        }
        
    };
}}

#include <memory>

namespace value { namespace debug {
    
    template<class T>
    struct printer<std::shared_ptr<T>>
    {
        decltype(auto) operator()(std::ostream& os, const std::shared_ptr<T>& ptr) const
        {
            return printer<T*>()(os, ptr.get());
        }
    };
}}

