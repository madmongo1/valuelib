/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   exception.hpp
 * Author: Richard Hodges <hodges.r@gmail.com>
 *
 * Created on 16 May 2016, 14:31
 */

#pragma once

#include <typeindex>
#include <typeinfo>
#include <unordered_map>

#include <valuelib/debug/unwrap.hpp>

namespace value { namespace stdext {
    
    template<class...Args>
    std::string collate(Args&&...args)
    {
        std::ostringstream ss;
        using expand = int[];
        void(expand{ 0,
            ((ss << args), 0)...
        });
        return ss.str();
    }
    
    template<class SourceType>
    struct re_raiser
    {
        template<class Message>
        [[noreturn]]
        bool operator()(Message&& message, const std::exception& source)
        {
            //        assert(typeid(source) == typeid(SourceType));
            std::throw_with_nested(SourceType(std::forward<Message>(message)));
        }
    };
    
    template<>
    struct re_raiser<std::system_error>
    {
        template<class Message>
        [[noreturn]]
        bool operator()(Message&& message, const std::exception& source)
        {
            //        assert(typeid(source) == typeid(std::system_error));
            auto& original = static_cast<const std::system_error&>(source);
            std::throw_with_nested(std::system_error(original.code(),
                                                     std::forward<Message>(message)));
        }
    };
    
    struct exception_register
    {
        using wrapper_sig = void(std::string, const std::exception&);
        using wrapper_function = std::function<wrapper_sig>;
        
        struct actions
        {
            wrapper_function wrapper;
        };
        
        struct impl
        {
            template<class E>
            auto type_when_nesting(E model)
            {
                try {
                    throw std::runtime_error("test");
                }
                catch(std::exception&) {
                    try {
                        std::throw_with_nested(model);
                    }
                    catch(E& e) {
                        auto ti = std::type_index(typeid(e));
                        return ti;
                    }
                }
            }
            
            template<class F>
            bool register_exact_type(std::type_index ti, F& function)
            {
                auto ifind = _map.find(ti);
                if (ifind == _map.end())
                {
                    _map.emplace(ti, function());
                    return true;
                }
                else {
                    return false;
                }
            }

            template<class E, class F>
            void register_exception(const E& e, F&& function)
            {
                using base_type = std::decay_t<E>;

                auto lock = std::unique_lock<std::mutex>(_mutex);
                if(register_exact_type(typeid(base_type), function))
                {
                    const E base_c = e;
                    register_exact_type(typeid(base_c), function);
                    register_exact_type(type_when_nesting<base_type>(e), function);
                }
            }
            
            template<class Message>
            [[noreturn]]
            void wrap(Message&& message, const std::exception& e) const
            {
                auto ti = std::type_index(typeid(e));
                auto lock = std::unique_lock<std::mutex>(_mutex);
                auto ifind = _map.find(ti);
                if (ifind == _map.end())
                {
                    lock.unlock();
                    default_wrap(std::forward<Message>(message), e);
                }
                else {
                    ifind->second.wrapper(std::forward<Message>(message),
                                          e);
                }
                lock.unlock();
                std::throw_with_nested(std::logic_error(collate(__func__,
                                                                " failed to wrap ",
                                                                value::debug::unwrap(e,
                                                                                     value::debug::csv_formatter()))));
            }
            
            template<class Message>
            [[noreturn]]
            void default_wrap(Message&& message, const std::exception& e) const
            {
                try {
                    throw;
                }
                catch(const std::invalid_argument& e)
                {
                    std::throw_with_nested(std::invalid_argument(std::forward<Message>(message)));
                }
                catch(const std::logic_error& e)
                {
                    std::throw_with_nested(std::logic_error(std::forward<Message>(message)));
                }
                catch(const std::system_error& e)
                {
                    std::throw_with_nested(std::system_error(e.code(), std::forward<Message>(message)));
                }
                catch(const std::runtime_error& e)
                {
                    std::throw_with_nested(std::runtime_error(std::forward<Message>(message)));
                }
                catch(const std::exception& e)
                {
                    std::throw_with_nested(std::runtime_error(std::forward<Message>(message)));
                }
            }
            
            
        private:
            mutable std::mutex _mutex;
            std::unordered_map<std::type_index, actions> _map;
        };
        
        template<class Exception>
        [[noreturn]]
        bool announce(Exception e) {
            using type = std::decay_t<Exception>;
            auto f = [&e] {
                return actions {
                    [](std::string message, const std::exception& e)
                    {
                        re_raiser<type>()(std::move(message), e);
                    }
                };
            };
            require_impl().register_exception(e, f);
            throw std::decay_t<Exception>(e);
        }
        
        template<class Exception>
        [[noreturn]]
        bool announce_with_nested(Exception e) {
            using type = std::decay_t<Exception>;
            auto f = [&e] {
                return actions {
                    [](std::string message, const std::exception& e)
                    {
                        re_raiser<type>()(std::move(message), e);
                    }
                };
            };
            require_impl().register_exception(e, f);
            std::throw_with_nested(std::decay_t<Exception>(e));
        }
        
        template<class Message>
        [[noreturn]]
        void wrap(Message&& message, std::exception_ptr ep)
        {
            try {
                std::rethrow_exception(ep);
            }
            catch(const std::exception& e)
            {
                require_impl().wrap(std::forward<Message>(message), e);
            }
        }
        
    private:
        static impl& require_impl() {
            static impl _ {};
            return _;
        }
    };
    
    template<class Exception>
    [[noreturn]]
    bool raise(Exception&& e)
    {
        exception_register _register;
        _register.announce(std::move(e));
    }
    
    template<class Message, std::enable_if_t<!std::is_base_of<std::exception, Message>::value>* = nullptr>
    [[noreturn]]
    void re_raise(Message&& message)
    {
        exception_register _register;
        _register.wrap(std::forward<Message>(message), std::current_exception());
    }
    
    template<class Exception, std::enable_if_t<std::is_base_of<std::exception, Exception>::value>* = nullptr>
    [[noreturn]]
    void raise_with_nested(Exception&& e)
    {
        exception_register _register;
        _register.announce_with_nested(std::forward<Exception>(e));
    }
    
}}
