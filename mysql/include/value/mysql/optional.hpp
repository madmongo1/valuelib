#pragma once

#define VALUE_MYSQL_OPTIONAL_CONSIDER_STD 1
#define VALUE_MYSQL_OPTIONAL_CONSIDER_STD_EXPERIMENTAL 1
#define VALUE_MYSQL_OPTIONAL_CONSIDER_BOOST 1

// #define VALUE_MYSQL_OPTIONAL_DEFINED

#if VALUE_MYSQL_OPTIONAL_CONSIDER_STD and not defined(VALUE_MYSQL_OPTIONAL_DEFINED)

    #if __has_include(<optional>)

        #include <optional>
        #define VALUE_MYSQL_OPTIONAL_USE_STD
        #define VALUE_MYSQL_OPTIONAL_DEFINED

    #endif

#endif

#if not defined(VALUE_MYSQL_OPTIONAL_DEFINED) and VALUE_MYSQL_OPTIONAL_CONSIDER_STD_EXPERIMENTAL

    #if __has_include(<experimental/optional>)

        #include <experimental/optional>
        #define VALUE_MYSQL_OPTIONAL_USE_STD_EXPERIMENTAL
        #define VALUE_MYSQL_OPTIONAL_DEFINED

    #endif

#endif

#if not defined(VALUE_MYSQL_OPTIONAL_DEFINED) and VALUE_MYSQL_OPTIONAL_CONSIDER_BOOST

    #if __has_include(<boost/optional.hpp>)

        #include <boost/optional.hpp>
        #define VALUE_MYSQL_OPTIONAL_USE_BOOST
        #define VALUE_MYSQL_OPTIONAL_DEFINED
    #endif

#endif

#if not defined(VALUE_MYSQL_OPTIONAL_DEFINED)

    #error "Cannot configure optional"

#endif


namespace value { namespace mysql {

#if defined(VALUE_MYSQL_OPTIONAL_USE_STD)

    using std::optional;
    static constexpr auto nullopt = std::nullopt;
    using std::make_optional;

#endif
    
#if defined(VALUE_MYSQL_OPTIONAL_USE_STD_EXPERIMENTAL)

    using std::experimental::optional;
    static constexpr auto nullopt = std::experimental::nullopt;
    using std::experimental::make_optional;

#endif
    
#if defined(VALUE_MYSQL_OPTIONAL_USE_BOOST)
    
    using boost::optional;
    const auto nullopt = boost::none;
    using boost::make_optional;

#endif
    
}}