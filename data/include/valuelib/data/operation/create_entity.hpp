#pragma once

namespace value { namespace  data { namespace operation {

/// Meta data that describes the operation of creating a data entity in a database
/*
	template<class Entity>
	struct create_entity
	{
		using entity_type = Entity;
	};



	struct execute_update_statement {};

	update table set 

	entity: table;
	has: identifier, columns, primary_key, indexes
	depends on: nothing

	in sql: has create_statement, drop statement
	in ptree: creation in adding value to the ptree, drop is deleting the ptree


	so every entity must have a create, drop and execute function

    basic concepts:
    database connection:


    value::data::ptree::driver
    value::data::ptree::database(ptree&)
    value::data::ptree::table(); (a child of "tables")
    value::data::ptree::construct<entity>(entity)
    value::data::ptree::drop<entity>(entity)
    value::data::query<query_type>(query_type)
    value::data::update<query_type>(query_type)

    secr::models::mysql::driver - > model of value::data::driver;
    secr::models::mysql::database - > model of value::data::driver;



    value::data::driver::create_entity<value::data::ptree_database(), table<fields...>>::apply

    for each database {
    	if (mysql) { 
    		register mysql database(database)
    	}
    	if (json) { 
    		register json database(database)
    	}
    }

    for each subsystem {
    	if (http_session) {
    		register_http_session(database(database).type(), );

    	}
    }

    */


}}}