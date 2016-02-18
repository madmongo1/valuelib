#pragma once
#include <tuple>
#include <utility>
#include <string>
#include <boost/operators.hpp>
#include <valuelib/immutable/string.hpp>

namespace value { namespace data {

#define VALUE_DATA_OP_CHECK(Name, Symbol) \
template<class L, class R, typename = decltype(std::declval<L>() Symbol std::declval<R>())> \
    static constexpr auto Name() { return true; } \
    template<class L, class R, typename ...> \
    static constexpr auto Name() { return false; }

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
    
    template<class Tag, class Type>
    struct field_type
    : boost::equality_comparable<field_type<Tag, Type>>
    , boost::less_than_comparable<field_type<Tag, Type>>
    {
        using tag_type = Tag;
        using value_type = Type;
        using implementation_type = field_type;
        
        field_type() : _value() {};
        
        template<
        class...Args,
        typename std::enable_if_t<std::is_constructible<value_type, Args...>::value>* = nullptr
        >
        explicit field_type(Args&&... args)
        : _value(std::forward<Args>(args)...)
        {}
        
        value_type const& value() const { return _value; }
        operator value_type const& () const { return _value; }
        
    private:
        value_type _value;
    };
    
#define VALUE_DATA_IMPLEMENT_FIELD_OPERATOR(Name, Symbol) \
template<class Tag, class Type, class Right, std::enable_if_t<op_check::Name<Type, Right>()>* = nullptr> \
    auto operator Symbol (field_type<Tag, Type> const& l, Right const& r) { \
        return l.value() Symbol r; \
    }
    
    VALUE_DATA_IMPLEMENT_FIELD_OPERATOR(eq, ==)
    VALUE_DATA_IMPLEMENT_FIELD_OPERATOR(less, <)
    VALUE_DATA_IMPLEMENT_FIELD_OPERATOR(le, <=)
    VALUE_DATA_IMPLEMENT_FIELD_OPERATOR(gt, >)
    VALUE_DATA_IMPLEMENT_FIELD_OPERATOR(ge, >=)
    VALUE_DATA_IMPLEMENT_FIELD_OPERATOR(ne, !=)

    template<class Tag, class Type>
    std::ostream& operator<<(std::ostream& os, const field_type<Tag, Type>& f)
    {
        return os << f.value();
    }
    
    template<class Tag, class Type>
    bool operator==(const field_type<Tag, Type>& l, const field_type<Tag, Type>& r)
    {
        return l.value() == r.value();
    }
    
    template<class Tag, class Type>
    bool operator<(const field_type<Tag, Type>& l, const field_type<Tag, Type>& r)
    {
        return l.value() < r.value();
    }
    template<class Tag, class Type, class Enabled = void>
    struct hasher;
    
    template<class Tag, class Type>
    struct hasher<Tag, Type, std::enable_if_t<op_check::hash<Type>()>>
    {
        using result_type = std::size_t;
        using argument_type = field_type<Tag, Type>;
        result_type operator()(const argument_type& r) const {
            return std::hash<Type>(r.value());
        }
    };
    
    //
    // to_string
    //
    
    template<class Tag, class Value, std::enable_if_t<op_check::to_string<Value>()>>
    std::string to_string(const field_type<Tag, Value>& r)
    {
        using namespace std;
        return to_string(r.value());
    }
    
    template<class Tag>
    const std::string& to_string(const field_type<Tag, std::string>& r)
    {
        using namespace std;
        return r.value();
    }
    
}}
namespace std {

    
    template<class Tag, class Type>
    struct hash<value::data::field_type<Tag, Type>>
    : value::data::hasher<Tag, Type>
    {};
    
}

#define VALUE_DATA_DEFINE_FIELD(Identifier, Type) \
namespace field_workspace { \
    struct Identifier \
    { \
        static constexpr auto identifier = value::immutable::string(#Identifier); \
    }; \
} \
using Identifier = value::data::field_type<field_workspace::Identifier, Type>

