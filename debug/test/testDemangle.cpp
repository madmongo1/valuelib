#include <valuelib/debug/demangle.hpp>
#include <gtest/gtest.h>
#include <stdexcept>

namespace Y {
    struct X {
        struct Z {};
    };
}



TEST(testDemangle, basicTest)
{
    using namespace std::string_literals;
    std::ostringstream ss;
    ss << value::debug::demangle(typeid(Y::X::Z));
    EXPECT_EQ("Y::X::Z", ss.str());
    
    ss.str("");
    ss << value::debug::demangle(typeid(Y::X::Z).name());
    EXPECT_EQ("Y::X::Z", ss.str());
    
    ss.str("");
    EXPECT_NO_THROW(ss << value::debug::demangle("@£$%^&*"));
    EXPECT_EQ("invalid mangled name~@\xC2\xA3$%^&*", ss.str());
    
}

