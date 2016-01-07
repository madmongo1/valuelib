#include <gtest/gtest.h>

#include <value/mysql/connection.hpp>
#include <value/mysql/transaction.hpp>
#include <value/mysql/statement.hpp>
#include <value/mysql/execution_result.hpp>

#include <vector>
#include <string>

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


struct exception_printer
{
    exception_printer(const std::exception& e)
    : pe(std::addressof(e))
    {}
    
    static void print_exception(std::ostream& os, const std::exception& e, int level =  0)
    {
        std::cerr << std::string(level, ' ') << "exception: " << e.what();
        try {
            std::rethrow_if_nested(e);
        } catch(const std::exception& e) {
            os << std::endl;
            print_exception(os, e, level+1);
        } catch(...) {}
    }

    std::ostream& operator()(std::ostream& os) const
    {
        print_exception(os, *pe);
        return os;
    }

private:
    const std::exception* pe;
};

inline std::ostream& operator<<(std::ostream& os, const exception_printer& p)
{
    return p(os);
}



exception_printer print(const std::exception& e)
{
    return exception_printer(e);
}

TEST(localConnectionTest, query)
{
    struct col_info
    {
        size_t max_size = 0;
    };
    
    try {
        auto connection = value::mysql::connection(value::mysql::host("localhost"),
                                                   value::mysql::username("value_mysql_test"),
                                                   value::mysql::password("value_mysql_test_password"),
                                                   value::mysql::schema("value_mysql_test"));
        
        auto trans = make_transaction(connection);
        auto stmt = make_statement(trans, "SELECT * FROM customers");
        std::cout << stmt << std::endl;
        auto results = stmt.execute().results();
        
        
        
    }
    catch(const std::exception& e)
    {
        std::cout << print(e) << std::endl;
    }
    
}

TEST(localConnectionTest, connectionLimitTest)
{
    std::vector<value::mysql::transaction> transactions;
    try {
        
        auto connection = value::mysql::connection(value::mysql::host("localhost"),
                                                   value::mysql::username("value_mysql_test"),
                                                   value::mysql::password("value_mysql_test_password"));

        while(transactions.size() < 3000)
            transactions.push_back(make_transaction(connection));
        
    }
    catch(const std::exception& e)
    {
    }
    EXPECT_LT(transactions.size(), 2000);
    EXPECT_GT(transactions.size(), 10);
    
}
