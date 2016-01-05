#pragma once

#include <utility>

namespace value { namespace pointers {
    
    template<class Candidate>
    struct pointer_check
    {
        template<class Ptr> static auto test(Ptr&& ptr) -> decltype((*ptr), void(), std::true_type());
        template<class Ptr> static auto test(...) -> decltype(std::false_type());
        constexpr static bool value = decltype(test<Candidate>(std::declval<Candidate>()))::value;
    };
    
    template<class T> constexpr bool CanBeDereferenced = pointer_check<T>::value;
    
    template<class Pointer, std::enable_if_t<CanBeDereferenced<Pointer>>* = nullptr>
    auto& dereference(Pointer& p) {
        return *p;
    }
    
    template<class Pointer, std::enable_if_t<CanBeDereferenced<Pointer>>* = nullptr>
    auto& dereference(const Pointer& p) {
        return *p;
    }
    
    template<class Pointer, std::enable_if_t<CanBeDereferenced<Pointer>>* = nullptr>
    struct value_semantic_ptr
    {
        using underlying_pointer_type = Pointer;
        using dereference_type = decltype(dereference(std::declval<underlying_pointer_type>()));
        using const_dereference_type = decltype(dereference(std::declval<const underlying_pointer_type>()));
        
        template<class Ptr,
        std::enable_if_t
        <
        std::is_convertible<Ptr, underlying_pointer_type>::value
        and
        not std::is_base_of<value_semantic_ptr, Ptr>::value
        >
        * = nullptr>
        value_semantic_ptr(Ptr&& ptr)
        : _ptr(std::forward<Ptr>(ptr))
        {}
        
        operator dereference_type&() {
            return dereference(_ptr);
        }
        
        operator const_dereference_type&() const {
            return dereference(_ptr);
        }
        
    private:
        underlying_pointer_type _ptr;
    };
    
    template<class Pointer>
    auto dereference(value_semantic_ptr<Pointer>& vs)
    {
        return static_cast<typename value_semantic_ptr<Pointer>::dereference_type&>(vs);
    }
    
    template<class Pointer>
    auto dereference(const value_semantic_ptr<Pointer>& vs)
    {
        return static_cast<typename value_semantic_ptr<Pointer>::const_dereference_type&>(vs);
    }
    
    template<class Pointer, class Right>
    bool operator==(const value_semantic_ptr<Pointer>& left, const Right& right) {
        return dereference(left) == right;
    }
    
    template<class Pointer, class Left>
    bool operator==(const Left& left, const value_semantic_ptr<Pointer>& right) {
        return left == dereference(right);
    }
    
    template<class Left, class Right>
    bool operator==(const value_semantic_ptr<Left>& left, const value_semantic_ptr<Right>& right) {
        return dereference(left) == dereference(right);
    }
    
    template<class Pointer> using StripPointer = std::remove_reference_t<std::remove_cv_t<Pointer>>;
    
    template<class Pointer>
    auto value_semantics(Pointer&& p)
    {
        return value_semantic_ptr<StripPointer<Pointer>>(std::forward<Pointer>(p));
    }
    
}}