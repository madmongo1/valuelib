#include "connection_pool.hpp"

#include <stack>
#include <mutex>

namespace value {namespace mysql {
    
    
    struct connection_pool::impl
    : std::enable_shared_from_this<impl>
    {
        using mutex_type = std::mutex;
        using lock_type = std::unique_lock<mutex_type>;
        using guard_type = std::lock_guard<mutex_type>;
        
        auto pop() -> mysql_ptr;
        auto push(mysql_ptr) -> void;

    private:
        
        mutex_type _mutex;
        std::stack<mysql_ptr> _free_mysql_connections;
    };
    
    connection_pool::connection_pool()
    : _impl_ptr(std::make_shared<impl>())
    {}
    
    auto connection_pool::get_weak_impl() -> std::weak_ptr<impl>
    {
        return _impl_ptr;
    }
    
    auto connection_pool::take_ownership(mysql_ptr mysql) -> shared_mysql_ptr
    {
        auto deleter = [weak_impl = get_weak_impl(),
                        original = mysql.get_deleter()](MYSQL* p)
        {
            auto impl = weak_impl.lock();
            if (impl && p) {
                impl->push(mysql_ptr(p, original));
            }
            else {
                original(p);
            }
        };
        
        return shared_mysql_ptr { mysql.release(), std::move(deleter) };
    }
    
    auto connection_pool::pop() -> shared_mysql_ptr
    {
        return take_ownership(_impl_ptr->pop());
    }
    
    //
    // impl
    //
    
    auto connection_pool::impl::push(mysql_ptr mysql) -> void
    {
        guard_type guard(_mutex);
        _free_mysql_connections.push(std::move(mysql));
    }
    
    auto connection_pool::impl::pop() -> mysql_ptr
    {
        lock_type lock(_mutex);
        while (!_free_mysql_connections.empty())
        {
            auto mysql = std::move(_free_mysql_connections.top());
            _free_mysql_connections.pop();
            lock.unlock();
            if(ping_query(mysql))
                return mysql;
            lock.lock();
        }
        return {};
    }
    
}}
