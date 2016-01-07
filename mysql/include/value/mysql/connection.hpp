#pragma once

#include <value/mysql/types.hpp>
#include <value/mysql/optional.hpp>
#include <value/mysql/connection_invariants.hpp>
#include <value/mysql/transaction.hpp>

#include <string>
#include <boost/variant.hpp>
#include <typeinfo>

namespace value  { namespace mysql {
    
    
    
    struct results {
        
    };
    
    
    
    
    
    struct connection_instance;
    
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

        
        struct transaction transaction();
        
        
        
        // private implementation
    private:
        struct impl;
        using impl_ptr = impl*;
        static auto acquire_impl(const connection_invariants& params) -> impl_ptr;
        impl_ptr _impl;
        
        // internal interface
    public:
        class transaction_access_key {
            constexpr transaction_access_key() {};
            friend struct transaction;
        };
        
        connection_instance acquire_connection_instance(transaction_access_key);
        

    };
    
}}

