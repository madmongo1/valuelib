#include "connection_impl.hpp"

namespace value { namespace mysql {


    connection_instance connection::impl::acquire_connection_instance()
    {
        return connection_instance(_connection_pool.acquire());
    }
}}