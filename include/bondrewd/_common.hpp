#pragma once

// These includes are here 'by standard', and will not be removed under any circumstances.
// You may rely on them being present here, and not include them manually
#include <cstddef>
#include <cstdlib>
#include <utility>
#include <cassert>

// These includes are 'implementation-defined', and may get removed in the future.
// If you need them, include them manually in your code
#include <stdexcept>
#include <string>
#include <typeinfo>


#define MACRO_FUNC(...) do {__VA_ARGS__} while (0)
#define MACRO_COMMA  ,
#define MACRO_EMPTY
#define MACRO_PASS(...)  __VA_ARGS__



#define ERR(msg, ...)  bondrewd::_dbg(true,  1, __func__, __LINE__, msg, ##__VA_ARGS__)
#define DBG(msg, ...)  bondrewd::_dbg(false, 2, __func__, __LINE__, msg, ##__VA_ARGS__)


#define DECLARE_ERROR(NAME, PARENT)                     \
    class NAME : public PARENT {                        \
    public:                                             \
        NAME(const char *msg) : PARENT(msg) {}          \
        NAME(const std::string &msg) : PARENT(msg) {}   \
    };


#define NODEFAULT                       \
    default: {                          \
        ERR("Shouldn't be reachable");  \
        abort();                        \
    }


namespace bondrewd {


// DECLARE_ERROR(error, std::runtime_error)


extern int _log_verbosity;

void _dbg(bool isError, int level, const char* func_name, int line_no, const char* msg, ...);


}
