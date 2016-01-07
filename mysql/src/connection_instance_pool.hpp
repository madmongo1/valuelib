#pragma once

#include <value/mysql/connection_invariants.hpp>
#include "connection_instance.hpp"

#include <memory>

namespace value { namespace mysql {
    
    struct connection_instance::pool
    {
        pool(const connection_invariants& params);
        
        connection_instance acquire();
        
        struct impl;
        std::shared_ptr<impl> _impl_ptr;
    };
}}