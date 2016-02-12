#pragma once
#include <cstddef>
#include <utility>
#include <boost/operators.hpp>

namespace value { namespace immutable {

	namespace detail
    {
        template<std::size_t Length>
        struct string
        {
            constexpr string(const char* buffer)
            : _data {}
            {
                for(std::size_t i = 0 ; i < Length ; ++i)
                    _data[i] = buffer[i];
                _data[Length] = 0;
            }

            template<std::size_t RLength>
            constexpr auto concat(const string<RLength>& r) const
            {
                char buf [Length + RLength + 1] = { 0 };
                for (size_t i = 0 ; i < Length ; ++i) {
                    buf[i] = _data[i];
                }
                for (size_t i = 0 ; i < RLength ; ++i) {
                    buf[i + Length] = r.at(i);
                }
                return string<Length + RLength>(buf);
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
                return string<Length + RLength-1>(buf);
            }
            
            const char* c_str() const noexcept { return _data; }
            constexpr char at(size_t i) const {
                return _data[i];
            }
            
            constexpr static auto size() { return Length; }

        private:
            char _data[Length+1];
        };
        
        template<std::size_t L, std::size_t R>
        bool operator==(const string<L>& l, const string<R>& r)
        {
            return l.size() == r.size()
            and (std::memcmp(l.c_str(), r.c_str(), r.size()) == 0);
        }
        
        template<std::size_t L, std::size_t R>
        bool operator!=(const string<L>& l, const string<R>& r)
        {
            return !(l == r);
        }
        
        template<std::size_t Length>
        bool operator==(const std::string& l, const string<Length>& r)
        {
            return l.size() == r.size()
            and (std::memcmp(l.c_str(), r.c_str(), r.size()) == 0);
        }
        
        template<std::size_t Length>
        bool operator!=(const std::string& l, const string<Length>& r)
        {
            return !(l == r);
        }
        
        template<std::size_t Length>
        bool operator==(const string<Length>& l, const std::string& r)
        {
            return r == l;
        }
    }
    template<std::size_t N>
    using string_type = detail::string<N>;
    
    template<std::size_t Length>
    constexpr auto string(const char (&source) [Length])
    {
        auto s = detail::string<Length-1>(source);
        return s;
    }
    
    template<std::size_t Length>
    constexpr auto string(char (&source) [Length])
    {
        return detail::string<Length-1>(source);
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

}}