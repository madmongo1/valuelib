#pragma once

namespace value { namespace stdext {
    
  struct const_byte_buffer
    {
        const_byte_buffer(const std::uint8_t* begin, const std::uint8_t* end)
        : _begin(begin), _end(end)
        {}

        auto begin() const { return _begin; }
        auto end() const { return _end; }

    private:
        const std::uint8_t* _begin;
        const std::uint8_t* _end;
    };
    
    struct const_char_buffer
    {
        const_char_buffer(const char* begin, const char* end)
        : _begin(begin), _end(end)
        {}
        
        auto begin() const { return _begin; }
        auto end() const { return _end; }
        
    private:
        const char* _begin;
        const char* _end;
    };
    
    auto to_char_buffer(const_byte_buffer b)
    {
        return const_char_buffer(reinterpret_cast<const char*>(b.begin()),
                                 reinterpret_cast<const char*>(b.end()));
    }
}}