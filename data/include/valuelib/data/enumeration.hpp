#pragma once
#include <valuelib/immutable/string.hpp>
#include <tuple>
#include <string>
#include <boost/preprocessor.hpp>
#include <stdexcept>
#include <exception>
#include <valuelib/data/type_traits.hpp>

namespace value { namespace data {
    
    namespace detail {
        template<size_t I , size_t N>
        struct index_of
        {
            template<class Tuple>
            static size_t apply(const Tuple& t, const std::string& s)
            {
                if (std::get<I>(t) == s)
                    return I;
                else
                    return index_of<I + 1, N>::apply(t, s);
            }
        };
        
        template<size_t N>
        struct index_of<N, N>
        {
            template<class Tuple>
            [[noreturn]]
            static size_t apply(const Tuple& t, const std::string& s)
            {
                using namespace std;
                throw invalid_argument("value not valid : "s + s);
            }
        };
        
        template<size_t I , size_t N>
        struct string_of
        {
            template<class Tuple>
            static auto apply(const Tuple& t, std::size_t search)
            {
                if (search == I) {
                    const auto first = std::get<I>(t).c_str();
                    const auto last = first + std::get<I>(t).size();
                    return std::string(first, last);
                }
                else
                    return string_of<I + 1, N>::apply(t, search);
            }
        };
        
        template<size_t N>
        struct string_of<N, N>
        {
            template<class Tuple>
            [[noreturn]]
            static std::string apply(const Tuple& t, const std::size_t& s)
            {
                using namespace std;
                throw invalid_argument("value not valid : "s + std::to_string(s));
            }
        };
    }
    
    template<class...Strings>
    size_t find_index(const std::tuple<Strings...>& sequence, const std::string& s)
    {
        return detail::index_of<0, sizeof...(Strings)>::apply(sequence, s);
    };
    
    template<class...Values>
    constexpr
    auto nth_string(const std::tuple<Values...>& t, std::size_t i)
    {
        if (i >= sizeof...(Values)) {
            using namespace std;
            throw invalid_argument("value not valid : "s + std::to_string(i));
        }
        return detail::string_of<0, sizeof...(Values)>::apply(t, i);
    }
    

    template<class Tag>
    struct enumeration
    {
        using tag_type = Tag;
        using value_type = typename tag_type::values;
        using generator_type = typename tag_type::generator;
        
        constexpr enumeration(value_type e = default_value()) : _value(e) {};
        
        enumeration(const std::string& s) : _value(from_string(s)) {};
        
        constexpr operator value_type() const { return _value; }
        constexpr auto value() const { return _value; }
        
        static auto from_string(const std::string& s) {
            return static_cast<value_type>(find_index(strings(), s));
        }
        
        static constexpr auto default_value() { return static_cast<value_type>(0); }
        
        static constexpr auto strings() { return generator_type::strings(); }
        
        value_type _value;
    };
    template<class Tag>
    std::string to_string(enumeration<Tag> e)
    {
        constexpr auto strings = enumeration<Tag>::strings();
        return nth_string(strings, std::size_t(e.value()));
    }
    template<class Tag>
    std::ostream& operator<<(std::ostream& os, enumeration<Tag> e)
    {
        return os << to_string(e);
    }
    
    template<class T>
    static constexpr auto IsEnumeration = is_derived_from_template_v<T, enumeration>;
    
    namespace impl {
        template<class Type>
        struct underlying_type<Type, std::enable_if_t<IsEnumeration<Type> > >
        {
            using result = UnderlyingType<typename Type::value_type>;
        };
    }


}}
    //
    // generated code
    //
#define VALUE_DATA_IMMUTABLE_STRINGIFY(Value) #Value
#define VALUE_DATA_IMMUTABLE_STRING(Value) value::immutable::string(VALUE_DATA_IMMUTABLE_STRINGIFY(Value)),
#define VALUE_DATA_LIST_NAME(Value) Value
        
#define DEFINE_ENUM_DECL_VAL(r, name, val) val
#define DEFINE_ENUM_VAL_STR(r, name, val) value::immutable::string(BOOST_PP_STRINGIZE(val))

#define xyz x, y, z
#define xyz_strings value::immutable::string("x"),value::immutable::string("y"),value::immutable::string("z")

#define VALUE_DATA_DEFINE_ENUM(Name, val_seq)\
namespace enumeration_workspace { \
    struct Name { \
        enum class values { \
             BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(DEFINE_ENUM_DECL_VAL, Name, val_seq)) \
        }; \
        \
        struct generator { \
            static constexpr auto strings() { \
                return std::make_tuple( \
                    BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(DEFINE_ENUM_VAL_STR, Name, val_seq)) \
                ); \
            } \
        }; \
    }; \
} \
 \
using Name = value::data::enumeration<enumeration_workspace::Name>

    /*
enum class name { \
BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(DEFINE_ENUM_DECL_VAL, name, val_seq)) \
}; \
constexpr auto strings(name*) { return std::make_tuple(\
BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(DEFINE_ENUM_VAL_STR, name, val_seq)) \
); } \
constexpr auto from_string(const std::string& s, name* = nullptr) { \
return value::data::from_string<name>(s); \
} \
auto to_string(name e) { \
    return value::data::nth_string(strings(&e), static_cast<std::size_t>(e)); \
}\
std::ostream& operator<<(std::ostream& os, name e) {\
return os << to_string(e); \
}
*/

