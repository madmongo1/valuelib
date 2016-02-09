#pragma once

#include <boost/optional.hpp>
#include <memory>

namespace value { namespace stdext {
    
    using boost::optional;
    using boost::none;

    struct called_flag {};

    namespace detail
    {
        template<class Target, class F>
        struct weak_binder
        {
            using target_type = Target;
            using weak_ptr_type = std::weak_ptr<Target>;

            weak_binder(weak_ptr_type weak_ptr, F f)
            : _weak_ptr(std::move(weak_ptr))
            , _f(std::move(f))
            {}
            
            template<class...Args,
            class Result = std::result_of_t<F(Args...)>,
            std::enable_if_t<not std::is_void<Result>::value>* = nullptr>
            auto operator()(Args&&...args) const -> optional<Result>
            {
                auto locked_ptr = _weak_ptr.lock();
                if (locked_ptr)
                {
                    return _f(std::forward<Args>(args)...);
                }
                else
                {
                    return none;
                }
                
            }

            template<class...Args,
            class Result = std::result_of_t<F(Args...)>,
            std::enable_if_t<std::is_void<Result>::value>* = nullptr>
            auto operator()(Args&&...args) const -> optional<called_flag>
            {
                auto locked_ptr = _weak_ptr.lock();
                if (locked_ptr)
                {
                    _f(std::forward<Args>(args)...);
                    return called_flag {};
                }
                else
                {
                    return none;
                }
                
            }
            
            weak_ptr_type _weak_ptr;
            F _f;
        };
    }
    
    //
    // versions for handling member functions
    //
    
    template<class Ret, class Target, class...FuncArgs, class...Args>
    auto bind_weak(Ret (Target::*mfp)(FuncArgs...), const std::shared_ptr<Target>& ptr, Args&&...args)
    {
        using binder_type = decltype(std::bind(mfp, ptr.get(), std::forward<Args>(args)...));
        return detail::weak_binder<Target, binder_type>
        {
            std::weak_ptr<Target>(ptr),
            std::bind(mfp, ptr.get(), std::forward<Args>(args)...)
        };
    }

    // version for calling any callable when an unrelated weak_ptr can be locked
    template<class Callable, class Pointee, class...Args>
    auto bind_weak(Callable&& f, const std::shared_ptr<Pointee>& ptr, Args&&...args)
    {
        using binder_type = decltype(std::bind(std::forward<Callable>(f), std::forward<Args>(args)...));
        return detail::weak_binder<Pointee, binder_type>
        {
            std::weak_ptr<Pointee>(ptr),
            std::bind(std::forward<Callable>(f), std::forward<Args>(args)...)
        };
    }
    
    // optimisation for no arguments
    template<class Callable, class Pointee>
    auto bind_weak(Callable&& f, const std::shared_ptr<Pointee>& ptr)
    {
        return detail::weak_binder<Pointee, Callable>
        {
            std::weak_ptr<Pointee>(ptr),
            std::forward<Callable>(f)
        };
    }
    
    
    
    
    
    
}}