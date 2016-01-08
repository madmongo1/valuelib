#pragma once

#include <value/mysql/mysql_api.hpp>
#include <value/mysql/connection.hpp>

#include <string>
#include <memory>

namespace value { namespace mysql {

    struct connection;
    
    struct connection_instance;
    struct transaction
    {
        transaction(struct connection con);

        const struct connection& connection() const;
        
    private:
        struct impl;
        using impl_ptr = std::shared_ptr<impl>;
        impl_ptr _impl_ptr;
    };
    
    transaction make_transaction(const connection& con);
    
}}