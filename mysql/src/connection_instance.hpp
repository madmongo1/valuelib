#pragma once

#include <value/mysql/types.hpp>
#include <value/mysql/optional.hpp>
#include <mysql.h>

#include <value/mysql/connection_invariants.hpp>
#include "error.hpp"


namespace value { namespace mysql {

    
    struct connection_instance
    {
        connection_instance(const host& host,
                            port port,
                            const username& user,
                            const password& password,
                            const schema& default_schema);
        
        connection_instance(const connection_invariants& invariants);

        connection_instance(const connection_instance&) = delete;
        connection_instance& operator=(const connection_instance&) = delete;
        
        ~connection_instance() noexcept;
        
        auto close() noexcept -> void;
        
        /// Performs a ping on the server and indicate whether it was successful.
        /// this function shall not throw an error. Its purpose is to give an indication
        /// as to whether the server is still there at the time of the ping.
        auto pings_ok() noexcept -> bool;
        
        MYSQL _mysql;
        bool _need_close = false;
    };
    

}}
