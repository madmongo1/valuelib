#pragma once

#include "string_traits.hpp"

namespace value {
	namespace stdext {

        template<class String, EnableIf< IsModelOfString<String> >* = nullptr>
        typename String::size_type
        find(String& s, const ValueType<String>& str, typename String::size_type pos = 0)
        {
            return s.find(str, pos);
        } 
        
        template<class String, EnableIf< IsModelOfString<String> >* = nullptr>
        typename String::size_type
        find(String& s, typename ValueType<String>::value_type ch, typename String::size_type pos = 0)
        {
            return s.find(ch, pos);
        } 
        
	}
}