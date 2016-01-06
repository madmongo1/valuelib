#pragma once

#include <utility>
#include <tuple>
#include <value/mysql/optional.hpp>

namespace value { namespace mysql {
    
    
    namespace detail
    {
        template<class T>
        using Reduce = std::remove_reference_t<std::remove_cv_t<T>>;
        
        template<class T> struct is_tuple : std::false_type {};
        template<class...Elements> struct is_tuple<std::tuple<Elements...>> : std::true_type {};
        
        template<class T>
        static constexpr bool IsTuple = is_tuple<std::decay_t<T>>::value;

        template<class Type, std::size_t N, std::size_t TupleSize>
        struct optional_extracter
        {
            template<
            class Tuple,
            std::enable_if_t
            <
            std::is_same<Type, Reduce< std::tuple_element_t<N, Tuple> > >::value
            >* = nullptr
            >
            optional<Type> operator()(Tuple&& t) const
            {
                return optional<Type>(std::get<N>(std::forward<Tuple>(t)));
            }
            
            template<
            class Tuple,
            std::enable_if_t
            <
            not std::is_same<Type, Reduce< std::tuple_element_t<N, Tuple> >>::value
            >* = nullptr
            >
            optional<Type> operator()(Tuple&& t) const
            {
                return optional_extracter<Type, N + 1, TupleSize>()(std::forward<Tuple>(t));
            }
            
        };
        
        template<class Type, std::size_t N>
        struct optional_extracter<Type, N, N>
        {
            template<class Tuple>
            optional<Type> operator()(Tuple&&) const {
                return { nullopt };
            }
        };
        
    }
    
    template<class Type, class Tuple>
    optional<Type> extract_optional(Tuple&& t)
    {
        static constexpr size_t size = std::tuple_size<std::decay_t<Tuple>>()();
        return detail::optional_extracter<Type, 0, size>()(std::forward<Tuple>(t));
    }
    
}}