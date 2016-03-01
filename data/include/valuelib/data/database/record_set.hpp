//
//  record_set.hpp
//  ProjectX
//
//  Created by Richard Hodges on 29/02/2016.
//
//
#pragma once
#include <valuelib/data/deduce_nartive.hpp>

namespace value { namespace data { namespace database {
    
    template<class Seek, class Element>
    struct match_element : std::false_type {};
    
    template<class Seek>
    struct match_element<Seek, Seek> : std::true_type {};
    
    template<class Seek>
    struct match_element<boost::optonal<Seek>, Seek> : std::true_type {};
    
    namespace impl {
        template<class Tuple> struct tuple_begin;
        template<class Tuple> struct tuple_next;
        template<class Tuple> struct tuple_end;
        template<class Tuple> struct tuple_find;
        template<class IterL, class IterR> struct tuple_same_iter : std::is_same<IterL, IterR> {};
    }
    template<class Tuple>
    using TupleBegin = impl::tuple_begin<Tuple>;
    
    template<class Tuple> using TupleNext = impl::tuple_next<Tuple>::result;
    
    template<class Tuple> struct TupleEnd = impl::tuple_end<Tuple>;
    
    
    template<class Tuple, class Predicate> using TupleFindIf = impl::tuple_find<Tuple, Predicate>;
    
    template<class...Ts>
    struct TupleBegin<std::tuple<Ts...>>
    {
        using as_tuple = std::tuple<Ts...>;
        static constexpr size_t position = 0;
    };
    
    template<class...Ts>
    struct TupleEnd
    
    
    
   
    template<class...StorageTypes>
    struct record_set
    {
        using native_types = DeduceNativeTypes<std::tuple<StorageTypes>>;
        
        struct require_impl<boost::optional<T>> {
            template<std::size_t Index, class Tuple>
            const T& apply(Tuple const& tuple) const
            {
                auto& opt = std::get<Index>(tuple);
                return opt.value();
            }
        };
        
        struct require_impl<T> {
            template<std::size_t Index, class Tuple>
            const T& apply(Tuple const& tuple) const
            {
                return std::get<Index>(tuple);
            }
        };
        
        template<class Type>
        const Type& require() const {
            
            using Iter = TupleFindIf<native_types, SameOrOptional<Type>>;
            using Type = Dereference<Iter>;
            
            auto impl = require_impl<Type>();
            return impl.get();
        }

        

        struct require_impl<boost::optional<T>> {
            template<std::size_t Index, class Tuple>
            const T& apply(Tuple const& tuple) const
            {
                auto& opt = std::get<Index>(tuple);
                return opt.value();
            }
        };
        
        struct require_impl<T> {
            template<std::size_t Index, class Tuple>
            const T& apply(Tuple const& tuple) const
            {
                return std::get<Index>(tuple);
            }
        };


        template<class Type>
        const Type& optional() const {
            
            using Iter = TupleFindIf<native_types, SameOrOptional<Type>>;
            using Type = Dereference<Iter>;
            
            auto impl = optional_impl<Type>();
            return impl.get();
        }
        
        
        
        native_types _current_data;
    };
}}}