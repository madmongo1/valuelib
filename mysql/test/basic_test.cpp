#include <gtest/gtest.h>

#include <value/mysql/connection.hpp>

TEST(connectionArgumentsTest, basic)
{
    EXPECT_TRUE(value::mysql::AreAllConnectionArguments<value::mysql::host>);
    EXPECT_TRUE(value::mysql::AreAllConnectionArguments<value::mysql::host&>);
    EXPECT_TRUE(value::mysql::AreAllConnectionArguments<value::mysql::host&&>);
    EXPECT_TRUE(value::mysql::AreAllConnectionArguments<const value::mysql::host>);
    EXPECT_TRUE(value::mysql::AreAllConnectionArguments<volatile value::mysql::host>);
    EXPECT_TRUE(value::mysql::AreAllConnectionArguments<const volatile value::mysql::host>);
    EXPECT_TRUE(value::mysql::AreAllConnectionArguments< value::mysql::host volatile&>);
    
}

TEST(connectionInvariantsTest, basic)
{
    auto ci = value::mysql::connection_invariants(value::mysql::host("localhost"),
                                                  value::mysql::port(345),
                                                  value::mysql::username("myself"),
                                                  value::mysql::password("foo"));
    EXPECT_EQ(to_string(ci),
              R"txt({ host="localhost", user="myself", passwd="******", db="{null}", port=345, unix_socket="{null}", flags=30000 })txt" );
    
}

