#pragma once

#include <ostream>
#include <iomanip>

namespace value { namespace debug {
    
    template<class T, typename = void>
    struct printer;
    
    template<class T>
    struct printer_call
    {
        auto operator()(std::ostream& os) const
        -> std::ostream&
        {
            printer<std::decay_t<T>>()(os, _t);
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
    
    
    
    
    
    
    template<>
    struct printer<bool>
    {
        void operator()(std::ostream& os, bool b) const
        {
            os << std::boolalpha << b;
        }
    };
    
    template<class T>
    struct printer<T, std::enable_if_t< std::is_arithmetic<T>::value >>
    {
        void operator()(std::ostream& os, T const& t) const
        {
            os << t;
        }
    };
    
    template<class T>
    struct has_ostream
    {
        template<class U> static auto test(const U* p) -> decltype((std::declval<std::ostream>() << (*p)), void(), std::true_type());
        template<class U> static auto test(...) -> decltype(std::false_type());
        using type = decltype(test<T>(0));
        static constexpr bool value = type::value;
    };
    template<class T> static constexpr bool HasOStream = has_ostream<T>::value;
    
    template<class T>
    struct has_as_object {
        template<class U> static auto test(const U*p) -> decltype(as_object(*p), void(), std::true_type());
        template<class U> static auto test(...) -> decltype(std::false_type());
        using type = decltype(test<T>(nullptr));
        static constexpr bool value = type::value;
    };
    template<class T> using has_as_object_t = typename has_as_object<T>::type;
    template<class T> static constexpr bool HasAsObject = has_as_object<T>::value;
    
    template<class T>
    struct printable {
        template<class U> static auto test(const U*p) -> decltype(print(*p), void(), std::true_type());
        template<class U> static auto test(...) -> decltype(std::false_type());
        using type = decltype(test<T>(nullptr));
        static constexpr bool value = type::value;
    };
    template<class T> using printable_t = typename printable<T>::type;
    template<class T> static constexpr bool Printable = has_as_object<T>::value;
    
    template<class T>
    struct printer<T*, std::enable_if_t<Printable<std::decay_t<T>>>>
    {
        void operator()(std::ostream& os, T* t) const
        {
            if (t)
                os << print(*t);
            else
                os << "null";
        }
    };
    
    template<class T>
    struct printer<T*, std::enable_if_t<not Printable<std::decay_t<T>>>>
    {
        void operator()(std::ostream& os, T* t) const
        {
            os << t;
        }
    };
    
    
    
    template<class T>
    struct printer<T, std::enable_if_t<HasAsObject<T>>>
    {
        void operator()(std::ostream& os, T const& t) const
        {
            os << as_object(t);
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
        void operator()(std::ostream& os, const std::shared_ptr<T>& ptr) const
        {
            if (auto p = ptr.get())
            {
                os << print(*ptr);
            }
            else
            {
                os << "null";
            }
        }
    };
}}

