#include "test_utils.hpp"
#include <iostream>

void emit(const char* str, std::size_t size) noexcept
{
    try {
	std::cout.write(str, size);
    }
    catch(...)
    {}
}
