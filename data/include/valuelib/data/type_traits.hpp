#pragma once
#include <boost/optional/optional_fwd.hpp>
#include <utility>

namespace value { namespace data {
    
#define VALUE_DATA_OP_CHECK(Name, Symbol) \
template<class L, class R, typename = decltype(std::declval<L>() Symbol std::declval<R>())> \
static constexpr auto Name() { return true; } \
template<class L, class R, typename ...> \
static constexpr auto Name() { return false; }
    
    /*
    template<class T, class U>
    struct supports_eq
    {
        template<class X, class Y>
        static auto test(int) -> decltype((std::declval<X>() == std::declval<Y>()), void(), std::true_type());

        template<typename, typename>
        static auto test() -> std::false_type;
        
        static constexpr bool value = decltype(test<T, U>(0))::value;
    };
    template<class X, class Y> static constexpr auto SupportsEq = supports_eq<X, Y>::value;
*/
#define VALUE_DATA_SUPPORTS_EQ_TEST(NAME, OPERATOR) \
    template<class T, class U> \
    struct supports_##NAME \
    { \
        template<class X, class Y> \
        static auto test(int) -> decltype((std::declval<X>() OPERATOR std::declval<Y>()), void(), std::true_type()); \
\
        template<typename, typename> \
        static auto test() -> std::false_type; \
\
        static constexpr bool value = decltype(test<T, U>(0))::value; \
    }; \
    template<class X, class Y> static constexpr auto Supports##NAME = supports_##NAME<X, Y>::value;

    VALUE_DATA_SUPPORTS_EQ_TEST(Eq, ==);
    VALUE_DATA_SUPPORTS_EQ_TEST(Lt, <);
    VALUE_DATA_SUPPORTS_EQ_TEST(Ne, !=);
    VALUE_DATA_SUPPORTS_EQ_TEST(Le, <=);
    VALUE_DATA_SUPPORTS_EQ_TEST(Gt, >);
    VALUE_DATA_SUPPORTS_EQ_TEST(Ge, >=);
    
    struct op_check {
        
        VALUE_DATA_OP_CHECK(eq, ==)
        VALUE_DATA_OP_CHECK(less, <)
        VALUE_DATA_OP_CHECK(ne, !=)
        VALUE_DATA_OP_CHECK(le, <=)
        VALUE_DATA_OP_CHECK(gt, >)
        VALUE_DATA_OP_CHECK(ge, >=)
        VALUE_DATA_OP_CHECK(plus_equals, +=)
        
        template<class R, typename = decltype(std::hash<std::decay_t<R>>(std::declval<R>()))>
        static constexpr auto hash() { return true; }
        template<class R, typename ...>
        static constexpr auto hash() { return false; }
        
        template<class R, typename = decltype(to_string(std::declval<R>()))>
        static constexpr auto to_string() { return true; }
        template<class R, typename ...>
        static constexpr auto to_string() { return false; }
    };

    
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