//
//  metafunction.hpp
//  ProjectX
//
//  Created by Richard Hodges on 25/02/2016.
//
//

#pragma once

namespace value { namespace tuple {
  
    namespace impl {
        template <typename T, typename Tuple>
        struct tuple_has_type;
        
        template <typename T>
        struct tuple_has_type<T, std::tuple<>> : std::false_type {};
        
        template <typename T, typename U, typename... Ts>
        struct tuple_has_type<T, std::tuple<U, Ts...>> : tuple_has_type<T, std::tuple<Ts...>> {};
        
        template <typename T, typename... Ts>
        struct tuple_has_type<T, std::tuple<T, Ts...>> : std::true_type {};
    }
    template<class Tuple, class Type>
    static constexpr bool TupleHasType = impl::tuple_has_type<std::decay_t<Type>, Tuple>::value;

}}