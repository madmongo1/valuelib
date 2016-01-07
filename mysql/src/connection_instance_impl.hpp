#pragma once
#include "connection_instance.hpp"
#include <mysql.h>

namespace value { namespace mysql {

    struct connection_instance::impl
    {        
        impl(const connection_invariants& invariants);

        impl(const connection_instance&) = delete;
        impl& operator=(const connection_instance&) = delete;
        
        ~impl() noexcept;
        
        auto close() noexcept -> void;
        
        /// Performs a ping on the server and indicate whether it was successful.
        /// this function shall not throw an error. Its purpose is to give an indication
        /// as to whether the server is still there at the time of the ping.
        auto pings_ok() noexcept -> bool;
        
        MYSQL* mysql() noexcept { return std::addressof(_mysql); }

    private:
        
        
        struct mysql_proxy : MYSQL {
            mysql_proxy() noexcept {
                mysql_init(this);
            }
        };
        mysql_proxy _mysql { };
        bool _connected = false;
    };

}}
