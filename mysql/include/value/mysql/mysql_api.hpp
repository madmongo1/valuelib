#pragma once

#include <mysql.h>
#include <memory>
#include <stdexcept>
#include <exception>


#include "error.hpp"
#include "connection_invariants.hpp"

namespace value { namespace mysql {
    
    
    struct auto_free
    {
        void operator()(MYSQL* p) const noexcept {
            if (p) {
                mysql_close(p);
            }
        }
        
        void operator()(MYSQL_RES* p) const noexcept {
            if (p)
                mysql_free_result(p);
        }
        
        void operator()(MYSQL_STMT* p) const noexcept {
            if (p)
                mysql_stmt_close(p);
        }
    };
    
    using mysql_ptr = std::unique_ptr<MYSQL, auto_free>;
    using shared_mysql_ptr = std::shared_ptr<MYSQL>;
        
    using result_ptr = std::unique_ptr<MYSQL_RES, auto_free>;
    using shared_result_ptr = std::shared_ptr<MYSQL_RES>;
    using statement_ptr = std::unique_ptr<MYSQL_STMT, auto_free>;
    using shared_statement_ptr = std::shared_ptr<MYSQL_STMT>;
    
    bool throw_error(MYSQL_STMT* ptr);
    bool throw_error(MYSQL* ptr);
    
    inline bool throw_error(const mysql_ptr& ptr) {
        return throw_error(ptr.get());
    }
    
    inline bool throw_error(const shared_mysql_ptr& ptr) {
        return throw_error(ptr.get());
    }
    
    inline bool throw_error(const statement_ptr& ptr) {
        return throw_error(ptr.get());
    }

    inline bool throw_error(const shared_statement_ptr& ptr) {
        return throw_error(ptr.get());
    }
    
    mysql_ptr connect(const connection_invariants& ci);
    void connect(MYSQL* mysql, const connection_invariants& ci);
    
    bool ping_query(const mysql_ptr& ptr) noexcept;
    
    result_ptr result_metadata(MYSQL_STMT* stmt);
    
    inline result_ptr result_metadata(const statement_ptr& ptr) {
        return result_metadata(ptr.get());
    }
    
    inline result_ptr result_metadtaa(const shared_statement_ptr& ptr) {
        return result_metadata(ptr.get());
    }

    inline void bind_result(MYSQL_STMT* stmt, MYSQL_BIND* bind)
    try
    {
        if (mysql_stmt_bind_result(stmt, bind)) {
            throw_error(stmt);
        }
    }
    catch(...)
    {
        std::throw_with_nested(std::runtime_error(__func__));
    }
    
    enum class fetch_result
    {
        success,
        finished,
        truncated
    };
    
    inline void begin_transaction(const shared_mysql_ptr& mysql)
    {
        (0 == mysql_autocommit(mysql.get(), 0))
        or throw_error(mysql);
    }
    
    inline void commit(const shared_mysql_ptr& mysql, bool reopen = false)
    {
        (0 == mysql_commit(mysql.get()))
        or throw_error(mysql);
        
        if (not reopen) {
            (0 == mysql_autocommit(mysql.get(), 1))
            or throw_error(mysql);
        }
    }
    
    inline void rollback(const shared_mysql_ptr& mysql)
    {
        (0 == mysql_rollback(mysql.get()))
        or throw_error(mysql);

        (0 == mysql_autocommit(mysql.get(), 1))
        or throw_error(mysql);
    }
    
    inline fetch_result fetch_next(MYSQL_STMT* stmt)
    try
    {
        auto result = mysql_stmt_fetch(stmt);
        switch(result) {
            case 0:
                return fetch_result::success;
                
            default:
                throw_error(stmt);
                
            case MYSQL_NO_DATA:
                return fetch_result::finished;
                
            case MYSQL_DATA_TRUNCATED:
                return fetch_result::truncated;
        }
    }
    catch(...)
    {
        std::throw_with_nested(std::runtime_error(__func__));
    }
    
    inline void fetch_column(MYSQL_STMT* stmt, MYSQL_BIND* bind, unsigned int column, unsigned long offset = 0)
    try
    {
        if(mysql_stmt_fetch_column(stmt, bind, column, offset))
        {
            throw_error(stmt);
        }
    }
    catch(...)
    {
        std::throw_with_nested(std::runtime_error(__func__));
    }
    
    inline void prepare(MYSQL_STMT* stmt, const std::string& sql)
    try
    {
        if(mysql_stmt_prepare(stmt, sql.c_str(), sql.size()))
            throw_error(stmt);
    }
    catch(...)
    {
        using namespace std;
        throw_with_nested(runtime_error("preparing query: "s + sql));
    }
    
    inline void bind_params(MYSQL_STMT* stmt, MYSQL_BIND* bind)
    try
    {
        if(mysql_stmt_bind_param(stmt, bind)) {
            throw_error(stmt);
        }
    }
    catch(...)
    {
        using namespace std;
        throw_with_nested(runtime_error(__func__));
    }
    
    inline void execute(MYSQL_STMT* stmt)
    try
    {
        if(mysql_stmt_execute(stmt)) {
            throw_error(stmt);
        }
    }
    catch(...)
    {
        using namespace std;
        throw_with_nested(runtime_error(__func__));
    }
    
    
}}