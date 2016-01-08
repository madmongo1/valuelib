#pragma once

#include <value/mysql/types.hpp>
#include <value/mysql/connection.hpp>
#include <value/mysql/transaction.hpp>

#include "connection/impl.hpp"

namespace value { namespace mysql {
  
    struct transaction::impl
    {
        impl(struct connection conn);
        ~impl();
        
        void rollback();
        void rollback(without_exception_t) noexcept;
        
        shared_mysql_ptr mysql() { return _connection.mysql(); }
        

    private:
        struct connection _connection;
        bool _notified = false;
    };
}}

