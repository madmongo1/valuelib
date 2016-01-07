#pragma once

#include <value/mysql/connection_invariants.hpp>
#include <value/mysql/connection.hpp>

#include "connection_instance.hpp"
#include "connection_instance_pool.hpp"

#include <deque>
#include <mutex>

namespace value { namespace mysql {

	struct connection::impl
    {
        impl(const connection_invariants& params)
        : _params(params)
        , _connection_pool(_params)
        {}
        
        const connection_invariants& params() const {
            return _params;
        }
        
        connection_instance acquire_connection_instance();
        
    private:
        connection_invariants _params;
        connection_instance::pool _connection_pool;
    };

}}