#include <value/mysql/transaction.hpp>
#include <value/mysql/connection.hpp>

#include "connection_impl.hpp"
#include "connection_instance.hpp"

#include "transaction/impl.hpp"

#include <mysql.h>

namespace value { namespace mysql {
  
    

    //
    // transaction
    //
    
    transaction::transaction(struct connection conn)
    : _impl(std::make_shared<impl>(std::move(conn)))
    {}
    
    auto transaction::implementation() const -> impl&
    {
        return *_impl;
    }
    
    auto transaction::connection() const -> const struct connection&
    {
        return _impl->connection();
    }

    struct connection_instance& transaction::connection_instance() const
    {
        return _impl->connection_instance();
    }

    
    //
    // free functions
    //
    
    transaction make_transaction(connection conn)
    {
        return transaction(std::move(conn));
    }
}}