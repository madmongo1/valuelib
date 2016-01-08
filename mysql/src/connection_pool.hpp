#pragma once
#include <value/mysql/mysql_api.hpp>

namespace value { namespace mysql {
  
    struct connection_pool
    {
        connection_pool();
        shared_mysql_ptr pop();
        shared_mysql_ptr take_ownership(mysql_ptr mysql);
        
        
    private:
        struct impl;
        std::shared_ptr<impl> _impl_ptr;

        std::weak_ptr<impl> get_weak_impl();
};

}}