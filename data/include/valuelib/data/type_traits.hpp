#pragma once
#include <boost/optional/optional_fwd.hpp>
#include <utility>

namespace value { namespace data {
    
    template <typename T, template <typename, typename...> class Tmpl>  // #1 see note
    struct is_derived_from_template
    {
        typedef char yes[1];
        typedef char no[2];
        
        static no & test(...);
        
        template <typename ...U>
        static yes & test(Tmpl<U...> const &);
        
        static bool constexpr value = sizeof(test(std::declval<T>())) == sizeof(yes);
    };
    template<typename T, template <typename, typename...> class Tmpl>
    static constexpr bool is_derived_from_template_v = is_derived_from_template<T, Tmpl>::value;
    
    
    namespace impl {
        // default case - underlying type is the same as type
        template<class T, typename = void>
        struct underlying_type
        {
            using result = T;
        };
    }
    
    template<class T> using UnderlyingType = typename impl::underlying_type<T>::result;
    
    namespace impl {
        template<class T> struct underlying_type<boost::optional<T>, void> {
            using result = UnderlyingType<T>;
        };
    }
    
}}