//
//  Header.h
//  ProjectX
//
//  Created by Richard Hodges on 25/02/2016.
//
//
#pragma once
#include <valuelib/tuple/print.hpp>
#include <valuelib/tuple/metafunction.hpp>

namespace value { namespace data {
    // introduce storage idea
    template<class T>
    struct storage_traits
    {
        using connector_write_interface = std::tuple<>;
        using connector_read_interface = std::tuple<>;
    };
    
    template<class Type>
    using StorageTraits = storage_traits<std::decay_t<Type>>;
    
    template<class Type, class Interface>
    static constexpr bool SupportsWrite =
    value::tuple::TupleHasType
    <
    typename StorageTraits<Type>::connector_write_interface,
    Interface
    >;
    
    template<class Type, class Interface>
    static constexpr bool SupportsRead =
    value::tuple::TupleHasType
    <
    typename StorageTraits<Type>::connector_read_interface,
    Interface
    >;
    
    template<class Identifier, class Type, typename Enable = void>
    struct storable_data
    {
        using underlying_type = Type;
        using storable_data_type = storable_data<Identifier, Type>;
        using storage_traits = storage_traits<underlying_type>;
        
        static constexpr auto identifier() { return Identifier::identifier(); }
        
        // constructors
        template<
        class...Args,
        typename std::enable_if_t<std::is_constructible<underlying_type, Args...>::value>* = nullptr
        >
        explicit storable_data(Args&&... args)
        : _value(std::forward<Args>(args)...)
        {}
        
        // convert to underlying type
        const underlying_type& value() const { return _value; }
        underlying_type& value() { return _value; }
        
        operator const underlying_type&() { return value(); }
        operator underlying_type&() { return value(); }
        
    private:
        underlying_type _value;
    };
    
    template<class Identifier, class Type>
    std::ostream& operator<<(std::ostream& os, const storable_data<Identifier, Type>& sd)
    {
        os << sd.identifier();
        os << " : ";
        os << value::tuple::print_tuple(sd.value());
        return os;
    }
#define VALUE_DATA_STORABLE_DATA_OPERATOR(NAME, SYMBOL) \
\
    template<class Identifier, class Type, std::enable_if_t<Supports##NAME<Type, Type>>* = nullptr> \
    decltype(auto) operator SYMBOL(const storable_data<Identifier, Type>& l, \
                    const storable_data<Identifier, Type>& r) \
    { \
        return l.value() SYMBOL r.value(); \
    } \
\
    template<class Identifier, class Type, std::enable_if_t<Supports##NAME<Type, Type>>* = nullptr> \
    decltype(auto) operator SYMBOL(const storable_data<Identifier, Type>& l, \
                    const Type& r) \
    { \
        return l.value() SYMBOL r; \
    }
    VALUE_DATA_STORABLE_DATA_OPERATOR(Eq, ==)
    VALUE_DATA_STORABLE_DATA_OPERATOR(Lt, <)
    VALUE_DATA_STORABLE_DATA_OPERATOR(Le, <=)
    VALUE_DATA_STORABLE_DATA_OPERATOR(Gt, >)
    VALUE_DATA_STORABLE_DATA_OPERATOR(Ge, >=)
    VALUE_DATA_STORABLE_DATA_OPERATOR(Ne, !=)
    
    template<class Container>
    struct to_byte_stream
    {
        using argument_type = const Container&;
        using result_type = std::ostream&;
        
        result_type operator()(std::ostream& os, argument_type vec) const {
            return os.write(reinterpret_cast<const char*>(vec.data()), vec.size());
        }
    };
    
    struct flag_saver {
        flag_saver(std::istream& is) : _is(is), _flags(is.flags()) {}
        ~flag_saver() { try { _is.flags(_flags); } catch(...) { } }
        std::istream& _is;
        std::ios_base::fmtflags _flags;
    };

    
    template<class Container> struct from_byte_stream
    {
        using result_type = Container;
        using argument_type = std::istream&;
        result_type operator()(argument_type is) const
        {
            using namespace std;
            auto v = Container();
            flag_saver saver(is);
            is.unsetf(ios_base::skipws);
            copy(istream_iterator<typename Container::value_type>(is),
                 istream_iterator<typename Container::value_type>(),
                 back_inserter(v));
            return v;
        }
    };

    template<class Identifier,
    std::enable_if_t
    <
    is_derived_from_template_v<Identifier, storable_data> and
    value::data::SupportsWrite<typename Identifier::underlying_type, std::ostream>
    > * = nullptr
    >
    void write_to_connector(std::ostream& os, const Identifier& sd)
    {
        auto cmd = to_byte_stream<typename Identifier::underlying_type>();
        cmd(os, sd.value());
    }
    
    template<class Identifier,
    std::enable_if_t
    <
    is_derived_from_template_v<Identifier, storable_data> and
    value::data::SupportsRead<typename Identifier::underlying_type, std::istream>
    > * = nullptr
    >
    auto read_from_connector(std::istream& is)
    {
        auto cmd = from_byte_stream<typename Identifier::underlying_type>();
        return Identifier(cmd(is));
    }
    
    
    template<class A>
    struct from_byte_stream<std::vector<std::uint8_t, A>>
    {
        using result_type = std::vector<std::uint8_t, A>;
        using argument_type = std::istream&;
        result_type operator()(argument_type is) const
        {
            using namespace std;
            auto v = vector<uint8_t>();
            flag_saver saver(is);
            is.unsetf(ios_base::skipws);
            copy(istream_iterator<uint8_t>(is),
                 istream_iterator<uint8_t>(),
                 back_inserter(v));
            return v;
        }
    };
    
    template<class A>
    struct to_byte_stream<std::vector<std::uint8_t, A>>
    {
        using argument_type = const std::vector<std::uint8_t, A>&;
        using result_type = std::ostream&;
        
        result_type operator()(std::ostream& os, argument_type vec) const {
            return os.write(reinterpret_cast<const char*>(vec.data()), vec.size());
        }
    };
    
    
    template<class A>
    struct storage_traits<std::vector<std::uint8_t, A>>
    {
        using connector_write_interface = std::tuple<std::ostream>;
        using connector_read_interface = std::tuple<std::istream>;
    };
    
    template<class Traits, class Allocator>
    struct storage_traits<std::basic_string<char, Traits, Allocator>>
    {
        using connector_write_interface = std::tuple<std::ostream, std::string>;
        using connector_read_interface = std::tuple<std::istream, std::string>;
    };
    
    template<>
    struct storage_traits<boost::uuids::uuid>
    {
        using connector_write_interface = std::tuple<std::string>;
        using connector_read_interface = std::tuple<std::string>;
    };
    
    template<>
    struct storage_traits<boost::posix_time::ptime>
    {
        using connector_write_interface = std::tuple<std::string>;
        using connector_read_interface = std::tuple<std::string>;
    };
    
    namespace impl {
        
        template<class T, typename Enable = void>
        struct is_storable_data : std::false_type {};
        
        template<class T>
        struct is_storable_data<T, std::enable_if_t< is_derived_from_template_v<T, storable_data> >>
        : std::true_type {};
        
    }
    
    template<class T> constexpr auto IsStorableData = impl::is_storable_data<T>::value;
}}