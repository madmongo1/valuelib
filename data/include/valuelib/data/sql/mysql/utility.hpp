#pragma once
#include <valuelib/immutable/string.hpp>

namespace value { namespace data { namespace sql { namespace mysql {

    template<std::size_t Length>
    static constexpr auto backtick(value::immutable::string_type<Length> s)
    {
        return value::immutable::string("`") + s + "`";
    }
    
}}}}
