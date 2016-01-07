#pragma once

#include <value/mysql/types.hpp>
#include <value/mysql/optional.hpp>
#include <mysql.h>

#include <value/mysql/connection_invariants.hpp>
#include "error.hpp"


namespace value { namespace mysql {

    
    struct connection_instance
    {
        struct pool;
        
        struct impl;
        using impl_ptr = std::shared_ptr<impl>;
        
        class create_from_impl {
            create_from_impl() {}
            friend pool;
        };

        connection_instance(const connection_invariants& invariants);
        connection_instance(create_from_impl, impl_ptr);
        
        auto close() noexcept -> void;
        
        /// Performs a ping on the server and indicate whether it was successful.
        /// this function shall not throw an error. Its purpose is to give an indication
        /// as to whether the server is still there at the time of the ping.
        auto pings_ok() noexcept -> bool;
        
        MYSQL* mysql() const;

    private:
        
        impl_ptr _impl;
    };
    

}}
