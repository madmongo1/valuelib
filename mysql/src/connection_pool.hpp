#include <value/mysql/connection_invariants.hpp>
#include "connection_instance.hpp"

#include <memory>
#include <map>
#include <deque>
#include <mutex>

namespace value { namespace mysql {
    
    struct connection_pool
    : std::enable_shared_from_this<connection_pool>
    {
        
        using free_connection_ptr = std::unique_ptr<connection_instance>;
        using free_connection_list = std::deque<free_connection_ptr>;
        using free_connection_map_type = std::map<connection_invariants, free_connection_list>;
        
        using mutex_type = std::mutex;
        using lock_type = std::unique_lock<mutex_type>;
        using guard_type = std::lock_guard<mutex_type>;
        
        auto acquire_connection(const connection_invariants& invariants)
        -> std::shared_ptr<connection_instance>;
        
    private:
        
        struct free_connection
        {
            free_connection(const connection_invariants& invariants,
                            std::weak_ptr<connection_pool> pool);
            void operator()(connection_instance* p) const noexcept;

        private:
            connection_invariants _invariants;
            std::weak_ptr<connection_pool> _weak_pool;
        };
        friend free_connection;
                            

        free_connection_ptr pop_free_connection(const connection_invariants& invariants)
        {
            guard_type lock(_mutex);
            auto& free_list = _free_connection_map[invariants];
            if (free_list.empty()) {
                return free_connection_ptr();
            }
            else {
                auto ptr = std::move(free_list.back());
                free_list.pop_back();
                return ptr;
            }
        }
        
        void push_free_connection(const connection_invariants& invariants, free_connection_ptr connection) noexcept
        {
            
        }
        
        
    private:
        
        mutex_type _mutex;
        free_connection_map_type _free_connection_map;
    };
}}