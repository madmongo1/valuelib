#pragma once

#include <value/mysql/types.hpp>
#include <value/mysql/mysql_api.hpp>
#include <value/mysql/optional.hpp>
#include <value/mysql/connection_invariants.hpp>

#include <string>
#include <boost/variant.hpp>
#include <typeinfo>

namespace value  { namespace mysql {
    
    
    
    struct results {
        
    };
    

    struct connection
    {
        template<class...Args, std::enable_if_t<AreAllConnectionArguments<Args...>>* = nullptr >
        connection(Args&&...args)
        : connection {
            connection_invariants {
                std::forward_as_tuple(std::forward<Args>(args)...)
            }
        }
        {
        }
        
        connection(const connection_invariants& params);
        
        auto begin_transaction() -> void;
        auto commit(bool reopen = false) -> void;
        auto rollback() -> void;
        auto rollback(without_exception_t) noexcept -> void;
        

        
        shared_mysql_ptr mysql() const;
        
        // private implementation
    private:
        struct library_initialiser;
        static library_initialiser& acquire_library();

        struct impl;
        using impl_ptr = std::shared_ptr<impl>;
        impl_ptr make_impl(connection_parameters params);
        impl_ptr _impl;
        
    public:
        struct transaction_sentinel;
        transaction_sentinel get_transaction_sentinel();
        
    };
    
}}

