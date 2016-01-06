#pragma once
#include <value/mysql/error.hpp>

#include <mysql.h>

namespace value { namespace mysql {

	[[noreturn]]
	inline bool throw_database_error(MYSQL& mysql)
	{
		throw database_error(mysql_error(std::addressof(mysql)));
	}
}}