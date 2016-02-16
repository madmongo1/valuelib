#pragma once 

namespace value { namespace data {
	namespace metafunction {

	/// Return the type of the native type of an entity.
	/// The native type is the type thr eprogram should use to  progive this value as an argument
		namespace impl {
			template<class Entity>
			struct native_arg_type;
		}

		template<class Entity>
        using native_arg_type = typename impl::native_arg_type<Entity>::result;

        
        ///
        /// get a list of entity types that this entity depends on
        ///
        namespace impl {
            template<class Entity>
            struct entity_dependencies
            {
//                using result = std::tuple<>;
            };
        }
        
        template<class Entity> using entity_dependencies =
        typename impl::entity_dependencies<Entity>::result;

        

    }
}}