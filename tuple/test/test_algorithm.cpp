#include <gtest/gtest.h>

#include <valuelib/tuple/algorithm.hpp>
#include <string>

TEST(test_algorithm, accumulate)
{
    auto t = std::make_tuple(1, 2, 3, 4, 5, 6);
    auto x = value::tuple::accumulate(t, 0);
    ASSERT_EQ(21, x);
}


template<class T>
struct is_to_string_valid
{
    template<class X> static auto test(X*p) -> decltype(std::to_string(std::declval<X>()), void(), std::true_type());
    template<class X> static auto test(...) -> decltype(std::false_type());
    static constexpr auto value = decltype(test<T>(0))::value;
};
template<class T> static const auto IsToStringValid = is_to_string_valid<std::decay_t<T>>::value;

struct joiner
{
    joiner(const char* separator = ", ", const char* inital = "")
    : sep(inital)
    , between(separator)
    {};
    
    template<class Arg, std::enable_if_t<IsToStringValid<Arg>>* = nullptr>
    void append(Arg&& arg) {
        s += std::to_string(arg);
    }
    
    void append(const std::string& arg) {
        s += arg;
    }
    
    void append(const char* arg) {
        s += arg;
    }
    
    template<class Arg>
    joiner& operator()(Arg&& arg)
    {
        s += sep;
        append(std::forward<Arg>(arg));
        sep = between;
        return *this;
    }
    
    const std::string& str() const & { return s; }
    std::string&& str() && { return std::move(s); }

private:
    const char* sep = "";
    const char* between = "";
    std::string s;
};

TEST(test_algorithm, foreach_mutable)
{
    using namespace std;
    auto t = std::make_tuple(6, "cats", "meowing"s);
    auto s = value::tuple::collect(t, joiner(" "));
    ASSERT_EQ("6 cats meowing", s.str());
}