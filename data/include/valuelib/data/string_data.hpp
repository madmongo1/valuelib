//
//  Header.h
//  ProjectX
//
//  Created by Richard Hodges on 25/02/2016.
//
//
#pragma once

namespace value { namespace data {
    template<class Identifier>
    struct storable_data_tag {};
    
    template<class Outer, class Type> struct storable_data;
    
    // storage tags
    struct storage_capability_string {};
    struct storage_capability_binary_stream {};
    struct storage_capability_string_stream {};
    
    
    template<class Identifier>
    struct storable_data<Identifier, std::string> : std::string
    {
        using std::string::string;
        
        struct storage_tags
        : storage_capability_string_stream
        , storage_capability_string
        {};
    };
    
    inline std::vector<std::uint8_t> byte_vector_from_stream(std::istream& is) {
        using namespace std;
        static std::atomic<std::size_t> _biggest_seen { 0 };
        auto biggest_seen = _biggest_seen.load();
        auto v = vector<uint8_t>();
        v.reserve(biggest_seen);
        auto saved_flags = is.setf(0, ios_base::skipws);
        copy(istream_iterator<uint8_t>(is),
             istream_iterator<uint8_t>(),
             back_inserter(v));
        is.setf(saved_flags, ios_base::skipws);
        auto my_size = v.size();
        if (my_size > biggest_seen)
        {
            _biggest_seen.compare_exchange_strong(biggest_seen, my_size);
        }
        return v;
    }
    
    template<class Identifier>
    struct storable_data<Identifier, std::vector<std::uint8_t>> : std::vector<std::uint8_t>
    {
        using underlying_type = std::vector<std::uint8_t>;
        using underlying_type::underlying_type;
        using identity_tag = storable_data_tag<Identifier>;
        using storable_data_type = storable_data<Identifier, std::vector<std::uint8_t>>;
        
        static constexpr auto identifier() { return Identifier::identifier(); }
        
        struct storage_tags
        : storage_capability_binary_stream
        {};
        
        friend void to_binary_stream(std::ostream& os, const Identifier& r)
        {
            os.write(r.data(), r.size());
        }
        
        friend void from_binary_stream(std::istream& is, identity_tag)
        {
            return Identifier(byte_vector_from_stream(is));
        }
        
        friend std::ostream& operator<<(std::ostream& os, const storable_data_type& sd)
        {
            auto tup = std::make_tuple(sd.identifier(),
                                       static_cast<const underlying_type&>(sd));
            auto printer = value::tuple::print_tuple(tup);
            return os << printer;
        }
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