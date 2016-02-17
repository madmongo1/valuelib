#pragma once
#include "metafunctions.hpp"
#include "storage.hpp"

#include "type_traits.hpp"

namespace value { namespace data {

    /*
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
*/
    template<class Tag>
    struct column
    {
        using tag_type = Tag;

        // must be overridden
        static constexpr auto identifier() { return Tag::identifier(); }
        
        // must be overridden
        static constexpr auto storage() { return Tag::storage(); }
        
        static constexpr auto default_value() { return no_default(); }
    };
    template<class T, typename = void> struct is_column : std::false_type {};
    template<class Type>
    struct is_column<Type, std::enable_if_t<is_derived_from_template_v<Type, column>>> : std::true_type {};
    template<class T> static constexpr bool is_column_v = is_column<T>::value;
    
    
    namespace metafunction { namespace impl {
        
        template<class Column>
        struct native_arg_type<Column, std::enable_if_t<is_column_v<Column>>>
        {
            using argument_type = Column;
            using storage_type = decltype(std::declval<Column>().storage());
            using result = metafunction::native_arg_type<storage_type>;
        };
    }}

	template<class...Columns>
	struct column_list
	{
        static constexpr auto as_tuple() { return std::tuple<Columns...>{}; }
	};
}}