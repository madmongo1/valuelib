#pragma once

#include <value/mysql/types.hpp>
#include <value/mysql/connection.hpp>
#include <value/mysql/transaction.hpp>
#include "connection_instance.hpp"

namespace value { namespace mysql {
  
    struct transaction::impl
    {
        impl(struct connection conn);
        ~impl();
        
        void rollback();
        void rollback(without_exception_t) noexcept;
        
        struct connection& connection() noexcept { return _connection; }
        struct connection_instance& connection_instance() noexcept { return _instance; }
        

    private:
        // the connection pool
        struct connection _connection;
        
        // the real connection handle
        struct connection_instance _instance;
        
        // whether the transaction needs to roll anything back
        
        bool _transaction_open = false;
    };
}}

