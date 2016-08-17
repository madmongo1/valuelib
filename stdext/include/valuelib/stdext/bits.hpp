#pragma once
#include <tuple>
#include <string>
#include <array>
#include <utility>

namespace value { namespace stdext {
    
    struct bit
    {
        constexpr bit(int n) : n(n) {}
        int n;
    };
    struct mask
    {
        constexpr mask(int m) : m(m) {}
        int m;
    };
    
    struct bit_translation
    {
        constexpr bit_translation(int bit, const char* name) : mask(1 << bit), name(name) {}
        constexpr bit_translation(mask m, const char* name) : mask(m.m), name(name) {}
        constexpr bit_translation(bit b, const char* name) : bit_translation(b.n, name) {}
        int mask;
        const char* name;
    };
    
    namespace detail {
        
        template<class Tuple, std::size_t...Is>
        constexpr auto tuple_to_array_impl(Tuple&& t, std::index_sequence<Is...>)
        {
            constexpr auto size = sizeof...(Is);
            return std::array<bit_translation, size>
            {
                {
                    bit_translation
                    {
                        std::get<0>(std::get<Is>(std::forward<Tuple>(t))),
                        std::get<1>(std::get<Is>(std::forward<Tuple>(t)))
                    }...
                }
            };
        }
        
        template<class Tuple>
        constexpr auto tuple_to_array(Tuple&& t)
        {
            constexpr auto size = std::tuple_size<Tuple>::value;
            return tuple_to_array_impl(std::forward<Tuple>(t), std::make_index_sequence<size>());
        }
        
        constexpr auto make_translation_tuple()
        {
            return std::make_tuple();
        }
        
        template<class MaskOrBit>
        constexpr auto make_translation_tuple(MaskOrBit number, const char* name)
        {
            return std::make_tuple(std::make_tuple(number, name));
        }
        
        template<class MaskOrBit, class...Rest>
        constexpr auto make_translation_tuple(MaskOrBit number, const char* name, Rest&&...rest)
        {
            return std::tuple_cat(make_translation_tuple(number, name),
                                  make_translation_tuple(std::forward<Rest>(rest)...));
        }
        
        struct ignore_unknown
        {
            void operator()(std::string&, const char*, int&) const
            {
                // NOP
            }
        };
        struct report_unknown
        {
            void operator()(std::string& result, const char* sep, int& bits) const
            {
                int bitnum = 0;
                while (bits) {
                    if (bits & (1 << bitnum)) {
                        result += sep;
                        result += "bit " + std::to_string(bitnum);
                        sep = ", ";
                        bits &= ~(1 << bitnum);
                    }
                    ++bitnum;
                }
            }
        };
        
        struct assert_unknown
        {
            void operator()(std::string&, const char*, int&) const
            {
                assert(!"there are unknown bits set");
            }
        };
        
        template<class Iter>
        std::string bits_to_string_impl(std::string& result, const char*&sep, int& bits, Iter first, Iter last)
        {
            for ( ; first != last ; ++first)
            {
                auto mask = first->mask;
                if ( (bits & mask) == mask )
                {
                    result += sep;
                    result += first->name;
                    sep = ", ";
                    bits &= ~(mask);
                }
            }
            
            return result;
        }

        template<class Policy, class Iter>
        void bits_to_string_impl(Policy policy, std::string& result, const char* sep, int& bits, Iter first, Iter last)
        {
            bits_to_string_impl(result, sep, bits, first, last);
            policy(result, sep, bits);
        }
    }
    
    static const auto ignore_unknown = detail::ignore_unknown {};
    static const auto report_unknown = detail::report_unknown {};
    static const auto assert_unknown = detail::assert_unknown {};

    
    template <class...Args>
    constexpr auto make_bit_translation_table(Args&&...args)
    {
        constexpr auto nargs = sizeof...(Args);
        static_assert(nargs % 2 == 0, "must be an even number of arguments");
        return detail::tuple_to_array(detail::make_translation_tuple(std::forward<Args>(args)...));
    }
    
    template<class Policy, class Iter>
    std::string bits_to_string(Policy policy, int bits, Iter first, Iter last)
    {
        std::string result;
        auto sep = "";

        detail::bits_to_string_impl(policy, result, sep, bits, first, last);
        
        return result;
    }
    
    template<std::size_t N>
    std::string bits_to_string(int bits, std::array<bit_translation, N> const& trans)
    {
        return bits_to_string(report_unknown, bits, std::begin(trans), std::end(trans));
    }
    
    template<class Policy, std::size_t N>
    std::string bits_to_string(Policy policy, int bits, std::array<bit_translation, N> const& trans)
    {
        return bits_to_string(policy, bits, std::begin(trans), std::end(trans));
    }
    
    
}}