#pragma once

#include "string_traits.hpp"
#include "invoke.hpp"
#include <sstream>
#include <algorithm>

namespace value { namespace stdext {
    
    
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
    
    struct c_escape
    {
        template<class Iter>
        void operator()(char c, Iter& iter) {
            if (std::isprint(c)) {
                *iter++ = c;
            } else {
                switch (c) {
                    case '\'':
                    case '\"':
                    case '\\': {
                        emit_escaped(c, iter);
                    } break;
                    case '\r': emit_escaped('r', iter); break;
                    case '\f': emit_escaped('f', iter); break;
                    case '\v': emit_escaped('v', iter); break;
                    case '\n': emit_escaped('n', iter); break;
                    case '\t': emit_escaped('t', iter); break;
                    case '\b': emit_escaped('b', iter); break;
                    case '\a': emit_escaped('a', iter); break;
                    default:
                        emit_octal(c, iter); break;
                        break;
                }
            }
        }
        
        template<class Iter>
        void emit_escaped(char c, Iter& iter) {
            *iter++ = '\\';
            *iter++ = c;
        }

        template<class Iter>
        void emit_octal(char c, Iter& iter) {
            std::ostringstream ss;
            ss << std::oct << (int(c) & 0xff);
            auto s = ss.str();
            iter = std::copy(s.begin(), s.end(), iter);
        }
    };
    
    template<class OutIter, class Iter, class EscapeStyle = c_escape>
    OutIter do_escape(Iter first, Iter last, OutIter firstout,
                      EscapeStyle style_functor = EscapeStyle())
    {
        for( ; first != last ; ++first) {
            invoke(style_functor, *first, firstout);
        }
        return firstout;
    }

    template<class Iter, class EscapeStyle>
    struct string_escaper
    {
        string_escaper(Iter first, Iter last, EscapeStyle functor)
        : _first(first), _last(last), _functor(functor)
        {}
        
        std::ostream& operator()(std::ostream& os) {
            do_escape(_first, _last, std::ostream_iterator<char>(os),
                      std::ref(_functor));
            return os;
        }
        
        Iter _first;
        Iter _last;
        EscapeStyle _functor;
    };
    
    template<class Iter, class EscapeStyle>
    std::ostream& operator<<(std::ostream& os, string_escaper<Iter, EscapeStyle> escaper)
    {
        return escaper(os);
    }
    
    template<class Iter, class EscapeStyle = c_escape>
    string_escaper<Iter, EscapeStyle> escape(Iter first, Iter last,
                      EscapeStyle style_functor = EscapeStyle())
    {
        return { first, last , style_functor };
    }

    
    
    
    
}}