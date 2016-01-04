#include <gtest/gtest.h>

#include "pointer_traits.hpp"

namespace value { namespace pointers {

	void foop() {}
}}


TEST(pointerTraitsTest, basics)
{
    int* p;
    
    ASSERT_TRUE(value::pointers::CanBeDereferenced<decltype(p)>);
}