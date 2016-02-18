#pragma once
#include "entity.hpp"
#include <vector>
#include <functional>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <valuelib/debug/demangle.hpp>

namespace value { namespace data { namespace database {

	template<class Database>
	struct ddl_context
	{
		using database_type = Database;
		using entity_type = entity<database_type>;
		using entity_reference = std::reference_wrapper<const entity_type>;
        
        using vector_type = std::vector<entity_reference>;
        using iterator = typename vector_type::iterator;
        

		bool register_entity(const entity_type& entity, std::size_t first_legal_index = 0)
        try
		{
            // see whether the entity has been seen before
            auto it = std::find_if(begin(), end(), [&entity](auto r){ return entity == r.get(); });
            
            // if so, then if it appears to be a dependency of this one, then this is an error
            if (to_index(it) < first_legal_index)
                throw std::invalid_argument("circular dependency detected");
            
            // otherwise, no work to do
            if (it != end())
                return false;
            
            // now find the position in the registry of the first of our dependencies
            const auto& dependencies = entity.dependencies();
            it = std::find_first_of(begin(), end(),
                                    std::begin(dependencies), std::end(dependencies),
                                    [](auto l, auto r) { return l.get() == r.get(); });
            if (to_index(it) < first_legal_index)
                throw std::invalid_argument("circular dependency detected");
            
            it = _entities.insert(it, entity);
            first_legal_index = to_index(it) + 1;
            for (auto dep : dependencies)
            {
                register_entity(dep, first_legal_index);
            }
            return true;
		}
        catch(const std::exception& e)
        {
            using namespace std::string_literals;
            std::throw_with_nested(std::logic_error("register_entity("s +
                                                    debug::demangle(typeid(entity))+
                                                    ", " +
                                                    std::to_string(first_legal_index) +
                                                    ")"));
        }

        iterator begin() { return std::begin(_entities); }
        iterator end() { return std::end(_entities); }
        std::size_t to_index(iterator iter) { return iter - begin(); }
        
        struct reverse_proxy
        {
            using iterator_type = typename vector_type::const_reverse_iterator;
            
            reverse_proxy(iterator_type first, iterator_type last)
            : _begin(first), _end(last)
            {}
            
            auto begin() const { return _begin; }
            auto end() const { return _end; }
            
            iterator_type _begin, _end;
        };
        
        struct forward_proxy
        {
            using iterator_type = typename vector_type::const_iterator;
            
            forward_proxy(iterator_type first, iterator_type last)
            : _begin(first), _end(last)
            {}
            
            auto begin() const { return _begin; }
            auto end() const { return _end; }
            
            iterator_type _begin, _end;
        };
        
        auto drop_order() const {
            return forward_proxy(std::begin(_entities), std::end(_entities));
        }

        auto create_order() const {
            return reverse_proxy(std::rbegin(_entities), std::rend(_entities));
        }
    private:

		vector_type _entities;
	};

}}}