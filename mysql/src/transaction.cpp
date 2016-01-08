#include <value/mysql/transaction.hpp>
#include <value/mysql/connection.hpp>

#include "transaction/impl.hpp"

#include <mysql.h>

namespace value { namespace mysql {
  
    

    //
    // transaction
    //
    
    transaction::transaction(struct connection con)
    : _impl_ptr(std::make_shared<impl>(std::move(con)))
    {
    }

    
    //
    // free functions
    //
    
    transaction make_transaction(const connection& conn)
    {
        return transaction(std::move(conn));
    }
}}