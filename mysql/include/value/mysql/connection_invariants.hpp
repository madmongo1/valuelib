#pragma once

#include <value/mysql/types.hpp>
#include <value/mysql/extract_optional.hpp>

#include <tuple>
#include <ostream>

namespace value { namespace mysql {


	/// Arguments that describe a connection to a mysql server
    /// the mysql_real_connect arguments are deduced from here
	struct connection_invariants
	{
        struct client_option_builder
        {
            client_option_builder() noexcept;
            operator client_options() const;
            
        private:
            client_options _options;
        };
        

        template<class...Args, std::enable_if_t< AreAllConnectionArguments<Args...> >* = nullptr>
        connection_invariants(std::tuple<Args...>&& args)
        : _host(extract_optional<host>(std::move(args)))
        , _user(extract_optional<username>(std::move(args)))
        , _pwd(extract_optional<password>(std::move(args)))
        , _default_schema(extract_optional<schema>(std::move(args)))
        , _port(default_port(extract_optional<port>(std::move(args))))
        , _unix_socket(extract_optional<unix_socket>(std::move(args)))
        , _client_flag(client_option_builder())
        {}
        
        template<class...Args, std::enable_if_t< AreAllConnectionArguments<Args...> >* = nullptr>
        connection_invariants(Args&&...args)
        : connection_invariants(std::forward_as_tuple(std::forward<Args>(args)...))
        {}
        
        static port default_port(optional<port>&& p) {
            if (p) {
                return *p;
            }
            else {
                return port(0);
            }
        }
        
        optional<host> _host;
        optional<username> _user;
        optional<password> _pwd;
        optional<schema> _default_schema;
        port _port;
        optional<unix_socket> _unix_socket;
        client_options _client_flag;
        
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
    
    std::string to_string(const connection_invariants& ci);
    
    std::size_t hash_code(const connection_invariants& r);
    
    
    using connection_parameters = connection_invariants;
    
}}