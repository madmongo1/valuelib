#include <gtest/gtest.h>

#include <stdext/bind_weak.hpp>
#include <memory>

TEST(bindWeakTest, testBasics)
{
    
    struct Y
    {
        void bar() {};
    };
    
    struct X : std::enable_shared_from_this<X>
    {

        int increment(int by) {
            count += by;
            return count;
        }
        
        void foo() {
            
        }
        
        Y y;
        
        int count = 0;
    };
    
    auto px = std::make_shared<X>();
    
    auto wf = value::stdext::bind_weak(&X::increment, px, std::placeholders::_1);
    auto weak_call_bar = value::stdext::bind_weak(&Y::bar, std::shared_ptr<Y>(px, &px->y));
    
    auto ret1 = wf(4);
    EXPECT_TRUE(bool(ret1));
    EXPECT_EQ(4, ret1.get());
    
    auto wfoo1 = value::stdext::bind_weak(&X::foo, px);
    auto retfoo1 = wfoo1();
    EXPECT_TRUE(bool(retfoo1));

    auto retbar1 = weak_call_bar();
    EXPECT_TRUE(bool(retbar1));
    
    px.reset();
    auto ret2 = wf(4);
    EXPECT_FALSE(bool(ret2));
    
    auto retfoo2 = wfoo1();
    EXPECT_FALSE(bool(retfoo2));
    
    auto retbar2 = weak_call_bar();
    EXPECT_FALSE(bool(retbar2));
    
    
}

