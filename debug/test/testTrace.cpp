/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <valuelib/debug/trace.hpp>
#include <gtest/gtest.h>
#include <string>

void test(int x)
{
    value::debug::tracer t(std::cerr, __func__, x);
    if (x) {
        test(x - 1);
    }
}

TEST(testTrace, basic)
{
    int a = 5;
    std::string b = "hello";
    value::debug::tracer t(std::cerr, __func__, a, b);
    test(20);
}