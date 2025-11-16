#include <cstdarg>
#include <cstdio> // printing
#include <cstdlib>
#include <ctime>
#include <vector>

inline void qassert(bool condition, size_t line, const char *message)
{
    if (condition)
        return;

    printf("Error in line %zu: %s\n", line, message);
    exit(1);
}

inline void default_assertion_handler(const char *assertion_as_cstring, const char *file, int line, const char *format_string, ...)
{
    const char message_string[] = "Assertion Failed in File: %s::%d\n"
                                  "Assertion: %s\n"
                                  "Message:\n";
    va_list argument_list;
    va_start(argument_list, format_string);
    // warnings dont matter, though we would usually want static checking for our format strings @todo
    fprintf(stdout, message_string, file, line, assertion_as_cstring);
    vfprintf(stdout, format_string, argument_list);
    va_end(argument_list);
    putchar('\n'); // the format string will not contain a newline
    exit(1);
}
#define verify(condition) assert(condition, "Verify Failed")
#undef assert
#define assert(cond, message, ...) \
    ((cond) ? (void)0 : default_assertion_handler(#cond, __FILE__, __LINE__, message __VA_OPT__(, ) __VA_ARGS__))

inline int nanoseconds(void)
{
    struct timespec p[1];
    clock_gettime(CLOCK_MONOTONIC, p);
    return p->tv_nsec % 1000;
}

// NON INLINE DECLARATIONS
// @TODO with how small most of these functions are we could just make them all inline functions
float ran0(long *idum);
void printVec_f(const std::vector<float> *vec, const char *vecName);
double roundTo_d(double x, size_t digits);
double randRange_d(double lower, double upper, long seed = 0);
