#pragma once

#include <valuelib/data/sql/dialect.hpp>
#include <valuelib/data/identifier.hpp>

namespace value { namespace data { namespace sql { namespace mysql {

	struct dialect : ::value::data::sql::dialect_base<dialect>
	{
		VALUE_DATA_IDENTIFIED_BY(mysql);
	};


}}}}

#include "create.hpp"
#include "drop.hpp"
#include "insert.hpp"

