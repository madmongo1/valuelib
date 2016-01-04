#include <gtest/gtest.h>

#include <stdext/string_traits.hpp>
#include <stdext/shared_string.hpp>
#include <stdext/string_algorithm.hpp>

TEST(debugTest, testSharedString)
{
	std::string s = "Hello";

	auto ss = value::stdext::make_shared_string(s);

	auto pos = value::stdext::find(ss, 'l');
	ASSERT_EQ(2, pos);
    
    ASSERT_EQ(s, ss);
    ASSERT_EQ(ss, s);

}