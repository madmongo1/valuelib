/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   light_future.hpp
 * Author: Richard Hodges <hodges.r@gmail.com>
 *
 * Created on 14 May 2016, 19:16
 */
#pragma once

#include <exception>
#include <utility>
#include <future>

namespace value { namespace stdext {
    
    namespace detail {
        struct void_state {};
        
        template<class Stored> struct value_promised_from_store
        {
            using type = Stored;
        };
        
        template<>
        struct value_promised_from_store<void_state>
        {
            using type = void;
        };
        
        template<class Stored>
        using value_promised_from_store_t =
        typename value_promised_from_store<Stored>::type;
        
        template<class Type>
        struct shared_state_storage_type
        {
            using type = Type;
        };
        
        template<class Type>
        using shared_state_storage_type_t =
        typename shared_state_storage_type<Type>::type;
        
        template<>
        struct shared_state_storage_type<void>
        {
            using type = void_state;
        };
        
        template<class StoreType>
        StoreType& convert_from_store(StoreType& from)
        {
            return from;
        }
        
        void convert_from_store(void_state&)
        {}
        
        template<class ValueType>
        ValueType convert_to_store(ValueType from)
        {
            return from;
        }
        
        void_state convert_to_store(void)
        {
            return void_state();
        }
    }
    
    struct shared_state_common
    {
        enum class which_type {
            no_state,
            value,
            exception
        };
        
        constexpr shared_state_common(which_type type)
        : _which(type)
        {}
        
        constexpr which_type which() const noexcept {
            return _which;
        }
        
    protected:
        constexpr void set_which(which_type new_state) 
        {
            _which = new_state;
        }
        
    private:
        which_type _which = which_type::no_state;
    };
    
    template<class T>
    struct light_future;
    
    template<class T>
    struct shared_state
    : shared_state_common
    {
        using type = T;
        using store_type = detail::shared_state_storage_type_t<T>;
        
        shared_state()
        : shared_state_common(which_type::no_state)
        , _promises(0)
        {}
        
        shared_state(const shared_state&) = delete;
        shared_state& operator=(const shared_state& r) = delete;
        
        void set_exception(std::exception_ptr ep)
        {
            switch(this->which())
            {
                case which_type::no_state:
                    new (std::addressof(_exception)) std::exception_ptr (std::move(ep));
                    this->set_which(shared_state_common::which_type::exception);
                    break;
                    
                case which_type::exception:
                case which_type::value:
                    throw std::future_error(std::future_errc::promise_already_satisfied);
            }
        }
        
        template<
        class E,
        std::enable_if_t<std::is_base_of<std::exception, E>::value>* = nullptr
        >
        void set_exception(E&& e)
        {
            set_exception(std::make_exception_ptr(std::forward<E>(e)));
        }
        
        void set_value(store_type value)
        {
            switch(this->which())
            {
                case which_type::no_state:
                    new (std::addressof(_value)) store_type (std::move(value));
                    set_which(shared_state_common::which_type::value);
                    break;
                    
                case which_type::exception:
                case which_type::value:
                    throw std::future_error(std::future_errc::promise_already_satisfied);
            }
        }
        
        bool has_state() const noexcept {
            switch(this->which())
            {
                case which_type::no_state:
                    return false;
                    
                case which_type::exception:
                case which_type::value:
                    return true;
            }
        }
        
        bool has_exception() const noexcept {
            switch(this->which())
            {
                case which_type::no_state:
                case which_type::value:
                    return false;
                    
                case which_type::exception:
                    return true;
            }
        }
        
        bool has_value() const noexcept {
            switch(this->which())
            {
                case which_type::no_state:
                case which_type::exception:
                    return false;
                    
                case which_type::value:
                    return true;
            }
        }
        
        decltype(auto)
        reference()
        {
            switch(which())
            {
                case which_type::no_state:
                    throw std::future_error(std::future_errc::no_state);
                    break;
                    
                case which_type::exception:
                    std::rethrow_exception(_exception);
                    break;
                    
                case which_type::value:
                    return detail::convert_from_store(_value);
            }
        }
        
        decltype(auto) get() {
            return reference();
        }
        
        void add_promise() noexcept
        {
            ++_promises;
        }
        
        void remove_promise()
        {
            try {
                if (--_promises == 0 and not has_state()) {
                    set_exception(std::future_error(std::future_errc::broken_promise));
                }
            }
            catch(...) {
                assert(!"this should not throw");
            }
        }
        
        ~shared_state()
        {
            assert(!_promises);
            switch (this->which())
            {
                case which_type::no_state:
                    assert(!"should have some state");
                    break;

                case shared_state_common::which_type::value:
                    _value.~store_type();
                    break;
                
                case shared_state_common::which_type::exception:
                    _exception.~exception_ptr();
                    break;
            }
        }
        
        union
        {
            store_type             _value;
            std::exception_ptr     _exception;
        };
        std::size_t _promises;
    };
    
    template<class T, class Result>
    struct shared_state_with_completion
    : shared_state<T>
    , std::enable_shared_from_this<shared_state_with_completion<T, Result>>
    {
        using type = typename shared_state<T>::type;
        using completion_result = Result;
        using completion_function = std::function<completion_result(light_future<type>)>;

        shared_state_with_completion(completion_function f)
        : _function(std::move(f))
        {}
        
        auto call()
        {
            assert(_function);
            assert(this->has_state());
            auto copy_f = std::move(_function);
            _function = nullptr;
            return copy_f(light_future<type>(this->shared_from_this()));
        }

    private:
        completion_function _function;
    };
    
    template<class T>
    struct light_future
    {
        using type = T;
        using store_type = detail::shared_state_storage_type_t<T>;
        
        light_future(std::shared_ptr<shared_state<type>> ptr)
        : _shared_state(std::move(ptr))
        {}
        
        friend shared_state<store_type>;
        
    public:
        light_future() noexcept = default;
        
        decltype(auto) reference() const
        {
            if (_shared_state)
            {
                return _shared_state->get();
            }
            else {
                throw(std::future_error(std::future_errc::no_state));
            }
        }
        
        decltype(auto) get() const {
            return reference();
        }
        
        bool valid() const {
            return _shared_state and _shared_state->has_state();
        }
        
    private:
        std::shared_ptr<shared_state<type>> _shared_state;
    };

    template<class Outer, class T>
    struct light_promise_impl
    {
        void set_value(T t) {
            static_cast<Outer*>(this)->handle_set(std::move(t));
        }
    };
    
    template<class Outer>
    struct light_promise_impl<Outer, void>
    {
        void set_value() {
            static_cast<Outer*>(this)->handle_set(detail::void_state());
        }
    };
    
    template<class T, class Result>
    struct light_promise
    : light_promise_impl<light_promise<T, Result>, T>
    {
        friend light_promise_impl<light_promise<T, Result>, T>;
        using type = T;
        using store_type = detail::shared_state_storage_type_t<type>;
        using completion_arg_type = light_future<type>;
        using completion_result_type = Result;
        using shared_state_type = shared_state_with_completion<type, completion_result_type>;

    public:
        
        template<class F,
        std::enable_if_t<std::is_same<Result, std::result_of_t<F(completion_arg_type)>>::value>* = nullptr
        >
        light_promise(F&& func)
        : _shared_state { std::make_shared<shared_state_type>(std::forward<F>(func)) }
        {
            _shared_state->add_promise();
        }
        
        light_promise(const light_promise&) = delete;
        light_promise& operator=(const light_promise&) = delete;

        light_promise(light_promise&& r) noexcept
        : _shared_state(std::move(r._shared_state))
        {
            
        }

        light_promise& operator=(light_promise&& r) noexcept
        {
            auto tmp = std::move(r);
            swap(r);
        }
        
        ~light_promise() noexcept
        {
            if (_shared_state) {
                _shared_state->remove_promise();
            }
        }
        
        auto set_exception(std::exception_ptr ep) {
            auto p = _shared_state.get();
            p->set_exception(ep);
            p->call();
        }
        
        void swap(light_promise& r) noexcept
        {
            using std::swap;
            swap(_shared_state, r._shared_state);
        }
        
        auto get_future() const
        {
            return light_future<type> { _shared_state };
        }
        
    private:
        
        template<class...Args>
        void handle_set(Args&&...args)
        {
            auto p = _shared_state.get();
            p->set_value(std::forward<Args>(args)...);
            p->call();
        }
        
    private:
        std::shared_ptr<shared_state_type> _shared_state;
    };
    
    template<class T, class F>
    auto promise_to_call(F&& f)
    {
        using value_type = T;
        using completion_func_type = std::decay_t<F>;
        using result_type = std::result_of_t<completion_func_type(light_future<value_type>)>;
        return light_promise<value_type, result_type>(std::forward<F>(f));
    }
    
    
}}
