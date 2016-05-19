#pragma once
#include "entity.hpp"
#include <vector>
#include <functional>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <utility>
#include <valuelib/debug/demangle.hpp>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>

namespace value { namespace data { namespace database {

	template<class Database>
	struct ddl_context
	{
		using database_type = Database;
		using entity_type = entity<database_type>;
		using entity_reference = std::reference_wrapper<const entity_type>;
        
        using vector_type = std::vector<entity_reference>;
        using iterator = typename vector_type::iterator;
        
        std::size_t index_entity(const entity_type& entity)
        {
            auto it = std::find_if(std::begin(_entities), std::end(_entities),
                                   [&entity](auto& r) {
                                       return entity == r.get();
                                   });
            if (it == std::end(_entities)) {
                it = _entities.insert(it, entity);
            }
            return std::distance(std::begin(_entities), it);
        }
        
        void register_entity(const entity_type& entity,
                             std::size_t dependent_idx)
        {
            auto i = index_entity(entity);
            _edges.emplace_back(dependent_idx, i);
            for (auto dependency : entity.dependencies())
            {
                register_entity(dependency, i);
            }
        }
        
        void register_entity(const entity_type& entity)
        {
            auto i = index_entity(entity);
            for (auto dependency : entity.dependencies())
            {
                register_entity(dependency, i);
            }
            
            // remove duplicate graph entries
            std::sort(std::begin(_edges), std::end(_edges));
            _edges.erase(std::unique(std::begin(_edges), std::end(_edges)),
                         std::end(_edges));
        }
        /*

		bool register_entity(const entity_type& entity, std::size_t first_legal_index = 0)
        try
		{
            auto it = std::find_if(begin(), end(), [&entity](auto& r) {
                return entity = r.get();
            });
            if (it == end())
            {
                
            }
            // see whether the entity has been seen before
            auto it = std::find_if(begin(), end(),
                                   [&entity](auto r){ return entity == r.get(); });
            
            // if so, then if it appears to be a dependency of this one, then this is an error
            if (to_index(it) < first_legal_index) {
                report_circular_dependency(entity, first_legal_index);
//                throw std::invalid_argument("circular dependency detected");
            }
            
            // otherwise, no work to do
            if (it != end())
                return false;
            
            // now find the position in the registry of the first of our dependencies
            const auto& dependencies = entity.dependencies();
            it = std::find_first_of(begin(), end(),
                                    std::begin(dependencies), std::end(dependencies),
                                    [](auto l, auto r) { return l.get() == r.get(); });
            if (to_index(it) < first_legal_index) {
                report_circular_dependency(entity, first_legal_index);
//                throw std::invalid_argument("circular dependency detected");
            }
            
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
        */
    
        void report_circular_dependency(const entity_type& entity, std::size_t first_legal_index)
        {
            std::ostringstream ss;
            ss << "entity list:\n";
            for (const auto& e : _entities)
            {
                ss << value::debug::demangle(typeid(e.get())) << '\n';
            }
            ss << "circular dependency detected inserting "
            << value::debug::demangle(typeid(entity))
            << " as first index " << first_legal_index;
            throw std::invalid_argument(ss.str());
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
        
        using edge = std::pair<std::size_t, std::size_t>;
        using edge_vector = std::vector<edge>;
        
        typedef boost::adjacency_list<boost::vecS, boost::vecS,
        boost::bidirectionalS,
        boost::property<boost::vertex_color_t, boost::default_color_type>
        > Graph;
        typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;

        struct cycle_detector : public boost::dfs_visitor<>
        {
            cycle_detector( edge_vector& back_edges)
            : _back_edges(back_edges)
            { }
            
            void back_edge(const boost::graph_traits<Graph>::edge_descriptor& e, const Graph&) const {
                _back_edges.emplace_back(e.m_source, e.m_target);
            }
        protected:
            edge_vector& _back_edges;
        };
        

        auto generate_graph() const {
            Graph g(std::begin(_edges), std::end(_edges), _entities.size());
            return g;
        }
        
        void check_cycles(const Graph& g) const
        {
            edge_vector back_edges;
            cycle_detector vis(back_edges);
            boost::depth_first_search(g, visitor(vis));
            
            if (back_edges.size())
            {
                std::ostringstream ss;
                ss << "cyclic dependency detected. Back edges are:\n";
                for (auto& p : back_edges)
                {
                    ss << value::debug::demangle(typeid(_entities[p.first].get()))
                    << " <<==>> "
                    << value::debug::demangle(typeid(_entities[p.second].get()))
                    << std::endl;
                }
                throw std::logic_error(ss.str());
            }
        }

        auto create_order() const {
            using namespace boost;
            
            auto g = generate_graph();
            check_cycles(g);
            
            std::vector<std::size_t> result_order;
            vector_type result;
            
            boost::topological_sort(g, std::back_inserter(result_order));
            std::transform(std::begin(result_order), std::end(result_order),
                           std::back_inserter(result), [this](auto i) {
                               return _entities[i];
                           });
            
            std::cout << "make ordering: ";
            for (auto ref : result)
            {
                std::cout << value::debug::demangle(typeid(ref.get())) << std::endl;
            }
            return result;
        }
        
        auto drop_order() const {
            auto result = create_order();
            std::reverse(std::begin(result), std::end(result));
            return result;
        }
        

    private:
        

        
        edge_vector _edges;
		vector_type _entities;
	};

}}}