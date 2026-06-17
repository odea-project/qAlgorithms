#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-format-attribute"
#pragma clang diagnostic ignored "-Wformat-nonliteral"

#include <cstdarg>
#include <cstdio> // printing
#include <cstdlib>
#include <ctime>
#include <qalgorithms_qpeaks.h>
#include <vector>
#define _USE_MATH_DEFINES
#include <cfloat> // placing this here instead of in every individual test
#include <math.h>

#ifndef _GLIBCXX_CFLOAT
    #error "this is just here to suppress a warning"
#endif

using namespace qAlgorithms;

bool flt_equal(double a, double b, double tol);

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
    /* set breakpoint here! */
    volatile int a = 3;
#if defined(__i386__) || defined(__x86_64__)
    __asm__ volatile("int $3");
#elif defined(__arm__) || defined(__aarch64__)
    __asm__ volatile("brk #0");
#else
    #error "Unsupported architecture for breakpoint trap"
#endif
    a++;
    printf("%d\n", a);
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
// @@todo with how small most of these functions are we could just make them all inline functions
float ran0(long *idum);
void printVec_f(const std::vector<float> *vec, const char *vecName);
double roundTo_d(double x, size_t digits);
double randRange_d(double lower, double upper, long seed = 0);

RegCoeffs getCoeffs(double height, double position, double sd_left, double sd_right);

double gauss_rand(const double mean, const double sdev);

double fwhm_empiric(const std::vector<float> *x, const std::vector<float> *y);
double area_empiric(const std::vector<float> *x, const std::vector<float> *y);
double position_empiric(const std::vector<float> *x, const std::vector<float> *y);

void print_regFit(const RegCoeffs *coeff, const std::vector<float> *x, const float delta_x);