#pragma once

namespace value { namespace data {
  
    template <typename T, template <typename, typename...> class Tmpl>  // #1 see note
    struct is_derived_from_template
    {
        typedef char yes[1];
        typedef char no[2];
        
        static no & test(...);
        
        template <typename U>
        static yes & test(Tmpl<U> const &);
        
        static bool constexpr value = sizeof(test(std::declval<T>())) == sizeof(yes);
    };
    template<typename T, template <typename, typename...> class Tmpl>
    static constexpr bool is_derived_from_template_v = is_derived_from_template<T, Tmpl>::value;

    
}}