#include <gtest/gtest.h>

#include <valuelib/stdext/bind_weak.hpp>
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

struct Resource
{
    
};

std::string something() { return "foo"; }

TEST(bindWeakTest, unrelatedResourceTest)
{
    auto pr = std::make_shared<Resource>();
    
    size_t touched = 0;
    auto f = [&touched](size_t times) { touched += times; };
    
    auto weak_touch_n = value::stdext::bind_weak(f, pr, std::placeholders::_1);
    auto weak_touch_4 = value::stdext::bind_weak(f, pr, 4);
    auto weak_something = value::stdext::bind_weak(something, pr);
    
    auto touched_n = weak_touch_n(1);
    EXPECT_TRUE(bool(touched_n));
    EXPECT_EQ(1, touched);
    
    auto touched_4 = weak_touch_4();
    EXPECT_TRUE(bool(touched_4));
    EXPECT_EQ(5, touched);
    
    auto str = weak_something();
    EXPECT_TRUE(bool(str));
    EXPECT_EQ(std::string("foo"), str.get());
    
    pr.reset();
    
    touched_n = weak_touch_n(1);
    EXPECT_FALSE(bool(touched_n));
    EXPECT_EQ(5, touched);
    
    touched_4 = weak_touch_4();
    EXPECT_FALSE(bool(touched_4));
    EXPECT_EQ(5, touched);
    
    str = weak_something();
    EXPECT_FALSE(bool(str));

    
}
