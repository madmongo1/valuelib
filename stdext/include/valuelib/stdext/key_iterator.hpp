#pragma once

#include <utility>

namespace value { namespace stdext {
    
    namespace detail {
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
    }
    template<typename T, template <typename, typename...> class Tmpl>
    static constexpr bool IsDerivedFromTemplate = detail::is_derived_from_template<T, Tmpl>::value;
    
    template<class ValueType, typename Enable = void>
    struct key_iterator;
    
    template<class Iter>
    struct key_iterator<Iter, std::enable_if_t< IsDerivedFromTemplate<typename Iter::value_type, std::pair>  >> : Iter
    {
        key_iterator(Iter i) : Iter(i) {};
        using underlying_value_type = typename Iter::value_type;
        using value_type = typename underlying_value_type::first_type;
        using reference = std::add_lvalue_reference_t<value_type>;
        using pointer = std::add_pointer_t<value_type>;
        
        decltype(auto) operator*() const { return Iter::operator*().first; }
    };
    
    template<class Iter>
    struct key_iterator<Iter, std::enable_if_t< not IsDerivedFromTemplate<typename Iter::value_type, std::pair>  >> : Iter
    {
        key_iterator(Iter i) : Iter(i) {};
    };
    
    
    template<class Iter>
    auto make_key_iterator(Iter iter)
    {
        return key_iterator<std::decay_t<Iter>>(iter);
    }
    
    namespace detail {
        
        template<class T, typename Enable = void> struct key_type {
            using type = T;
        };

        template<class T> struct key_type<T, std::enable_if_t<IsDerivedFromTemplate<T, std::pair>>>
        {
            using type = typename T::first_type;
        };
        
        
        template<class T> struct has_begin
        {
            template<class X> static auto test(X*) -> decltype(std::begin(std::declval<X>()), void(), std::true_type());
            template<class X> static auto test(...) -> decltype(std::false_type());
            static constexpr auto value = decltype(test<T>(0))::value;
        };

        template<class T> struct has_end
        {
            template<class X> static auto test(X*) -> decltype(std::end(std::declval<X>()), void(), std::true_type());
            template<class X> static auto test(...) -> decltype(std::false_type());
            static constexpr auto value = decltype(test<T>(0))::value;
        };
    }
    
    template<class T> using KeyType = typename detail::key_type<std::decay_t<T>>::type;
    
    template<class T> static constexpr auto HasBegin = detail::has_begin<std::decay_t<T>>::value;
    template<class T> static constexpr auto HasEnd = detail::has_end<std::decay_t<T>>::value;
    template<class T> static constexpr auto IsRange = HasBegin<T> and HasEnd<T>;
    
    template<class Range, std::enable_if_t<IsRange<Range>>* = nullptr>
    auto all_keys(Range& r)
    {
        using key_type = std::decay_t<KeyType<typename Range::value_type>>;
        std::vector<key_type> result;
        std::copy(make_key_iterator(std::begin(r)),
                  make_key_iterator(std::end(r)),
                  std::back_inserter(result));
        return result;
    }
}}
