#include "impl.hpp"
#include "connection_instance.hpp"


namespace value { namespace mysql {

    transaction::impl::impl(struct connection conn)
    : _connection(std::move(conn))
    , _instance(_connection.acquire_connection_instance(connection::transaction_access_key()))
    {
    }
    
    void transaction::impl::rollback()
    {
        
    }
    
    void transaction::impl::rollback(without_exception_t) noexcept
    {
        if (_transaction_open) {
            mysql_rollback(_instance.mysql());
            _transaction_open = false;
        }
        
    }

    transaction::impl::~impl()
    {
        if (_transaction_open) {
            rollback(without_exception);
        }
    }
    

}}