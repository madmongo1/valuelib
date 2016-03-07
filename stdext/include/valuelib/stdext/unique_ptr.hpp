#pragma once

namespace value { namespace stdext {

	template<class T>
	auto to_unique_ptr(T* p) {
		return std::unique_ptr<T>(p);
	}
    
    template<class T, class D>
    auto to_unique_ptr(T* p, D&& deleter)
    {
        return std::unique_ptr<T, D>(p, std::forward<D>(deleter));
    }
    
    template<class T, class D, class Function>
    decltype(auto) reset_if_null(std::unique_ptr<T, D>& ptr, Function func)
    {
        if (not ptr.get()) {
            ptr.reset(func());
        }
        return ptr;
    }
}}