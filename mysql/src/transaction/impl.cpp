#include "impl.hpp"
#include <value/mysql/mysql_api.hpp>

namespace value { namespace mysql {

    transaction::impl::impl(struct connection con)
    : _connection(std::move(con))
    {
//        _connection.increase_transaction_count();
        _notified = true;
        
    }
    
    transaction::impl::~impl()
    {
//        if (_notified)
//            _connection.decrease_transaction_count();
    }
    
    void transaction::impl::rollback()
    {
        _connection.rollback();
    }
    
    void transaction::impl::rollback(without_exception_t without_exception) noexcept
    {
        _connection.rollback(without_exception);
    }

    

}}