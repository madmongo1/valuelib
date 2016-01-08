#pragma once
#include <value/mysql/mysql_api.hpp>

#include <value/mysql/connection_invariants.hpp>
#include <value/mysql/connection.hpp>




#include <deque>
#include <mutex>

namespace value { namespace mysql {

    struct connection::impl : MYSQL
    {
        impl(connection_invariants params);
        
        ~impl() noexcept;
        
        
    private:
        using mutex_type = std::mutex;
        using lock_type = std::unique_lock<mutex_type>;
        
        connection_invariants _params;
        mutex_type _mutex;
        bool _open = false;
        bool _need_close = false;
    };

}}