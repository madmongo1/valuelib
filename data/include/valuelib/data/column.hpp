#pragma once
#include "metafunctions.hpp"
#include "storage.hpp"

namespace value { namespace data {

	template<class Tag, class StorageDefinition, class DefaultValue = no_default>
	struct column
	{
		using tag_type = Tag;
		using storage_type = StorageDefinition;

		static constexpr auto identifier() {
			return Tag::identifier();
		}
        
        static constexpr auto storage() {
            return StorageDefinition();
        }
        
        static constexpr auto default_value() {
            return DefaultValue();
        }
	};
    namespace metafunction { namespace impl {
        
        template<class Tag, class StorageDefinition>
        struct native_arg_type<column<Tag, StorageDefinition>> {
            using argument_type = column<Tag, StorageDefinition>;
            using storage_type = typename argument_type::storage_type;
            using result = metafunction::native_arg_type<storage_type>;
        };
    }}

	template<class...Columns>
	struct column_list
	{
        static constexpr auto as_tuple() { return std::tuple<Columns...>{}; }
	};
}}