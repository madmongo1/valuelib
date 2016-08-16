#include <gtest/gtest.h>
#include <valuelib/stdext/invoke.hpp>
#include <valuelib/stdext/apply.hpp>

namespace stdext = value::stdext;

TEST(invoke, test1)
{
    
    auto dbl = [](auto const& x) -> int { return 2 * x; };
    
    auto result = stdext::invoke(dbl, 6);
    ASSERT_EQ(12, result);
    
    result = stdext::apply(dbl, std::make_tuple(4));
    ASSERT_EQ(8, result);
    
}