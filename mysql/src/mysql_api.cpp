#include <value/mysql/mysql_api.hpp>

namespace value { namespace mysql {
    
    bool throw_error(MYSQL_STMT* stmt)
    {
        using std::to_string;
        using namespace std::literals;
        
        if (stmt) {
            auto errnum = mysql_stmt_errno(stmt);
            if (errnum) {
                throw statement_error("MYSQL STMT Error: "s
                                      + to_string(errnum)
                                      + " SQLSTATE : " + mysql_stmt_sqlstate(stmt)
                                      + " : " + mysql_stmt_error(stmt));
            }
        }
        else {
            assert(!"statement is null");
            throw std::logic_error("statement pointer is null");
        }
        return false;
    }
    
    bool throw_error(MYSQL* mysql)
    {
        using std::to_string;
        using namespace std::literals;
        if (mysql) {
            if (auto err = mysql_errno(mysql)) {
                throw database_error("MYSQL Error : "s
                                     + to_string(err)
                                     + " : "
                                     + mysql_error(mysql));
            }
            return false;
        }
        else {
            throw std::logic_error("null mysql pointer");
        }
    }

    
    mysql_ptr connect(const connection_invariants& params)
    {
        auto ptr = mysql_ptr(mysql_init(nullptr));
        if (!ptr) {
            throw std::runtime_error("failed to allocate MYSQL");
        }
        
        mysql_real_connect(ptr.get(),
                           params.get_host(), params.get_user(),
                           params.get_passwd(),
                           params.get_db(),
                           params.get_port(),
                           params.get_unix_socket(),
                           params.get_client_flag())
        or throw_error(ptr);
        
        (0 == mysql_autocommit(ptr.get(), 0)) or throw_error(ptr);
        return ptr;
    }
    
    void connect(MYSQL* mysql, const connection_invariants& params)
    try
    {
        mysql_real_connect(mysql,
                           params.get_host(), params.get_user(),
                           params.get_passwd(),
                           params.get_db(),
                           params.get_port(),
                           params.get_unix_socket(),
                           params.get_client_flag())
        or throw_error(mysql);
    }
    catch(...)
    {
        using namespace std;
        throw_with_nested(runtime_error("connect : "s
                                        + to_string(params)));
    }
    
    bool ping_query(const mysql_ptr& ptr) noexcept {
        assert(ptr.get());
        return !mysql_ping(ptr.get());
    }
    
    result_ptr result_metadata(MYSQL_STMT* stmt)
    try
    {
        auto result = mysql_stmt_result_metadata(stmt);
        if (not result)
        {
            throw_error(stmt);
        }
        
        // warning: if we get here, and result is null, the statement legitimately does not have a result
        
        return result_ptr(result);
    }
    catch(...)
    {
        std::throw_with_nested(std::runtime_error(__func__));
    }
    

    
}}