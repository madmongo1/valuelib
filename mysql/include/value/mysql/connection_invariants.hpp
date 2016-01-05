#pragma once

#include <value/mysql/types.hpp>

#include <boost/optional.hpp>
#include <tuple>
#include <ostream>

namespace value { namespace mysql {


	/// Arguments that describe a connection to a mysql server
    /// the mysql_real_connect arguments are deduced from here
	struct connection_invariants
	{
        boost::optional<host> _host;
        boost::optional<username> _user;
        boost::optional<password> _pwd;
        boost::optional<schema> _default_schema;
        port _port;
        boost::optional<unix_socket> _unix_socket;
        unsigned long _client_flag;
        
        auto to_tuple() const {
            return std::tie(_host, _user, _pwd, _default_schema, _port, _unix_socket, _client_flag);
        }
        
        std::ostream& safe_print(std::ostream& os) const;
        
        const char* get_host() const;
        const char* get_user() const;
        const char* get_passwd() const;
        const char* get_db() const;
        unsigned int get_port() const;
        const char* get_unix_socket() const;
        unsigned long get_client_flag() const;
	};
    
    inline bool operator<(const connection_invariants& l, const connection_invariants& r)
    {
        return l.to_tuple() < r.to_tuple();
    }
    
    inline bool operator==(const connection_invariants& l, const connection_invariants& r)
    {
        return l.to_tuple() == r.to_tuple();
    }
    
    inline std::ostream& operator<<(std::ostream& os, const connection_invariants& i) {
        return i.safe_print(os);
    }
    
    std::size_t hash_code(const connection_invariants& r);
    
}}