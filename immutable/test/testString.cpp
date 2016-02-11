#include <valuelib/immutable/string.hpp>
#include <gtest/gtest.h>

TEST(testImmutableString, basicTest)
{
    namespace immutable = value::immutable;
    
    auto x = immutable::string("hello");
    EXPECT_STREQ("hello", x.c_str());
}

TEST(testImmutableString, testConcat)
{
    namespace immutable = value::immutable;
    
    constexpr auto x = immutable::string("hello");
    constexpr auto y = x.concat(" world");
    
    EXPECT_STREQ("hello world", y.c_str());

    constexpr auto z = immutable::string("hello").concat(" world").concat(immutable::string("!"));
    
    EXPECT_STREQ("hello world!", z.c_str());
}