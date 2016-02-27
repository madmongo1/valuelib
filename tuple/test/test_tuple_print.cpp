#include <gtest/gtest.h>
#include <type_traits>

#include <tuple>
#include <sstream>
#include <vector>
#include <algorithm>

#include <valuelib/tuple/print.hpp>

TEST(print_tuple_test, test_basics)
{
    using namespace std;
    auto t1 = int(5); // make_tuple("hello", 5);
    
//    ASSERT_FALSE(( value::tuple::TuplePrinterExists<int> ));
    
    auto ss = ostringstream();
    
    ss << value::tuple::print_tuple(t1);
    
    EXPECT_EQ("5", ss.str());
    
    ss.str("");
    auto t2 = string("hello");
    ss << value::tuple::print_tuple(t2);
    ASSERT_EQ("\"hello\"", ss.str());
    
    
    auto t3 = make_tuple(6, "foo");
    ss.str("");
    ss << value::tuple::print_tuple(t3);
    EXPECT_EQ("(6, \"foo\")", ss.str());
    
    auto t4 = make_tuple(make_tuple(t1, t2), t3);
    ss.str("");
    ss << value::tuple::print_tuple(t4);
    EXPECT_EQ("((5, \"hello\"), (6, \"foo\"))", ss.str());
}

TEST(print_tuple_test, vectors)
{
    using namespace std;
    auto v1 = vector<string> { "the", "cat", "mat" };
    
    ostringstream ss;
    ss << value::tuple::print_tuple(v1);
    EXPECT_EQ("[ \"the\", \"cat\", \"mat\" ]", ss.str());

    ss.str("");
    ss << value::tuple::print_tuple(rbegin(v1), rend(v1));
    EXPECT_EQ("[ \"mat\", \"cat\", \"the\" ]", ss.str());
    
    using iter_type = std::vector<unsigned int>::const_iterator;
    using value_type = value::tuple::ValueOfIterator<iter_type>;
    ss.str("");
    ss << value::debug::demangle<value_type>();
    ASSERT_EQ("unsigned int", ss.str());
    
    vector<uint8_t> v;
    generate_n(std::back_inserter(v), 80, [v = uint8_t(0)]() mutable -> uint8_t { return v++; });
    ss.str("");
    ss << value::tuple::print_tuple(v);
    EXPECT_EQ("[ 000102030405060708090a0b0c0d0e0f 101112131415161718191a1b1c1d1e1f 202122232425262728292a2b2c2d2e2f 303132333435363738393a3b3c3d3e3f\n  404142434445464748494a4b4c4d4e4f ]", ss.str());
}

#include <map>
TEST(print_tuple, maps)
{
    using namespace std;
    
    map<string, tuple<int, string>> m;
    
    m.emplace("richard", make_tuple(21, "hodges"));
    m.emplace("caesar", make_tuple(2, "the great"));
    m.emplace("spartacus", make_tuple(2, "the brave"));
    
    ostringstream ss;
    ss << value::tuple::print_tuple(value::tuple::make_range(m));
    EXPECT_EQ("[ (\"caesar\", (2, \"the great\")), (\"richard\", (21, \"hodges\")), (\"spartacus\", (2, \"the brave\")) ]", ss.str());
    
}
