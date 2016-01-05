#pragma once

#include <value/mysql/types.hpp>

#include <exception>
#include <stdexcept>

namespace value {
	namespace mysql {

	    struct database_error : std::runtime_error {
    		using std::runtime_error::runtime_error;
    	};

	    struct statement_error : std::runtime_error {
    		using std::runtime_error::runtime_error;
    	};

    	struct connection_error : std::runtime_error
    	{
    		using std::runtime_error::runtime_error;
    	};


	}
}