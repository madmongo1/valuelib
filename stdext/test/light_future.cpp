/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <gtest/gtest.h>

#include <valuelib/stdext/light_future.hpp>

TEST(light_future, shared_state)
{
    /*
    value::stdext::shared_state<std::string> state([](auto){});
    
    EXPECT_EQ(value::stdext::shared_state_common::which_type::no_state, state.which());
    EXPECT_FALSE(state.has_state());
    EXPECT_FALSE(state.has_value());
    EXPECT_FALSE(state.has_exception());
    EXPECT_THROW(state.reference(), std::future_error);
    
    value::stdext::shared_state<value::stdext::detail::void_state> state_v;
    EXPECT_EQ(value::stdext::shared_state_common::which_type::no_state, state_v.which());
    EXPECT_FALSE(state_v.has_state());
    EXPECT_FALSE(state_v.has_value());
    EXPECT_FALSE(state_v.has_exception());
    EXPECT_THROW(state_v.reference(), std::future_error);
*/
}

typedef ::testing::Types<void, std::string> available_args;

template<class T>
auto make_arg();

template<> auto make_arg<std::string>() { return std::string("hello"); }
template<> auto make_arg<void>() { return; }

struct value_receiver_base
{
    void handle_exception(std::string s) { _exception = s; _received = true; }
    bool populate(std::ostream& os) const {
        if (_received) {
            os << "received : ";
            if (_exception.size()) {
                os << "exception : " << _exception;
                return false;
            }
            return true;
        }
        os << "not received";
        return false;
    }
    
    bool is_exception() const {
        return _exception.size() > 0;
    }
    
    std::string _exception;
    bool _received = false;
};

template<class T>
struct value_receiver : value_receiver_base
{
    void handle_future(value::stdext::light_future<T> f)
    {
        try {
            _value = f.get();
            _received = true;
        }
        catch(std::exception& e) {
            handle_exception(e.what());
        }
    }
    
    bool is_value()
    {
        return _received && not is_exception() && (_value == make_arg<T>());
    }
    
    void populate(std::ostream& os) const {
        if (value_receiver_base::populate(os))
        {
            os << "value : " << _value;
        }
    }
    T _value;
};

template<>
struct value_receiver<void> : value_receiver_base
{
    void handle_future(value::stdext::light_future<void> f)
    {
        try {
            _received = true;
        }
        catch(std::exception& e) {
            handle_exception(e.what());
        }
    }
    
    void populate(std::ostream& os) const {
        if (value_receiver_base::populate(os))
        {
            os << "value";
        }
    }
    
    bool is_value()
    {
        return _received && not is_exception();
    }


};

template<class T>
bool received(const value_receiver<T>& vr)
{
    return vr._received;
}

template<class T>
bool is_exception(const value_receiver<T>& vr)
{
    return !vr._exception.empty();
}

template<class T>
std::ostream& operator<<(std::ostream& os, const value_receiver<T>& vr)
{
    vr.populate(os);
    return os;
}

template<class T, class R>
struct fulfil_promise
{
    auto operator()(value::stdext::light_promise<T, R>& promise) const
    {
        return promise.set_value(make_arg<T>());
    }
};

template<class R>
struct fulfil_promise<void, R>
{
    auto operator()(value::stdext::light_promise<void, R>& promise) const
    {
        return promise.set_value();
    }
};

template<class ArgType>
struct light_future_test : ::testing::Test
{
    auto make_value() {
        return make_arg<ArgType>();
    }
};

TYPED_TEST_CASE(light_future_test, available_args);

TYPED_TEST(light_future_test, keep_promise)
{
    value_receiver<TypeParam> vr;
    auto p = value::stdext::promise_to_call<TypeParam>([&](auto future_value)
                                                         {
                                                             vr.handle_future(future_value);
                                                         });
    
    EXPECT_FALSE(received(vr)) << vr;
    
    value::stdext::light_future<TypeParam> f;
    EXPECT_NO_THROW(f = p.get_future());
    EXPECT_FALSE(f.valid());
    EXPECT_NO_THROW(( fulfil_promise<TypeParam, void>()(p) ));
    EXPECT_TRUE(f.valid());
    EXPECT_TRUE(vr.is_value()) << vr;
    
    
}

TEST(light_future, keep_promise)
{
    std::string value;
    std::string exception;
    
    auto p = value::stdext::promise_to_call<std::string>([&](auto future_value)
    {
        try {
            value = future_value.get();
        }
        catch(const std::exception& e)
        {
            exception = e.what();
        }
    });
    
    EXPECT_TRUE(value.empty());
    EXPECT_TRUE(exception.empty());
    value::stdext::light_future<std::string> f;
    EXPECT_NO_THROW(f = p.get_future());
    EXPECT_FALSE(f.valid());
    EXPECT_NO_THROW(p.set_value("hello"));
    EXPECT_TRUE(f.valid());
    EXPECT_EQ("hello", value);
    
    
    
}
