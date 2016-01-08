#include <value/mysql/mysql_api.hpp>


#include "connection_instance_pool.hpp"
#include "connection_instance_impl.hpp"

#include <deque>
#include <mutex>

namespace value { namespace mysql {
    
    struct connection_instance::pool::impl
    : std::enable_shared_from_this<impl>
    {
        impl(const connection_invariants& params)
        : _params(params)
        {}
        
        using free_connection_ptr = std::unique_ptr<connection_instance::impl>;
        using free_connection_list = std::deque<free_connection_ptr>;
        
        using mutex_type = std::mutex;
        using lock_type = std::unique_lock<mutex_type>;
        using guard_type = std::lock_guard<mutex_type>;
        
        auto acquire() -> connection_instance::impl_ptr;
        
        auto acquire_mysql() -> shared_mysql_ptr;
        
    private:
        
        mysql_ptr new_connection() const;
        shared_mysql_ptr make_return_to_pool(mysql_ptr mysql);
        std::weak_ptr<impl> get_weak_ptr() { return shared_from_this(); }

        mysql_ptr pop_connection();
        void push_connection(mysql_ptr ptr);

        
        struct free_connection;
        friend struct free_connection;
        
        
        auto pop_free_connection() -> free_connection_ptr;
        
        auto push_free_connection(free_connection_ptr connection) noexcept -> void;
        
        
    private:
        
        const connection_invariants& _params;
        mutex_type _mutex;
        free_connection_list _free_connection_list;
        
        std::deque<mysql_ptr> _free_mysql_connections;
    };
    
    struct connection_instance::pool::impl::free_connection
    {
        free_connection(std::weak_ptr<impl> pool);
        void operator()(connection_instance::impl* p) const noexcept;
        
    private:
        std::weak_ptr<impl> _weak_pool;
    };
    
    
    auto connection_instance::pool::impl::acquire()
    -> connection_instance::impl_ptr
    {
        auto f = free_connection(shared_from_this());
        
        while(auto p = pop_free_connection())
        {
            if (p->pings_ok()) {
                return { p.release(), std::move(f) };
            }
        }
        
        return {
            new connection_instance::impl(_params),
            std::move(f)
        };
    }
    
    auto connection_instance::pool::impl::acquire_mysql() ->shared_mysql_ptr
    {
        while(auto mysql = pop_connection())
        {
            if (ping_query(mysql)) {
                return make_return_to_pool(std::move(mysql));
            }
        }
        // try to create a mysql connection
        return make_return_to_pool(new_connection());
    }
    
    auto connection_instance::pool::impl::make_return_to_pool(mysql_ptr mysql)
    -> shared_mysql_ptr
    {
        auto deleter = [weak_self = this->get_weak_ptr(),
                        original = mysql.get_deleter()](MYSQL* p)
        {
            auto self = weak_self.lock();
            if (self && p) {
                self->push_connection(mysql_ptr(p, original));
            }
            else {
                original(p);
            }
        };
        return {
            mysql.release(),
            std::move(deleter)
        };
    }
    
    auto connection_instance::pool::impl::pop_connection()
    -> mysql_ptr
    {
        lock_type lock(_mutex);
        if (!_free_mysql_connections.empty()) {
            auto p = std::move(_free_mysql_connections.back());
            _free_mysql_connections.pop_back();
            return p;
        }
        else {
            return { };
        }
    }
    
    auto connection_instance::pool::impl::push_connection(mysql_ptr ptr)
    -> void
    {
        lock_type lock(_mutex);
        _free_mysql_connections.push_back(std::move(ptr));
    }
    
    auto connection_instance::pool::impl::new_connection() const
    -> mysql_ptr
    {
        return connect(_params);
    }
    
    auto connection_instance::pool::impl::pop_free_connection() -> free_connection_ptr
    {
        lock_type lock(_mutex);
        if (_free_connection_list.empty()) {
            lock.unlock();
            return std::make_unique<connection_instance::impl>(_params);
        }
        else {
            auto ptr = std::move(_free_connection_list.back());
            _free_connection_list.pop_back();
            return ptr;
        }
    }
    
    auto connection_instance::pool::impl::push_free_connection(free_connection_ptr connection) noexcept -> void
    {
        try {
            lock_type lock(_mutex);
            _free_connection_list.push_back(std::move(connection));
        }
        catch(...) {
            
        }
    }
    
    
    
    //
    // pool::impl::free_connection
    //
    
    connection_instance::pool::impl::free_connection::free_connection(std::weak_ptr<connection_instance::pool::impl> pool)
    :_weak_pool(std::move(pool))
    {
        
    }
    
    void connection_instance::pool::impl::free_connection::operator()(connection_instance::impl* p) const noexcept
    {
        if (p)
        {
            if (auto pool = _weak_pool.lock()) {
                pool->push_free_connection(free_connection_ptr(p));
            }
            else {
                delete p;
            }
        }
    }
    
    
    //
    // pool implementation
    //
    
    connection_instance::pool::pool(const connection_invariants& params)
    : _impl_ptr(std::make_shared<impl>(params))
    {
        
    }
    
    auto connection_instance::pool::acquire() -> connection_instance
    {
        return { create_from_impl(), _impl_ptr->acquire() };
    }
    
    
    
    
    
}}

