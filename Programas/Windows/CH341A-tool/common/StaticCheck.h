#ifndef StaticCheckH
#define StaticCheckH

template<int> struct CompileTimeError;
template<> struct CompileTimeError<true> {};
#define STATIC_CHECK(expr, msg) { CompileTimeError<((expr) != 0)> ERROR_##msg; (void)ERROR_##msg; } 

#endif