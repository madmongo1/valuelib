#include "connection_instance_impl.hpp"
#include <value/mysql/error.hpp>
#include "error.hpp"

#include <exception>
#include <cassert>

using namespace std::literals;

namespace value { namespace mysql {
    
    
    connection_instance::impl::impl(const connection_invariants& params)
    try {
        mysql_real_connect(std::addressof(_mysql),
                           params.get_host(), params.get_user(),
                           params.get_passwd(),
                           params.get_db(),
                           params.get_port(),
                           params.get_unix_socket(),
                           params.get_client_flag())
        or throw_database_error(_mysql);
        _connected = true;
        
        (0 == mysql_autocommit(mysql(), 0)) or throw_database_error(_mysql);
    }
    catch(...) {
        std::throw_with_nested(connection_error("connection_context: "s + to_string(params)));
    }
    
    connection_instance::impl::~impl() noexcept {
        close();
    }
    
    auto connection_instance::impl::close() noexcept -> void
    {
        if (_connected) {
            _connected = false;
            mysql_close(std::addressof(_mysql));
        }
    }
    
    auto connection_instance::impl::pings_ok() noexcept -> bool
    {
        // precondition : the connection must be open
        assert(_connected);
        
        return 0 == mysql_ping(std::addressof(_mysql));
    }
    
    
    
    
}}