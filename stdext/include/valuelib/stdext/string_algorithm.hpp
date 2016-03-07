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
        
        template<class IterValueType>
        struct join_target
        {
            using type = IterValueType;
        };
        
        template<>
        struct join_target<const char*>
        {
            using type = std::string;
        };
        
        template<size_t N>
        struct join_target<const char [N]>
        {
            using type = std::string;
        };
        
        template<class IterValueType>
        using JoinTargetType = typename join_target<IterValueType>::type;
        
        
        
        template<class Iter, class Separator>
        auto join(const Separator& sep, Iter first, Iter last)
        {
            auto result = JoinTargetType<typename Iter::value_type>();
            if (first != last)
            {
                result = *first++;
                while (first != last) {
                    result += sep;
                    result += *first++;
                }
            }
            return result;
        }
        
	}
}