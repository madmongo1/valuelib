#include <gtest/gtest.h>

#include <valuelib/stdext/key_iterator.hpp>

#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <iterator>

TEST(key_iterator_test, map_test)
{
    std::map<std::string, std::string> m {
        { "foo", "oof" },
        { "bar", "rab" }
    };
    
    std::ostringstream ss;
    std::copy(value::stdext::make_key_iterator(std::begin(m)),
              value::stdext::make_key_iterator(std::end(m)),
              std::ostream_iterator<std::string>(ss, ", "));
    EXPECT_EQ("bar, foo, ", ss.str());
    
    ss.str("");
    std::copy(value::stdext::make_key_iterator(std::cbegin(m)),
              value::stdext::make_key_iterator(std::cend(m)),
              std::ostream_iterator<std::string>(ss, ", "));
    EXPECT_EQ("bar, foo, ", ss.str());
    
    auto v = std::vector<std::string> { "foo", "oof", "bar", "rab" };
    ss.str("");
    std::copy(value::stdext::make_key_iterator(std::cbegin(v)),
              value::stdext::make_key_iterator(std::cend(v)),
              std::ostream_iterator<std::string>(ss, ", "));
    EXPECT_EQ("foo, oof, bar, rab, ", ss.str());

    auto um = std::unordered_map<std::string, std::string>(std::begin(m), std::end(m));
    auto keys = std::vector<std::string>();
    std::copy(value::stdext::make_key_iterator(std::cbegin(um)),
              value::stdext::make_key_iterator(std::cend(um)),
              std::back_inserter(keys));
    EXPECT_EQ(2, keys.size());
    EXPECT_EQ(1, std::count(keys.begin(), keys.end(), "foo"));
    EXPECT_EQ(1, std::count(keys.begin(), keys.end(), "bar"));
    
    keys = value::stdext::all_keys(um);
    EXPECT_EQ(2, keys.size());
    EXPECT_EQ(1, std::count(keys.begin(), keys.end(), "foo"));
    EXPECT_EQ(1, std::count(keys.begin(), keys.end(), "bar"));
    
}