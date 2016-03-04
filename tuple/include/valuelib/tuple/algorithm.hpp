//
//  metafunction.hpp
//  ProjectX
//
//  Created by Richard Hodges on 25/02/2016.
//
//

#pragma once
#include <tuple>
#include <utility>

namespace value { namespace tuple {

    namespace impl
    {
        //! Tests if T is a specialization of Template
        template <typename T, template <typename...> class Template>
        struct is_specialization_of : std::false_type {};
        
        template <template <typename...> class Template, typename... Args>
        struct is_specialization_of<Template<Args...>, Template> : std::true_type {};
        
        template<class Func, class Tuple, std::size_t...Is>
        void for_each(Tuple&& tuple, Func&& func, std::index_sequence<Is...>)
        {
            (std::forward<Func>(func)(std::get<Is>(std::forward<Tuple>(tuple))),...);
        }
    }
    template<typename T, template <typename...> class Template>
    static constexpr auto IsSpecializationOf = impl::is_specialization_of<std::decay_t<T>, Template>::value;

    template<
    class Func,
    class Tuple,
    std::enable_if_t<IsSpecializationOf<Tuple, std::tuple>>* = nullptr
    >
    void for_each(Tuple&& tuple, Func&& func)
    {
        using tuple_type = std::decay_t<Tuple>;
        static constexpr auto tsize = std::tuple_size<tuple_type>::value;
        
        return impl::for_each(std::forward<Tuple>(tuple),
                              std::forward<Func>(func),
                              std::make_index_sequence<tsize>());
    }
}}