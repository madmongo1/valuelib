#pragma once
#include <utility>

namespace value { namespace data { namespace database {

	template<class Database>
	struct entity
	{
        using database_type = Database;
        using database_traits_type = database_traits<database_type>;
        using ddl_statement_type = typename database_traits_type::ddl_statement_type;
        
        using reference_type = std::reference_wrapper<const entity>;
        using dependency_list_type = std::vector<reference_type>;
	protected:
		// constructor is protected - must be derived from
		entity() = default;

		// may not be copied
		entity(const entity&) = delete;
		entity& operator=(const entity&) = delete;

	public:
		/// Two entities compare equal if they are at the same address
		bool operator==(const entity& r) const {
			return this == std::addressof(r);
		}

        /// Cause the underlying database engine to create the entity
		virtual void create(ddl_statement_type& stmt) const = 0;

        /// Cause the underlying database engine to drop the entity
		virtual void drop(ddl_statement_type& stmt) const = 0;

		/// return a vector of references to entities that this one depends on
		virtual const dependency_list_type& dependencies() const = 0;
	};

}}}