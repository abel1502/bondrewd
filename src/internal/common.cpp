#include <bondrewd/internal/common.hpp>

#include <cstdio>
#include <cstdarg>
#include <cerrno>


namespace bondrewd::util {


int log_verbosity = 0;

void _dbg(bool isError, int level, const char* func_name, int line_no, const char *msg, ...) {
    va_list args = {};
    va_start(args, msg);

    if (log_verbosity >= level) {
        fprintf(stderr, "[%s in %s() on #%d] ", isError ? "ERROR" : "DBG", func_name, line_no);
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wformat-nonliteral"
        vfprintf(stderr, msg, args);
        #pragma GCC diagnostic pop
        fprintf(stderr, "\n");
        if (errno != 0 && isError) {
            perror("System error");
        }
    }

    va_end(args);
}


}  // namespace bondrewd
