#pragma once
#include <cstddef>

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
            
            char _data[Length+1];
        };
    }
    
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

}}