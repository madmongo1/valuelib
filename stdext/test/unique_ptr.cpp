#include <gtest/gtest.h>

#include <valuelib/stdext/unique_ptr.hpp>


struct indicate_deleted
{
    indicate_deleted(bool& b)
    : _b(b)
    {}
    
    ~indicate_deleted() {
        _b = true;
    }
    
    bool& _b;
};

TEST(test_unique_ptr, default_deleter)
{
    bool deleted = false;
    auto object = new indicate_deleted(deleted);
    
    ASSERT_TRUE(object);
    ASSERT_FALSE(deleted);
    auto up = value::stdext::to_unique_ptr(object);
    ASSERT_FALSE(deleted);
    ASSERT_EQ(up.get(), object);
    
    up.reset();
    ASSERT_TRUE(deleted);
}

TEST(test_unique_ptr, custom_deleter)
{
    bool deleted = false;
    struct X {} x;
    auto up = value::stdext::to_unique_ptr(std::addressof(x), [&](X*){ deleted = true;});
    
    ASSERT_FALSE(deleted);
    ASSERT_TRUE(up.get() == std::addressof(x));
    up.reset();
    ASSERT_TRUE(deleted);
}