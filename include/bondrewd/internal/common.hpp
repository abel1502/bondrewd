#pragma once

/// These includes are here 'by standard', and will not be removed under any circumstances.
/// You may rely on them being present here, and not include them manually
#include <cstddef>
#include <cstdlib>
#include <utility>
#include <cassert>

/// These includes are 'implementation-defined', and may get removed in the future.
/// If you need them, include them manually in your code
#include <stdexcept>
#include <string>
#include <typeinfo>


/// These are some common macros that are useful with other macros
#define MACRO_FUNC(...) do {__VA_ARGS__} while (0)
#define MACRO_COMMA  ,
#define MACRO_EMPTY
#define MACRO_PASS(...)  __VA_ARGS__


/// CURRENTLY SUSPENDED
/// These provide a mechanism for defining a class with all methods in a header, but
/// still having them compiled in a single translation unit.
/// 
/// To use them:
///  - put `JUST_DECLARE(class, MyClass)` instead of `class MyClass`
///  - put `PROMISE_DEFINITION(class, MyClass)` somewhere in your header after 
///    the class declaration.
///  - put `DEFINE_DECLARED(class, MyClass)` in a translation unit. It will house
///    the actual definition of the class.
// #define JUST_DECLARE(TYPE, CLS) \
//     template <typename = void> TYPE CLS##_; \
//     using CLS = CLS##_<>; \
//     template <typename _> TYPE CLS##_
// #define PROMISE_DEFINITION(TYPE, CLS)  extern DEFINE_DECLARED(TYPE, CLS);
// #define DEFINE_DECLARED(TYPE, CLS)  template TYPE CLS##_<>;


/// These are some logging macros
#define ERR(msg, ...)  bondrewd::util::_dbg(true,  1, __func__, __LINE__, msg, ##__VA_ARGS__)
#define DBG(msg, ...)  bondrewd::util::_dbg(false, 2, __func__, __LINE__, msg, ##__VA_ARGS__)


/// This is a macro to facilitate exception declaraion
#define DECLARE_ERROR(NAME, PARENT)                     \
    class NAME : public PARENT {                        \
    public:                                             \
        NAME(const char *msg) : PARENT(msg) {}          \
        NAME(const std::string &msg) : PARENT(msg) {}   \
    };


/// This macro is used to ensure that a switch statement is exhaustive
#define NODEFAULT                       \
    default: {                          \
        ERR("Shouldn't be reachable");  \
        abort();                        \
    }


namespace bondrewd::util {


// DECLARE_ERROR(error, std::runtime_error)


extern int log_verbosity;

void _dbg(bool isError, int level, const char* func_name, int line_no, const char* msg, ...);


template <typename, template<typename ...> typename>
constexpr bool _is_specialization = false;

template <template<typename ...> typename T, typename ... Args>
constexpr bool _is_specialization<T<Args...>, T> = true;


template <typename T, template<typename ...> typename Tpl>
concept specialization_of = _is_specialization<T, Tpl>;


}  // namespace bondrewd
