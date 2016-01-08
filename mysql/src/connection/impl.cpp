#include <value/mysql/mysql_api.hpp>

#include "impl.hpp"

namespace value { namespace mysql {

    
    connection::impl::impl(connection_parameters params)
    : MYSQL { {0} }
    , _params(params)
    , _open(false)
    {
        if (not mysql_init(this))
        {
            throw std::runtime_error("initialisation failure");
        }
        
        _need_close = true;
        
        connect(this, _params);

        _open = true;
    }
    
    connection::impl::~impl() noexcept
    {
        if (_need_close) {
            mysql_close(this);
        }
    }
    


    
}}