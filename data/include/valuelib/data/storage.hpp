#pragma once
#include "field.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace value { namespace data {
    
    //
    // the concept of nullable
    //
    struct nullable_tag {};
    struct nullable { using tag_type = nullable_tag; };
    struct not_null { using tag_type = nullable_tag; };
    
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
    
    ///
    /// @tparam NativeType is the type that the program will use to manipulate the data
    /// @tparam LengthLimit is the type that describes any length limit on this data
    /// @tparam Nullable is the type that describes whether the field may be null.
    ///         if so, data access will be in terms of optional<NativeType>
    ///
    template<class NativeType, class LengthLimit, class Nullable>
    struct string_storage
    {
        struct attribute_tags : nullable_tag, size_limit_tag {};
        
        using length_limit_type = LengthLimit;
        using nullable_type = Nullable;
        using native_type = NativeType;
    };
    
    template<class NativeType, class Nullable>
    struct uuid_storage
    {
        struct attribute_tags : nullable_tag {};
        
        using nullable_type = Nullable;
        using native_type = NativeType;
    };
    
    template<class NativeType, class Nullable>
    struct timestamp_storage
    {
        struct attribute_tags : nullable_tag {};
        
        using nullable_type = Nullable;
        using native_type = NativeType;
    };
    
    ///
    /// @tparam NativeType is the type that the program will use to manipulate the data
    /// @tparam Nullable is the type that describes whether the field may be null.
    ///         if so, data access will be in terms of optional<NativeType>
    ///
    template<class NativeType, class Nullable>
    struct text_set_storage
    {
        struct attribute_tags : nullable_tag {};
        
        using nullable_type = Nullable;
        using native_type = NativeType;
    };
    
    namespace detail
    {
        template<class Field> struct default_storage;
        
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
    }
    template<class Field> using default_storage = typename detail::default_storage<Field>::type;
    
    
    
    
}}