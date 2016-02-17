#pragma once

namespace value { namespace data { namespace connectors {

	// translates data storage types to connector types do the connector drivers
	// can convert data
	namespace cppconn
	{
        class string_type {};
        class stream_type {};
        class unsigned_int_type {};
        class signed_int_type {};

        namespace metafunction
        {
            namespace impl
            {
                template<class EntityType, typename = void> struct connector_type;

                template<class Column>
                struct connector_type<Column, std::enable_if_t< is_column_v<Column> > >
                {
                    using column_type = Column;
                    using storage_type = decltype(column_type::storage());
                    using result = typename connector_type<storage_type>::result;
                };

                template<class NativeType, class Nullable>
                struct connector_type<data::uuid_storage<NativeType, Nullable>>
                {
                    using result = string_type;
                };
            };
            template<class EntityType>
            using connector_type = typename impl::connector_type<EntityType>::result;
        };


	};

}}}