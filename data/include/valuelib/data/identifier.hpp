#pragma once
#include <valuelib/immutable/string.hpp>

namespace value { namespace data {
    
    ///
    /// The concept of an identifier
    ///
    template<class NameLength>
    struct identifier_type
    {
        static constexpr value::immutable::string<NameLength> identifier();
    };

}}

#define VALUE_DATA_IDENTIFIED_BY(name) \
\
static constexpr auto identifier() { \
    return value::immutable::string(#name); \
}

