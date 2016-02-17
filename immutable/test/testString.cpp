#include <valuelib/immutable/string.hpp>
#include <valuelib/debug/unwrap.hpp>
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

TEST(testImmutableString, testQuotedString)
{
    constexpr auto a = value::immutable::string("bee");
    try {
        constexpr auto b = value::immutable::quoted(value::immutable::string("bee"));
        EXPECT_EQ("'bee'", b);
    }
    catch(const std::exception& e)
    {
        FAIL() << value::debug::unwrap(e);
    }
    EXPECT_NO_THROW({
        constexpr auto b = value::immutable::quoted(a);
        EXPECT_EQ("'bee'", b);
    });
    
    constexpr auto b = value::immutable::string("xxx'xxx");
    EXPECT_THROW(value::immutable::quoted(b), std::invalid_argument);
    
    
    
    
}