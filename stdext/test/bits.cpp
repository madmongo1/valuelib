#include <gtest/gtest.h>

#include <valuelib/stdext/bits.hpp>

using namespace value;

TEST(bits, bits1)
{
    
    constexpr auto table = stdext::make_bit_translation_table( 1, "foo", 4, "bar");
    int data = 0x3f;
    
    auto s = stdext::bits_to_string(data, table);
    
    EXPECT_EQ("foo, bar, bit 0, bit 2, bit 3, bit 5", s);
    
    s = stdext::bits_to_string(data, stdext::make_bit_translation_table(0, "bing", 2, "bong"));

    EXPECT_EQ("bing, bong, bit 1, bit 3, bit 4, bit 5", s);
    
    s = stdext::bits_to_string(stdext::ignore_unknown, data, table);
    EXPECT_EQ("foo, bar", s);

    EXPECT_DEATH(s = stdext::bits_to_string(stdext::assert_unknown, data, table),
                 R"___(!"there are unknown bits set")___");
    
    s = stdext::bits_to_string(data, stdext::make_bit_translation_table( 1, "foo",
                                                                        4, "bar",
                                                                        stdext::mask(0xd), "rest"));
    EXPECT_EQ("foo, bar, rest, bit 5", s);

}