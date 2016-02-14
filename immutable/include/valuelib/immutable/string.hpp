#pragma once
#include <cstddef>
#include <utility>
#include <boost/operators.hpp>
#include <cstring>
#include <string>
#include <ostream>

namespace value { namespace immutable {

	namespace detail
    {
        template<std::size_t Length>
        struct string_type
        {
            constexpr string_type(const char* buffer)
            : _data {}
            {
                for(std::size_t i = 0 ; i < Length ; ++i)
                    _data[i] = buffer[i];
                _data[Length] = 0;
            }

            template<std::size_t RLength>
            constexpr auto concat(const string_type<RLength>& r) const
            {
                char buf [Length + RLength + 1] = { 0 };
                for (size_t i = 0 ; i < Length ; ++i) {
                    buf[i] = _data[i];
                }
                for (size_t i = 0 ; i < RLength ; ++i) {
                    buf[i + Length] = r.at(i);
                }
                return string_type<Length + RLength>(buf);
            }
            
            template<std::size_t RLength>
            constexpr auto concat(const char (&r)[RLength]) const
            {
                char buf [Length + RLength] = { 0 };
                for (size_t i = 0 ; i < Length ; ++i) {
                    buf[i] = _data[i];
                }
                for (size_t i = 0 ; i < RLength ; ++i) {
                    buf[i + Length] = r[i];
                }
                return string_type<Length + RLength-1>(buf);
            }
            
            constexpr const char* c_str() const noexcept { return _data; }
            constexpr char at(size_t i) const {
                return _data[i];
            }
            
            constexpr static auto size() { return Length; }

        private:
            char _data[Length+1];
        };

        template<std::size_t Length>
        inline constexpr std::ostream& operator<<(std::ostream& os, string_type<Length> str)
        {
            return os.write(str.c_str(), str.size());
        }
        
        template<std::size_t L, std::size_t R>
        bool operator==(const string_type<L>& l, const string_type<R>& r)
        {
            return l.size() == r.size()
            and (std::memcmp(l.c_str(), r.c_str(), r.size()) == 0);
        }
        
        template<std::size_t L, std::size_t R>
        bool operator!=(const string_type<L>& l, const string_type<R>& r)
        {
            return !(l == r);
        }
        
        template<std::size_t Length>
        bool operator==(const std::string& l, const string_type<Length>& r)
        {
            return l.size() == r.size()
            and (std::memcmp(l.c_str(), r.c_str(), r.size()) == 0);
        }
        
        template<std::size_t Length>
        bool operator!=(const std::string& l, const string_type<Length>& r)
        {
            return !(l == r);
        }
        
        template<std::size_t Length>
        bool operator==(const string_type<Length>& l, const std::string& r)
        {
            return r == l;
        }
    }
    template<std::size_t N>
    using string_type = detail::string_type<N>;
    
    template<std::size_t Length>
    constexpr auto string(const char (&source) [Length])
    {
        return string_type<Length-1>(source);
    }
    
    template<std::size_t Length>
    constexpr auto string(char (&source) [Length])
    {
        return string_type<Length-1>(source);
    }

    template<class T, T N, typename std::enable_if_t<std::is_integral<T>::value and not std::is_same<bool, T>::value>* = nullptr>
    constexpr auto decimal_digits()
    {
        std::size_t digits = 0;
        T acc = N;
        do {
            ++digits;
            acc /= 10;
        } while(acc);
        return digits;
    }

    template<class T, T N, typename std::enable_if_t<std::is_integral<T>::value and not std::is_same<bool, T>::value>* = nullptr>
    constexpr auto to_string()
    {
        constexpr auto digits = decimal_digits<T, N>();
        char buffer[digits + 1] = { 0 };
        auto digit = digits;
        T acc = N;
        while(digit)
        {
            --digit;
            buffer[digit] = char('0' + (acc % 10));
            acc /= 10;
        }
        return string(buffer);
    }
    
    
    /// join
    
    namespace detail {
        
        
        template<size_t I, size_t N>
        struct join_impl
        {
            template<class Delimiter, class Tuple>
            constexpr auto apply(Delimiter del, Tuple t) const
            {
                return del.concat(std::get<I>(t)).concat(join_impl<I + 1, N>().apply(del, t));
            }
        };

        template<size_t N>
        struct join_impl<N, N>
        {
            template<class Delimiter, class Tuple>
            constexpr auto apply(Delimiter del, Tuple t) const
            {
                return value::immutable::string("");
            }
        };
        
        template<size_t N>
        struct join_impl<0, N>
        {
            template<class Delimiter, class Tuple>
            constexpr auto apply(Delimiter del, Tuple t) const
            {
                return std::get<0>(t).concat(join_impl<1, N>().apply(del, t));
            }
        };
        

    }
    
    template<std::size_t DelimiterLength, std::size_t...StringLengths>
    constexpr auto join(const char (&delimeter)[DelimiterLength], std::tuple<string_type<StringLengths>...> strings)
    {
        constexpr auto joiner = detail::join_impl<0, sizeof...(StringLengths)>();
        return joiner.apply(string(delimeter), strings);
    }
    
    
}}