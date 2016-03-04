#include <gtest/gtest.h>

#include <valuelib/stdext/string_traits.hpp>
#include <valuelib/stdext/shared_string.hpp>
#include <valuelib/stdext/string_algorithm.hpp>

TEST(debugTest, testSharedString)
{
	std::string s = "Hello";

	auto ss = value::stdext::make_shared_string(s);

	auto pos = value::stdext::find(ss, 'l');
	ASSERT_EQ(2, pos);
    
    ASSERT_EQ(s, ss);
    ASSERT_EQ(ss, s);

}