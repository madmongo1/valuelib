#include "connection_pool.hpp"

namespace value { namespace mysql {
    
    
    auto connection_pool::acquire_connection(const connection_invariants& invariants)
    -> std::shared_ptr<connection_instance>
    {
        auto f = free_connection(invariants,
                                 shared_from_this());
        
        while(auto p = pop_free_connection(invariants))
        {
            if (p->pings_ok()) {
                return { p.release(), std::move(f) };
            }
        }
        
        return {
            new connection_instance(invariants),
            std::move(f)
        };
    }
    
    connection_pool::free_connection::free_connection(const connection_invariants& invariants,
                                                      std::weak_ptr<connection_pool> pool)
    : _invariants(std::move(invariants))
    , _weak_pool(std::move(pool))
    {
        
    }
    
    void connection_pool::free_connection::operator()(connection_instance* p) const noexcept
    {
        if (auto pool = _weak_pool.lock()) {
            pool->push_free_connection(_invariants,
                                       free_connection_ptr(p));
        }
        else {
            delete p;
        }
        
    }
    
    
}}

