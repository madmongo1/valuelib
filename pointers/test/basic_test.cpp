#include <gtest/gtest.h>

#include <valuelib/pointers/pointer_traits.hpp>
#include <memory>

TEST(pointerTraitsTest, basics)
{
    int* pi = new int(7);
    const int* pci = new int(9);
    int i = 5;
    
    ASSERT_TRUE(value::pointers::CanBeDereferenced<decltype(pi)>);
    ASSERT_TRUE(value::pointers::CanBeDereferenced<decltype(pci)>);
    ASSERT_FALSE(value::pointers::CanBeDereferenced<decltype(i)>);
    
    auto vi = value::pointers::value_semantics(pi);
    auto& ri = vi;
    ASSERT_EQ(*pi, ri);
    ASSERT_EQ(ri, *pi);
    ASSERT_NE(vi, i);
    ASSERT_NE(i, vi);
    
    auto vci = value::pointers::value_semantics(pci);
    ASSERT_EQ(*pci, vci);
    ASSERT_EQ(vci, *pci);
    
    
    
    
    delete pi;
    delete pci;
}

TEST(pointerTraitsTest, stdStringTest)
{
    using namespace std;
    auto up = std::make_unique<std::string>("test1");
    
    auto vup = value::pointers::value_semantics(std::move(up));
    
    ASSERT_EQ(vup, "test1");
    ASSERT_EQ("test1"s, vup);
    ASSERT_EQ(vup, vup);

    auto sp = std::make_shared<std::string>("test2");
    auto vsp = value::pointers::value_semantics(std::move(sp));

}