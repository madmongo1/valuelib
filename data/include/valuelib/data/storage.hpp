#pragma once
#include "field.hpp"
#include "enumeration.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>
#include "metafunctions.hpp"
#include <type_traits>
#include "deduce_native.hpp"
#include <valuelib/data/storable_data.hpp>

namespace value { namespace data {
    
    //
    // the concept of nullable
    //
    struct nullable_tag {};
    struct nullable { using tag_type = nullable_tag; };
    struct not_null { using tag_type = nullable_tag; };
    
    template<class Nullable>
    struct implement_nullable
    {
        static constexpr auto nullable() { return Nullable(); }
        static_assert(std::is_same<Nullable, ::value::data::nullable>::value or std::is_same<Nullable, not_null>::value,
                      "Nullable must be either nullable or not_null");
    };
    
    //
    // the concept of default
    //
    
    struct no_default {};
    struct current_timestamp {};
    // a special tag that tells the table to get it's default from the ::default_value() function
    // on the native type (usually most useful for enumerations)
    struct default_from_native {};
    
    
    
    //
    // the concept of limited length
    //
    struct size_limit_tag {};
    
    template<std::size_t Limit>
    struct limited_length
    {
        using tag_type = size_limit_tag;
        static constexpr std::size_t limit = Limit;
    };
    
    // note: unlimited length might imply an inability to index on this field
    struct unlimited_length
    {
        using tag_type = size_limit_tag;
    };
    
    // note: allow the sql dialect to choose a reasonable length limit
    struct reasonable_length
    {
        using tag_type = size_limit_tag;
        static constexpr bool is_limited = false;
    };
    
    template<std::size_t Limit>
    struct fixed_length
    {
        static constexpr std::size_t limit = Limit;
    };
    
    ///
    /// @tparam NativeType is the type that the program will use to manipulate the data
    /// @tparam LengthLimit is the type that describes any length limit on this data
    /// @tparam Nullable is the type that describes whether the field may be null.
    ///         if so, data access will be in terms of optional<NativeType>
    ///
    template<class NativeType, class LengthLimit, class Nullable>
    struct string_storage : implement_nullable<Nullable>
    {
        struct attribute_tags : nullable_tag, size_limit_tag {};
        
        using length_limit_type = LengthLimit;
        using nullable_type = Nullable;
        using native_type = NativeType;
    };
    
    template<class NativeType, class LengthLimit>
    struct deduce_native_argument<string_storage<NativeType, LengthLimit, not_null>>
    {
        using result = NativeType;
    };
    
    template<class NativeType, class LengthLimit>
    struct deduce_native_argument<string_storage<NativeType, LengthLimit, nullable>>
    {
        using result = boost::optional<NativeType>;
    };

    
    template<class NativeType, class Nullable>
    struct uuid_storage : implement_nullable<Nullable>
    {
        struct attribute_tags : nullable_tag {};
        
        using nullable_type = Nullable;
        using native_type = NativeType;
    };
    
    namespace metafunction { namespace impl {
        template<class NativeType>
        struct native_arg_type<uuid_storage<NativeType, nullable>>
        {
            using argument_type = uuid_storage<NativeType, nullable>;
            using result = boost::optional<typename argument_type::native_type>;
        };
        template<class NativeType>
        struct native_arg_type<uuid_storage<NativeType, not_null>>
        {
            using argument_type = uuid_storage<NativeType, nullable>;
            using result = typename argument_type::native_type;
        };
    }}
    
    template<class NativeType>
    struct deduce_native_argument<uuid_storage<NativeType, not_null>>
    {
        using result = NativeType;
    };
    
    template<class NativeType>
    struct deduce_native_argument<uuid_storage<NativeType, nullable>>
    {
        using result = boost::optional<NativeType>;
    };
    
    template<class NativeType, class Nullable = not_null, class DefaultValue = no_default>
    struct timestamp_storage : implement_nullable<Nullable>
    {
        struct attribute_tags : nullable_tag {};

        // default types
        
        using nullable_type = Nullable;
        using native_type = NativeType;
    };

    template<class NativeType>
    struct deduce_native_argument<timestamp_storage<NativeType, not_null>>
    {
        using result = NativeType;
    };
    
    template<class NativeType>
    struct deduce_native_argument<timestamp_storage<NativeType, nullable>>
    {
        using result = boost::optional<NativeType>;
    };

    ///
    /// @tparam NativeType is the type that the program will use to manipulate the data
    /// @tparam Nullable is the type that describes whether the field may be null.
    ///         if so, data access will be in terms of optional<NativeType>
    ///
    template<class NativeType, class Nullable>
    struct text_set_storage : implement_nullable<Nullable>
    {
        struct attribute_tags : nullable_tag {};
        
        using nullable_type = Nullable;
        using native_type = NativeType;
        
        static constexpr auto get_native_type() { return NativeType(); }
        static constexpr auto strings() { return get_native_type().strings(); }
    };
    
    template<class NativeType>
    struct deduce_native_argument<text_set_storage<NativeType, not_null>>
    {
        using result = NativeType;
    };
    
    template<class NativeType>
    struct deduce_native_argument<text_set_storage<NativeType, nullable>>
    {
        using result = boost::optional<NativeType>;
    };
    
    //
    // binary object storage
    //
    
    template<class NativeType, class LengthLimit, class Nullable>
    struct binary_storage : implement_nullable<Nullable>
    {
        using length_limit_type = LengthLimit;
        using native_type = NativeType;
    };
    
    template<class NativeType, class LengthLimit>
    struct deduce_native_argument<binary_storage<NativeType, LengthLimit, not_null>>
    {
        using result = NativeType;
    };
    
    template<class NativeType, class LengthLimit>
    struct deduce_native_argument<binary_storage<NativeType, LengthLimit, nullable>>
    {
        using result = boost::optional<NativeType>;
    };

    
    namespace detail
    {
        template<class Field, typename Enable = void> struct default_storage;
        
        template<class Tag>
        struct default_storage< field_type<Tag, std::string> >
        {
            using type = string_storage<
            field_type<Tag, std::string>,
            limited_length<64>,
            not_null
            >;
        };
        
        template<class Tag>
        struct default_storage< field_type<Tag, boost::uuids::uuid> >
        {
            using type = uuid_storage<
            field_type<Tag, boost::uuids::uuid>,
            not_null
            >;
        };
        
        template<class Tag>
        struct default_storage< field_type<Tag, boost::posix_time::ptime> >
        {
            using type = timestamp_storage<
            field_type<Tag, boost::posix_time::ptime>,
            not_null
            >;
        };
        
        template<class Tag>
        struct default_storage< enumeration<Tag> >
        {
            using type = text_set_storage<
            enumeration<Tag>,
            not_null
            >;
        };
        
        template<class Tag>
        struct default_storage< field_type<Tag, std::vector<std::uint8_t>> >
        {
            using type = binary_storage<
            field_type<Tag, std::vector<std::uint8_t>>,
            unlimited_length,
            not_null
            >;
        };
        
        template<class Identifier>
        struct deduce_storable_data_storage;
        
        template<class Identifier>
        struct deduce_storable_data_storage< storable_data<Identifier, std::string> >
        {
            using type = string_storage<
            Identifier,
            limited_length<64>,
            not_null
            >;
        };
        
        template<class Identifier>
        struct deduce_storable_data_storage< storable_data<Identifier, boost::uuids::uuid> >
        {
            using type = string_storage<
            Identifier,
            fixed_length<36>,
            not_null
            >;
        };
        
        template<class Identifier>
        struct deduce_storable_data_storage< storable_data<Identifier, boost::posix_time::ptime> >
        {
            using type = timestamp_storage<
            Identifier,
            not_null
            >;
        };
        
        template<class Identifier>
        struct default_storage<
        Identifier,
        std::enable_if_t< is_derived_from_template_v<Identifier, storable_data> >
        >
        {
            using storable_data_type = typename Identifier::storable_data_type;
            using type = typename deduce_storable_data_storage<storable_data_type>::type;
        };
        


    }
    template<class Field> using default_storage = typename detail::default_storage<Field>::type;
    
    
    
    
    
}}