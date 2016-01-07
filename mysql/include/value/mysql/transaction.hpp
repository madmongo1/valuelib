#pragma once

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
        impl_ptr _impl;
        
    public:
        impl& implementation() const;
        struct connection_instance& connection_instance() const;
        
    };
    
    transaction make_transaction(connection con);
    
}}