# valuelib
A c++ library in which value semantics are encouraged

#contributors

Richard Hodges mailto://hodges.r@gmail.com from 2016

# Questions

Please email Richard Hodges on hodges.r@gmail.com

# Principles of this library

This library is designed to build upon the fantastic work in the BOOST libraries and the recent welcome improvements in the c++ core language and standard libraries. BOOT has been and remains probably the strongest force driving the developement of c++ technology. We stand in awe.

The core principles of this library is that everything in a program is easier to reason about if it can be treated as a value. This means that when developing a class or service suite, the interface presented to users of the suite should be expressed as a value - an object he/she can simply create.

Often of course, concepts have natural implementations that are naturally accessed through pointers. In this case authors of classes in this library are encouraged to implement not only the service, but also the value-semantic interface.

This includes, but is not limited to, polymorphic concepts and singletons.

# Why singletons?

Because later in the lifecycle of a library it might become apparent that what was though to be a singlton is actually an instance. We believe that should this happen, the impact on user code should be close to zero.


