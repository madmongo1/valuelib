#include <gtest/gtest.h>
#include <type_traits>

#include <valuelib/data/enumeration.hpp>
#include <valuelib/data/field.hpp>


VALUE_DATA_DEFINE_FIELD(foo, std::string);
VALUE_DATA_DEFINE_FIELD(bar, std::string);

VALUE_DATA_DEFINE_ENUM(Name, (x)(y)(z));

TEST(testDataItem, testMap)
{
    constexpr auto assignable_from_char_star = std::is_assignable<foo, const char*>::value;
    EXPECT_FALSE(assignable_from_char_star);

    constexpr auto assignable_from_string = std::is_assignable<foo, std::string>::value;
    EXPECT_FALSE(assignable_from_string);
    
    constexpr auto assignable_from_bar = std::is_assignable<foo, bar>::value;
    EXPECT_FALSE(assignable_from_bar);
    
    constexpr auto assignable_from_foo = std::is_assignable<foo, foo>::value;
    EXPECT_TRUE(assignable_from_foo);
    
    auto f = foo("hello");
    auto b = bar{"world"};
    
    auto f2 = foo("igloo");
    
    EXPECT_GT(f2, f);
    
    
    // efficient to_string
    
    EXPECT_TRUE(std::addressof(f.value()) == std::addressof(to_string(f)));
    
    Name x(std::string("y"));
    
    EXPECT_EQ(x, Name::value_type::y);
    EXPECT_THROW(x = std::string("foo"), std::invalid_argument);
    auto s = to_string(x);
    EXPECT_EQ(std::string("y"), s);
    
    constexpr auto is_enum = std::is_enum<Name::value_type>::value;
    EXPECT_TRUE(is_enum);
    
}