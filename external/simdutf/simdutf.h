/* auto-generated on 2026-04-21 17:01:59 -0400. Do not edit! */
/* begin file include/simdutf.h */
#ifndef SIMDUTF_H
#define SIMDUTF_H
#include <cstring>

/* begin file include/simdutf/compiler_check.h */
#ifndef SIMDUTF_COMPILER_CHECK_H
#define SIMDUTF_COMPILER_CHECK_H

#ifndef __cplusplus
  #error simdutf requires a C++ compiler
#endif

#ifndef SIMDUTF_CPLUSPLUS
  #if defined(_MSVC_LANG) && !defined(__clang__)
    #define SIMDUTF_CPLUSPLUS (_MSC_VER == 1900 ? 201103L : _MSVC_LANG)
  #else
    #define SIMDUTF_CPLUSPLUS __cplusplus
  #endif
#endif

// C++ 26
#if !defined(SIMDUTF_CPLUSPLUS26) && (SIMDUTF_CPLUSPLUS >= 202602L)
  #define SIMDUTF_CPLUSPLUS26 1
#endif

// C++ 23
#if !defined(SIMDUTF_CPLUSPLUS23) && (SIMDUTF_CPLUSPLUS >= 202302L)
  #define SIMDUTF_CPLUSPLUS23 1
#endif

// C++ 20
#if !defined(SIMDUTF_CPLUSPLUS20) && (SIMDUTF_CPLUSPLUS >= 202002L)
  #define SIMDUTF_CPLUSPLUS20 1
#endif

// C++ 17
#if !defined(SIMDUTF_CPLUSPLUS17) && (SIMDUTF_CPLUSPLUS >= 201703L)
  #define SIMDUTF_CPLUSPLUS17 1
#endif

// C++ 14
#if !defined(SIMDUTF_CPLUSPLUS14) && (SIMDUTF_CPLUSPLUS >= 201402L)
  #define SIMDUTF_CPLUSPLUS14 1
#endif

// C++ 11
#if !defined(SIMDUTF_CPLUSPLUS11) && (SIMDUTF_CPLUSPLUS >= 201103L)
  #define SIMDUTF_CPLUSPLUS11 1
#endif

#ifndef SIMDUTF_CPLUSPLUS17
  #error simdutf requires a compiler compliant with the C++17 standard
#endif

#endif // SIMDUTF_COMPILER_CHECK_H
/* end file include/simdutf/compiler_check.h */
/* begin file include/simdutf/common_defs.h */
#ifndef SIMDUTF_COMMON_DEFS_H
#define SIMDUTF_COMMON_DEFS_H

/* begin file include/simdutf/portability.h */
#ifndef SIMDUTF_PORTABILITY_H
#define SIMDUTF_PORTABILITY_H


#include <cfloat>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#ifndef _WIN32
  // strcasecmp, strncasecmp
  #include <strings.h>
#endif

#if defined(__apple_build_version__)
  #if __apple_build_version__ < 14000000
    #define SIMDUTF_SPAN_DISABLED                                              \
      1 // apple-clang/13 doesn't support std::convertible_to
  #endif
#endif

#if SIMDUTF_CPLUSPLUS20
  #include <version>
  #if __cpp_concepts >= 201907L && __cpp_lib_span >= 202002L &&                \
      !defined(SIMDUTF_SPAN_DISABLED)
    #define SIMDUTF_SPAN 1
  #endif // __cpp_concepts >= 201907L && __cpp_lib_span >= 202002L
  #if __cpp_lib_atomic_ref >= 201806L
    #define SIMDUTF_ATOMIC_REF 1
  #endif // __cpp_lib_atomic_ref
  #if __has_cpp_attribute(maybe_unused) >= 201603L
    #define SIMDUTF_MAYBE_UNUSED_AVAILABLE 1
  #endif // __has_cpp_attribute(maybe_unused) >= 201603L
#endif

/**
 * We want to check that it is actually a little endian system at
 * compile-time.
 */

#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__)
  #define SIMDUTF_IS_BIG_ENDIAN (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#elif defined(_WIN32)
  #define SIMDUTF_IS_BIG_ENDIAN 0
#else
  #if defined(__APPLE__) ||                                                    \
      defined(__FreeBSD__) // defined __BYTE_ORDER__ && defined
                           // __ORDER_BIG_ENDIAN__
    #include <machine/endian.h>
  #elif defined(sun) ||                                                        \
      defined(__sun) // defined(__APPLE__) || defined(__FreeBSD__)
    #include <sys/byteorder.h>
  #else // defined(__APPLE__) || defined(__FreeBSD__)

    #ifdef __has_include
      #if __has_include(<endian.h>)
        #include <endian.h>
      #endif //__has_include(<endian.h>)
    #endif   //__has_include

  #endif // defined(__APPLE__) || defined(__FreeBSD__)

  #ifndef !defined(__BYTE_ORDER__) || !defined(__ORDER_LITTLE_ENDIAN__)
    #define SIMDUTF_IS_BIG_ENDIAN 0
  #endif

  #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #define SIMDUTF_IS_BIG_ENDIAN 0
  #else // __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #define SIMDUTF_IS_BIG_ENDIAN 1
  #endif // __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

#endif // defined __BYTE_ORDER__ && defined __ORDER_BIG_ENDIAN__

/**
 * At this point in time, SIMDUTF_IS_BIG_ENDIAN is defined.
 */

#ifdef _MSC_VER
  #define SIMDUTF_VISUAL_STUDIO 1
  /**
   * We want to differentiate carefully between
   * clang under visual studio and regular visual
   * studio.
   *
   * Under clang for Windows, we enable:
   *  * target pragmas so that part and only part of the
   *     code gets compiled for advanced instructions.
   *
   */
  #ifdef __clang__
    // clang under visual studio
    #define SIMDUTF_CLANG_VISUAL_STUDIO 1
  #else
    // just regular visual studio (best guess)
    #define SIMDUTF_REGULAR_VISUAL_STUDIO 1
  #endif // __clang__
#endif   // _MSC_VER

#ifdef SIMDUTF_REGULAR_VISUAL_STUDIO
  // https://en.wikipedia.org/wiki/C_alternative_tokens
  // This header should have no effect, except maybe
  // under Visual Studio.
  #include <iso646.h>
#endif

#if (defined(__x86_64__) || defined(_M_AMD64)) && !defined(_M_ARM64EC)
  #define SIMDUTF_IS_X86_64 1
#elif defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
  #define SIMDUTF_IS_ARM64 1
#elif defined(__PPC64__) || defined(_M_PPC64)
  #if defined(__VEC__) && defined(__ALTIVEC__)
    #define SIMDUTF_IS_PPC64 1
  #endif
#elif defined(__s390__)
// s390 IBM system. Big endian.
#elif (defined(__riscv) || defined(__riscv__)) && __riscv_xlen == 64
  // RISC-V 64-bit
  #define SIMDUTF_IS_RISCV64 1

  // #if __riscv_v_intrinsic >= 1000000
  //   #define SIMDUTF_HAS_RVV_INTRINSICS 1
  //   #define SIMDUTF_HAS_RVV_TARGET_REGION 1
  // #elif ...
  //  Check for special compiler versions that implement pre v1.0 intrinsics
  #if __riscv_v_intrinsic >= 11000
    #define SIMDUTF_HAS_RVV_INTRINSICS 1
  #endif

  #define SIMDUTF_HAS_ZVBB_INTRINSICS                                          \
    0 // there is currently no way to detect this

  #if SIMDUTF_HAS_RVV_INTRINSICS && __riscv_vector &&                          \
      __riscv_v_min_vlen >= 128 && __riscv_v_elen >= 64
    // RISC-V V extension
    #define SIMDUTF_IS_RVV 1
    #if SIMDUTF_HAS_ZVBB_INTRINSICS && __riscv_zvbb >= 1000000
      // RISC-V Vector Basic Bit-manipulation
      #define SIMDUTF_IS_ZVBB 1
    #endif
  #endif

#elif defined(__loongarch_lp64)
  #if defined(__loongarch_sx) && defined(__loongarch_asx)
    #define SIMDUTF_IS_LSX 1
    #define SIMDUTF_IS_LASX 1 // We can always run both
  #elif defined(__loongarch_sx)
    #define SIMDUTF_IS_LSX 1
    // Adjust for runtime dispatching support.
    #if defined(__GNUC__) && !defined(__clang__) &&                            \
        !defined(__INTEL_COMPILER) && !defined(__NVCOMPILER)
      #if __GNUC__ > 15 || (__GNUC__ == 15 && __GNUC_MINOR__ >= 0)
        // We are ok, we will support runtime dispatch for LASX.
      #else
        // We disable runtime dispatch for LASX, which means that we will not be
        // able to use LASX even if it is supported by the hardware. Loongson
        // users should update to GCC 15 or better.
        #define SIMDUTF_IMPLEMENTATION_LASX 0
      #endif
    #else
      // We are not using GCC, so we assume that we can support runtime dispatch
      // for LASX. https://godbolt.org/z/jcMnrjYhs
      #define SIMDUTF_IMPLEMENTATION_LASX 0
    #endif
  #endif
#else
  // The simdutf library is designed
  // for 64-bit processors and it seems that you are not
  // compiling for a known 64-bit platform. Please
  // use a 64-bit target such as x64 or 64-bit ARM for best performance.
  #define SIMDUTF_IS_32BITS 1

  // We do not support 32-bit platforms, but it can be
  // handy to identify them.
  #if defined(_M_IX86) || defined(__i386__)
    #define SIMDUTF_IS_X86_32BITS 1
  #elif defined(__arm__) || defined(_M_ARM)
    #define SIMDUTF_IS_ARM_32BITS 1
  #elif defined(__PPC__) || defined(_M_PPC)
    #define SIMDUTF_IS_PPC_32BITS 1
  #endif

#endif // defined(__x86_64__) || defined(_M_AMD64)

#ifdef SIMDUTF_IS_32BITS
  #ifndef SIMDUTF_NO_PORTABILITY_WARNING
  // In the future, we may want to warn users of 32-bit systems that
  // the simdutf does not support accelerated kernels for such systems.
  #endif // SIMDUTF_NO_PORTABILITY_WARNING
#endif   // SIMDUTF_IS_32BITS

// this is almost standard?
#define SIMDUTF_STRINGIFY_IMPLEMENTATION_(a) #a
#define SIMDUTF_STRINGIFY(a) SIMDUTF_STRINGIFY_IMPLEMENTATION_(a)

// Our fast kernels require 64-bit systems.
//
// On 32-bit x86, we lack 64-bit popcnt, lzcnt, blsr instructions.
// Furthermore, the number of SIMD registers is reduced.
//
// On 32-bit ARM, we would have smaller registers.
//
// The simdutf users should still have the fallback kernel. It is
// slower, but it should run everywhere.

//
// Enable valid runtime implementations, and select
// SIMDUTF_BUILTIN_IMPLEMENTATION
//

// We are going to use runtime dispatch.
#if defined(SIMDUTF_IS_X86_64) || defined(SIMDUTF_IS_LSX)
  #ifdef __clang__
    // clang does not have GCC push pop
    // warning: clang attribute push can't be used within a namespace in clang
    // up til 8.0 so SIMDUTF_TARGET_REGION and SIMDUTF_UNTARGET_REGION must be
    // *outside* of a namespace.
    #define SIMDUTF_TARGET_REGION(T)                                           \
      _Pragma(SIMDUTF_STRINGIFY(clang attribute push(                          \
          __attribute__((target(T))), apply_to = function)))
    #define SIMDUTF_UNTARGET_REGION _Pragma("clang attribute pop")
  #elif defined(__GNUC__)
    // GCC is easier
    #define SIMDUTF_TARGET_REGION(T)                                           \
      _Pragma("GCC push_options") _Pragma(SIMDUTF_STRINGIFY(GCC target(T)))
    #define SIMDUTF_UNTARGET_REGION _Pragma("GCC pop_options")
  #endif // clang then gcc

#endif // defined(SIMDUTF_IS_X86_64) || defined(SIMDUTF_IS_LSX)

// Default target region macros don't do anything.
#ifndef SIMDUTF_TARGET_REGION
  #define SIMDUTF_TARGET_REGION(T)
  #define SIMDUTF_UNTARGET_REGION
#endif

// Is threading enabled?
#if defined(_REENTRANT) || defined(_MT)
  #ifndef SIMDUTF_THREADS_ENABLED
    #define SIMDUTF_THREADS_ENABLED
  #endif
#endif

// workaround for large stack sizes under -O0.
// https://github.com/simdutf/simdutf/issues/691
#ifdef __APPLE__
  #ifndef __OPTIMIZE__
    // Apple systems have small stack sizes in secondary threads.
    // Lack of compiler optimization may generate high stack usage.
    // Users may want to disable threads for safety, but only when
    // in debug mode which we detect by the fact that the __OPTIMIZE__
    // macro is not defined.
    #undef SIMDUTF_THREADS_ENABLED
  #endif
#endif

#ifdef SIMDUTF_VISUAL_STUDIO
  // This is one case where we do not distinguish between
  // regular visual studio and clang under visual studio.
  // clang under Windows has _stricmp (like visual studio) but not strcasecmp
  // (as clang normally has)
  #define simdutf_strcasecmp _stricmp
  #define simdutf_strncasecmp _strnicmp
#else
  // The strcasecmp, strncasecmp, and strcasestr functions do not work with
  // multibyte strings (e.g. UTF-8). So they are only useful for ASCII in our
  // context.
  // https://www.gnu.org/software/libunistring/manual/libunistring.html#char-_002a-strings
  #define simdutf_strcasecmp strcasecmp
  #define simdutf_strncasecmp strncasecmp
#endif

#if defined(__GNUC__) && !defined(__clang__)
  #if __GNUC__ >= 11
    #define SIMDUTF_GCC11ORMORE 1
  #endif //  __GNUC__ >= 11
  #if __GNUC__ == 10
    #define SIMDUTF_GCC10 1
  #endif //  __GNUC__ == 10
  #if __GNUC__ < 10
    #define SIMDUTF_GCC9OROLDER 1
  #endif //  __GNUC__ == 10
#endif   // defined(__GNUC__) && !defined(__clang__)

#endif // SIMDUTF_PORTABILITY_H
/* end file include/simdutf/portability.h */
/* begin file include/simdutf/avx512.h */
#ifndef SIMDUTF_AVX512_H_
#define SIMDUTF_AVX512_H_

/*
    It's possible to override AVX512 settings with cmake DCMAKE_CXX_FLAGS.

    All preprocessor directives has form `SIMDUTF_HAS_AVX512{feature}`,
    where a feature is a code name for extensions.

    Please see the listing below to find which are supported.
*/

#ifndef SIMDUTF_HAS_AVX512F
  #if defined(__AVX512F__) && __AVX512F__ == 1
    #define SIMDUTF_HAS_AVX512F 1
  #endif
#endif

#ifndef SIMDUTF_HAS_AVX512DQ
  #if defined(__AVX512DQ__) && __AVX512DQ__ == 1
    #define SIMDUTF_HAS_AVX512DQ 1
  #endif
#endif

#ifndef SIMDUTF_HAS_AVX512IFMA
  #if defined(__AVX512IFMA__) && __AVX512IFMA__ == 1
    #define SIMDUTF_HAS_AVX512IFMA 1
  #endif
#endif

#ifndef SIMDUTF_HAS_AVX512CD
  #if defined(__AVX512CD__) && __AVX512CD__ == 1
    #define SIMDUTF_HAS_AVX512CD 1
  #endif
#endif

#ifndef SIMDUTF_HAS_AVX512BW
  #if defined(__AVX512BW__) && __AVX512BW__ == 1
    #define SIMDUTF_HAS_AVX512BW 1
  #endif
#endif

#ifndef SIMDUTF_HAS_AVX512VL
  #if defined(__AVX512VL__) && __AVX512VL__ == 1
    #define SIMDUTF_HAS_AVX512VL 1
  #endif
#endif

#ifndef SIMDUTF_HAS_AVX512VBMI
  #if defined(__AVX512VBMI__) && __AVX512VBMI__ == 1
    #define SIMDUTF_HAS_AVX512VBMI 1
  #endif
#endif

#ifndef SIMDUTF_HAS_AVX512VBMI2
  #if defined(__AVX512VBMI2__) && __AVX512VBMI2__ == 1
    #define SIMDUTF_HAS_AVX512VBMI2 1
  #endif
#endif

#ifndef SIMDUTF_HAS_AVX512VNNI
  #if defined(__AVX512VNNI__) && __AVX512VNNI__ == 1
    #define SIMDUTF_HAS_AVX512VNNI 1
  #endif
#endif

#ifndef SIMDUTF_HAS_AVX512BITALG
  #if defined(__AVX512BITALG__) && __AVX512BITALG__ == 1
    #define SIMDUTF_HAS_AVX512BITALG 1
  #endif
#endif

#ifndef SIMDUTF_HAS_AVX512VPOPCNTDQ
  #if defined(__AVX512VPOPCNTDQ__) && __AVX512VPOPCNTDQ__ == 1
    #define SIMDUTF_HAS_AVX512VPOPCNTDQ 1
  #endif
#endif

#endif // SIMDUTF_AVX512_H_
/* end file include/simdutf/avx512.h */

// Sometimes logging is useful, but we want it disabled by default
// and free of any logging code in release builds.
#ifdef SIMDUTF_LOGGING
  #include <iostream>
  #define simdutf_log(msg)                                                     \
    std::cout << "[" << __FUNCTION__ << "]: " << msg << std::endl              \
              << "\t" << __FILE__ << ":" << __LINE__ << std::endl;
  #define simdutf_log_assert(cond, msg)                                        \
    do {                                                                       \
      if (!(cond)) {                                                           \
        std::cerr << "[" << __FUNCTION__ << "]: " << msg << std::endl          \
                  << "\t" << __FILE__ << ":" << __LINE__ << std::endl;         \
        std::abort();                                                          \
      }                                                                        \
    } while (0)
#else
  #define simdutf_log(msg)
  #define simdutf_log_assert(cond, msg)
#endif

#if defined(SIMDUTF_REGULAR_VISUAL_STUDIO)
  #define SIMDUTF_DEPRECATED __declspec(deprecated)

  #define simdutf_really_inline __forceinline // really inline in release mode
  #define simdutf_always_inline __forceinline // always inline, no matter what
  #define simdutf_never_inline __declspec(noinline)

  #define simdutf_unused
  #define simdutf_warn_unused

  #ifndef simdutf_likely
    #define simdutf_likely(x) x
  #endif
  #ifndef simdutf_unlikely
    #define simdutf_unlikely(x) x
  #endif

  #define SIMDUTF_PUSH_DISABLE_WARNINGS __pragma(warning(push))
  #define SIMDUTF_PUSH_DISABLE_ALL_WARNINGS __pragma(warning(push, 0))
  #define SIMDUTF_DISABLE_VS_WARNING(WARNING_NUMBER)                           \
    __pragma(warning(disable : WARNING_NUMBER))
  // Get rid of Intellisense-only warnings (Code Analysis)
  // Though __has_include is C++17, it is supported in Visual Studio 2017 or
  // better (_MSC_VER>=1910).
  #ifdef __has_include
    #if __has_include(<CppCoreCheck\Warnings.h>)
      #include <CppCoreCheck\Warnings.h>
      #define SIMDUTF_DISABLE_UNDESIRED_WARNINGS                               \
        SIMDUTF_DISABLE_VS_WARNING(ALL_CPPCORECHECK_WARNINGS)
    #endif
  #endif

  #ifndef SIMDUTF_DISABLE_UNDESIRED_WARNINGS
    #define SIMDUTF_DISABLE_UNDESIRED_WARNINGS
  #endif

  #define SIMDUTF_DISABLE_DEPRECATED_WARNING SIMDUTF_DISABLE_VS_WARNING(4996)
  #define SIMDUTF_DISABLE_STRICT_OVERFLOW_WARNING
  #define SIMDUTF_POP_DISABLE_WARNINGS __pragma(warning(pop))
  #define SIMDUTF_DISABLE_UNUSED_WARNING
#else // SIMDUTF_REGULAR_VISUAL_STUDIO
  #if defined(__OPTIMIZE__) || defined(NDEBUG)
    #define simdutf_really_inline inline __attribute__((always_inline))
  #else
    #define simdutf_really_inline inline
  #endif
  #define simdutf_always_inline                                                \
    inline __attribute__((always_inline)) // always inline, no matter what
  #define SIMDUTF_DEPRECATED __attribute__((deprecated))
  #define simdutf_never_inline inline __attribute__((noinline))

  #define simdutf_unused __attribute__((unused))
  #define simdutf_warn_unused __attribute__((warn_unused_result))

  #ifndef simdutf_likely
    #define simdutf_likely(x) __builtin_expect(!!(x), 1)
  #endif
  #ifndef simdutf_unlikely
    #define simdutf_unlikely(x) __builtin_expect(!!(x), 0)
  #endif
  // clang-format off
  #define SIMDUTF_PUSH_DISABLE_WARNINGS _Pragma("GCC diagnostic push")
  // gcc doesn't seem to disable all warnings with all and extra, add warnings
  // here as necessary
  #define SIMDUTF_PUSH_DISABLE_ALL_WARNINGS                                    \
    SIMDUTF_PUSH_DISABLE_WARNINGS                                              \
    SIMDUTF_DISABLE_GCC_WARNING(-Weffc++)                                      \
    SIMDUTF_DISABLE_GCC_WARNING(-Wall)                                         \
    SIMDUTF_DISABLE_GCC_WARNING(-Wconversion)                                  \
    SIMDUTF_DISABLE_GCC_WARNING(-Wextra)                                       \
    SIMDUTF_DISABLE_GCC_WARNING(-Wattributes)                                  \
    SIMDUTF_DISABLE_GCC_WARNING(-Wimplicit-fallthrough)                        \
    SIMDUTF_DISABLE_GCC_WARNING(-Wnon-virtual-dtor)                            \
    SIMDUTF_DISABLE_GCC_WARNING(-Wreturn-type)                                 \
    SIMDUTF_DISABLE_GCC_WARNING(-Wshadow)                                      \
    SIMDUTF_DISABLE_GCC_WARNING(-Wunused-parameter)                            \
    SIMDUTF_DISABLE_GCC_WARNING(-Wunused-variable)
  #define SIMDUTF_PRAGMA(P) _Pragma(#P)
  #define SIMDUTF_DISABLE_GCC_WARNING(WARNING)                                 \
    SIMDUTF_PRAGMA(GCC diagnostic ignored #WARNING)
  #if defined(SIMDUTF_CLANG_VISUAL_STUDIO)
    #define SIMDUTF_DISABLE_UNDESIRED_WARNINGS                                 \
      SIMDUTF_DISABLE_GCC_WARNING(-Wmicrosoft-include)
  #else
    #define SIMDUTF_DISABLE_UNDESIRED_WARNINGS
  #endif
  #define SIMDUTF_DISABLE_DEPRECATED_WARNING                                   \
    SIMDUTF_DISABLE_GCC_WARNING(-Wdeprecated-declarations)
  #define SIMDUTF_DISABLE_STRICT_OVERFLOW_WARNING                              \
    SIMDUTF_DISABLE_GCC_WARNING(-Wstrict-overflow)
  #define SIMDUTF_POP_DISABLE_WARNINGS _Pragma("GCC diagnostic pop")
  #define SIMDUTF_DISABLE_UNUSED_WARNING                                       \
    SIMDUTF_PUSH_DISABLE_WARNINGS                                              \
    SIMDUTF_DISABLE_GCC_WARNING(-Wunused-function)                             \
    SIMDUTF_DISABLE_GCC_WARNING(-Wunused-const-variable)
  // clang-format on

#endif // MSC_VER

// Will evaluate to constexpr in C++23 or later. This makes it possible to mark
// functions constexpr if the "if consteval" feature is available to use.
#if SIMDUTF_CPLUSPLUS23
  #define simdutf_constexpr23 constexpr
#else
  #define simdutf_constexpr23
#endif

#ifndef SIMDUTF_DLLIMPORTEXPORT
  #if defined(SIMDUTF_VISUAL_STUDIO) // Visual Studio
                                     /**
                                      * Windows users need to do some extra work when building
                                      * or using a dynamic library (DLL). When building, we need
                                      * to set SIMDUTF_DLLIMPORTEXPORT to __declspec(dllexport).
                                      * When *using* the DLL, the user needs to set
                                      * SIMDUTF_DLLIMPORTEXPORT __declspec(dllimport).
                                      *
                                      * Static libraries not need require such work.
                                      *
                                      * It does not matter here whether you are using
                                      * the regular visual studio or clang under visual
                                      * studio, you still need to handle these issues.
                                      *
                                      * Non-Windows systems do not have this complexity.
                                      */
    #if SIMDUTF_BUILDING_WINDOWS_DYNAMIC_LIBRARY

      // We set SIMDUTF_BUILDING_WINDOWS_DYNAMIC_LIBRARY when we build a DLL
      // under Windows. It should never happen that both
      // SIMDUTF_BUILDING_WINDOWS_DYNAMIC_LIBRARY and
      // SIMDUTF_USING_WINDOWS_DYNAMIC_LIBRARY are set.
      #define SIMDUTF_DLLIMPORTEXPORT __declspec(dllexport)
    #elif SIMDUTF_USING_WINDOWS_DYNAMIC_LIBRARY
      // Windows user who call a dynamic library should set
      // SIMDUTF_USING_WINDOWS_DYNAMIC_LIBRARY to 1.

      #define SIMDUTF_DLLIMPORTEXPORT __declspec(dllimport)
    #else
      // We assume by default static linkage
      #define SIMDUTF_DLLIMPORTEXPORT
    #endif
  #else // defined(SIMDUTF_VISUAL_STUDIO)
    // Non-Windows systems do not have this complexity.
    #define SIMDUTF_DLLIMPORTEXPORT
  #endif // defined(SIMDUTF_VISUAL_STUDIO)
#endif

#if SIMDUTF_MAYBE_UNUSED_AVAILABLE
  #define simdutf_maybe_unused [[maybe_unused]]
#else
  #define simdutf_maybe_unused
#endif

#endif // SIMDUTF_COMMON_DEFS_H
/* end file include/simdutf/common_defs.h */
/* begin file include/simdutf/encoding_types.h */
#ifndef SIMDUTF_ENCODING_TYPES_H
#define SIMDUTF_ENCODING_TYPES_H
#include <string_view>

#if !defined(SIMDUTF_NO_STD_TEXT_ENCODING) &&                                  \
    defined(__cpp_lib_text_encoding) && __cpp_lib_text_encoding >= 202306L
  #define SIMDUTF_HAS_STD_TEXT_ENCODING 1
  #include <text_encoding>
#endif

namespace simdutf {

enum encoding_type {
  UTF8 = 1,      // BOM 0xef 0xbb 0xbf
  UTF16_LE = 2,  // BOM 0xff 0xfe
  UTF16_BE = 4,  // BOM 0xfe 0xff
  UTF32_LE = 8,  // BOM 0xff 0xfe 0x00 0x00
  UTF32_BE = 16, // BOM 0x00 0x00 0xfe 0xff
  Latin1 = 32,

  unspecified = 0
};

#ifndef SIMDUTF_IS_BIG_ENDIAN
  #error "SIMDUTF_IS_BIG_ENDIAN needs to be defined."
#endif

enum endianness {
  LITTLE = 0,
  BIG = 1,
  NATIVE =
#if SIMDUTF_IS_BIG_ENDIAN
      BIG
#else
      LITTLE
#endif
};

simdutf_warn_unused simdutf_really_inline constexpr bool
match_system(endianness e) {
  return e == endianness::NATIVE;
}

simdutf_warn_unused std::string_view to_string(encoding_type bom);

// Note that BOM for UTF8 is discouraged.
namespace BOM {

/**
 * Checks for a BOM. If not, returns unspecified
 * @param input         the string to process
 * @param length        the length of the string in code units
 * @return the corresponding encoding
 */

simdutf_warn_unused encoding_type check_bom(const uint8_t *byte, size_t length);
simdutf_warn_unused encoding_type check_bom(const char *byte, size_t length);
/**
 * Returns the size, in bytes, of the BOM for a given encoding type.
 * Note that UTF8 BOM are discouraged.
 * @param bom         the encoding type
 * @return the size in bytes of the corresponding BOM
 */
simdutf_warn_unused size_t bom_byte_size(encoding_type bom);

} // namespace BOM
} // namespace simdutf
#endif
/* end file include/simdutf/encoding_types.h */
/* begin file include/simdutf/error.h */
#ifndef SIMDUTF_ERROR_H
#define SIMDUTF_ERROR_H
#include <string_view>

namespace simdutf {

enum error_code {
  SUCCESS = 0,
  HEADER_BITS, // Any byte must have fewer than 5 header bits.
  TOO_SHORT,   // The leading byte must be followed by N-1 continuation bytes,
               // where N is the UTF-8 character length This is also the error
               // when the input is truncated.
  TOO_LONG,    // We either have too many consecutive continuation bytes or the
               // string starts with a continuation byte.
  OVERLONG, // The decoded character must be above U+7F for two-byte characters,
            // U+7FF for three-byte characters, and U+FFFF for four-byte
            // characters.
  TOO_LARGE, // The decoded character must be less than or equal to
             // U+10FFFF,less than or equal than U+7F for ASCII OR less than
             // equal than U+FF for Latin1
  SURROGATE, // The decoded character must be not be in U+D800...DFFF (UTF-8 or
             // UTF-32)
             // OR
             // a high surrogate must be followed by a low surrogate
             // and a low surrogate must be preceded by a high surrogate
             // (UTF-16)
             // OR
             // there must be no surrogate at all and one is
             // found (Latin1 functions)
             // OR
             // *specifically* for the function
             // utf8_length_from_utf16_with_replacement, a surrogate (whether
             // in error or not) has been found (I.e., whether we are in the
             // Basic Multilingual Plane or not).
  INVALID_BASE64_CHARACTER, // Found a character that cannot be part of a valid
                            // base64 string. This may include a misplaced
                            // padding character ('=').
  BASE64_INPUT_REMAINDER,   // The base64 input terminates with a single
                            // character, excluding padding (=). It is also used
                            // in strict mode when padding is not adequate.
  BASE64_EXTRA_BITS,        // The base64 input terminates with non-zero
                            // padding bits.
  OUTPUT_BUFFER_TOO_SMALL,  // The provided buffer is too small.
  OTHER                     // Not related to validation/transcoding.
};

inline std::string_view error_to_string(error_code code) noexcept {
  switch (code) {
  case SUCCESS:
    return "SUCCESS";
  case HEADER_BITS:
    return "HEADER_BITS";
  case TOO_SHORT:
    return "TOO_SHORT";
  case TOO_LONG:
    return "TOO_LONG";
  case OVERLONG:
    return "OVERLONG";
  case TOO_LARGE:
    return "TOO_LARGE";
  case SURROGATE:
    return "SURROGATE";
  case INVALID_BASE64_CHARACTER:
    return "INVALID_BASE64_CHARACTER";
  case BASE64_INPUT_REMAINDER:
    return "BASE64_INPUT_REMAINDER";
  case BASE64_EXTRA_BITS:
    return "BASE64_EXTRA_BITS";
  case OUTPUT_BUFFER_TOO_SMALL:
    return "OUTPUT_BUFFER_TOO_SMALL";
  default:
    return "OTHER";
  }
}

struct result {
  error_code error;
  size_t count; // In case of error, indicates the position of the error. In
                // case of success, indicates the number of code units
                // validated/written.

  simdutf_really_inline simdutf_constexpr23 result() noexcept
      : error{error_code::SUCCESS}, count{0} {}

  simdutf_really_inline simdutf_constexpr23 result(error_code err,
                                                   size_t pos) noexcept
      : error{err}, count{pos} {}

  simdutf_really_inline simdutf_constexpr23 bool is_ok() const noexcept {
    return error == error_code::SUCCESS;
  }

  simdutf_really_inline simdutf_constexpr23 bool is_err() const noexcept {
    return error != error_code::SUCCESS;
  }
};

struct full_result {
  error_code error;
  size_t input_count;
  size_t output_count;
  bool padding_error = false; // true if the error is due to padding, only
                              // meaningful when error is not SUCCESS

  simdutf_really_inline simdutf_constexpr23 full_result() noexcept
      : error{error_code::SUCCESS}, input_count{0}, output_count{0} {}

  simdutf_really_inline simdutf_constexpr23 full_result(error_code err,
                                                        size_t pos_in,
                                                        size_t pos_out) noexcept
      : error{err}, input_count{pos_in}, output_count{pos_out} {}
  simdutf_really_inline simdutf_constexpr23 full_result(
      error_code err, size_t pos_in, size_t pos_out, bool padding_err) noexcept
      : error{err}, input_count{pos_in}, output_count{pos_out},
        padding_error{padding_err} {}

  simdutf_really_inline simdutf_constexpr23 operator result() const noexcept {
    if (error == error_code::SUCCESS) {
      return result{error, output_count};
    } else {
      return result{error, input_count};
    }
  }
};

} // namespace simdutf
#endif
/* end file include/simdutf/error.h */

SIMDUTF_PUSH_DISABLE_WARNINGS
SIMDUTF_DISABLE_UNDESIRED_WARNINGS

// Public API
/* begin file include/simdutf/simdutf_version.h */
// /include/simdutf/simdutf_version.h automatically generated by release.py,
// do not change by hand
#ifndef SIMDUTF_SIMDUTF_VERSION_H
#define SIMDUTF_SIMDUTF_VERSION_H

/** The version of simdutf being used (major.minor.revision) */
#define SIMDUTF_VERSION "9.0.0"

namespace simdutf {
enum {
  /**
   * The major version (MAJOR.minor.revision) of simdutf being used.
   */
  SIMDUTF_VERSION_MAJOR = 9,
  /**
   * The minor version (major.MINOR.revision) of simdutf being used.
   */
  SIMDUTF_VERSION_MINOR = 0,
  /**
   * The revision (major.minor.REVISION) of simdutf being used.
   */
  SIMDUTF_VERSION_REVISION = 0
};
} // namespace simdutf

#endif // SIMDUTF_SIMDUTF_VERSION_H
/* end file include/simdutf/simdutf_version.h */
/* begin file include/simdutf/implementation.h */
#ifndef SIMDUTF_IMPLEMENTATION_H
#define SIMDUTF_IMPLEMENTATION_H
#if !defined(SIMDUTF_NO_THREADS)
  #include <atomic>
#endif
#ifdef SIMDUTF_INTERNAL_TESTS
  #include <vector>
#endif
/* begin file include/simdutf/internal/isadetection.h */
/* From
https://github.com/endorno/pytorch/blob/master/torch/lib/TH/generic/simd/simd.h
Highly modified.

Copyright (c) 2016-     Facebook, Inc            (Adam Paszke)
Copyright (c) 2014-     Facebook, Inc            (Soumith Chintala)
Copyright (c) 2011-2014 Idiap Research Institute (Ronan Collobert)
Copyright (c) 2012-2014 Deepmind Technologies    (Koray Kavukcuoglu)
Copyright (c) 2011-2012 NEC Laboratories America (Koray Kavukcuoglu)
Copyright (c) 2011-2013 NYU                      (Clement Farabet)
Copyright (c) 2006-2010 NEC Laboratories America (Ronan Collobert, Leon Bottou,
Iain Melvin, Jason Weston) Copyright (c) 2006      Idiap Research Institute
(Samy Bengio) Copyright (c) 2001-2004 Idiap Research Institute (Ronan Collobert,
Samy Bengio, Johnny Mariethoz)

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

3. Neither the names of Facebook, Deepmind Technologies, NYU, NEC Laboratories
America and IDIAP Research Institute nor the names of its contributors may be
   used to endorse or promote products derived from this software without
   specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SIMDutf_INTERNAL_ISADETECTION_H
#define SIMDutf_INTERNAL_ISADETECTION_H

#include <cstdint>
#include <cstdlib>
#if defined(_MSC_VER)
  #include <intrin.h>
#elif defined(HAVE_GCC_GET_CPUID) && defined(USE_GCC_GET_CPUID)
  #include <cpuid.h>
#endif


// RISC-V ISA detection utilities
#if SIMDUTF_IS_RISCV64 && defined(__linux__)
  #include <unistd.h> // for syscall
// We define these ourselves, for backwards compatibility
struct simdutf_riscv_hwprobe {
  int64_t key;
  uint64_t value;
};
  #define simdutf_riscv_hwprobe(...) syscall(258, __VA_ARGS__)
  #define SIMDUTF_RISCV_HWPROBE_KEY_IMA_EXT_0 4
  #define SIMDUTF_RISCV_HWPROBE_IMA_V (1 << 2)
  #define SIMDUTF_RISCV_HWPROBE_EXT_ZVBB (1 << 17)
#endif // SIMDUTF_IS_RISCV64 && defined(__linux__)

#if defined(__loongarch__) && defined(__linux__)
  #include <sys/auxv.h>
// bits/hwcap.h
// #define HWCAP_LOONGARCH_LSX             (1 << 4)
// #define HWCAP_LOONGARCH_LASX            (1 << 5)
#endif

namespace simdutf {
namespace internal {

enum instruction_set {
  DEFAULT = 0x0,
  NEON = 0x1,
  AVX2 = 0x4,
  SSE42 = 0x8,
  PCLMULQDQ = 0x10,
  BMI1 = 0x20,
  BMI2 = 0x40,
  ALTIVEC = 0x80,
  AVX512F = 0x100,
  AVX512DQ = 0x200,
  AVX512IFMA = 0x400,
  AVX512PF = 0x800,
  AVX512ER = 0x1000,
  AVX512CD = 0x2000,
  AVX512BW = 0x4000,
  AVX512VL = 0x8000,
  AVX512VBMI2 = 0x10000,
  AVX512VPOPCNTDQ = 0x2000,
  RVV = 0x4000,
  ZVBB = 0x8000,
  LSX = 0x40000,
  LASX = 0x80000,
};

#if defined(__PPC64__)

static inline uint32_t detect_supported_architectures() {
  return instruction_set::ALTIVEC;
}

#elif SIMDUTF_IS_RISCV64

static inline uint32_t detect_supported_architectures() {
  uint32_t host_isa = instruction_set::DEFAULT;
  #if SIMDUTF_IS_RVV
  host_isa |= instruction_set::RVV;
  #endif
  #if SIMDUTF_IS_ZVBB
  host_isa |= instruction_set::ZVBB;
  #endif
  #if defined(__linux__)
  simdutf_riscv_hwprobe probes[] = {{SIMDUTF_RISCV_HWPROBE_KEY_IMA_EXT_0, 0}};
  long ret = simdutf_riscv_hwprobe(&probes, sizeof probes / sizeof *probes, 0,
                                   nullptr, 0);
  if (ret == 0) {
    uint64_t extensions = probes[0].value;
    if (extensions & SIMDUTF_RISCV_HWPROBE_IMA_V)
      host_isa |= instruction_set::RVV;
    if (extensions & SIMDUTF_RISCV_HWPROBE_EXT_ZVBB)
      host_isa |= instruction_set::ZVBB;
  }
  #endif
  #if defined(RUN_IN_SPIKE_SIMULATOR)
  // Proxy Kernel does not implement yet hwprobe syscall
  host_isa |= instruction_set::RVV;
  #endif
  return host_isa;
}

#elif defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)

static inline uint32_t detect_supported_architectures() {
  return instruction_set::NEON;
}

#elif defined(__x86_64__) || defined(_M_AMD64) // x64

namespace {
namespace cpuid_bit {
// Can be found on Intel ISA Reference for CPUID

// EAX = 0x01
constexpr uint32_t pclmulqdq = uint32_t(1)
                               << 1; ///< @private bit  1 of ECX for EAX=0x1
constexpr uint32_t sse42 = uint32_t(1)
                           << 20; ///< @private bit 20 of ECX for EAX=0x1
constexpr uint32_t osxsave =
    (uint32_t(1) << 26) |
    (uint32_t(1) << 27); ///< @private bits 26+27 of ECX for EAX=0x1

// EAX = 0x7f (Structured Extended Feature Flags), ECX = 0x00 (Sub-leaf)
// See: "Table 3-8. Information Returned by CPUID Instruction"
namespace ebx {
constexpr uint32_t bmi1 = uint32_t(1) << 3;
constexpr uint32_t avx2 = uint32_t(1) << 5;
constexpr uint32_t bmi2 = uint32_t(1) << 8;
constexpr uint32_t avx512f = uint32_t(1) << 16;
constexpr uint32_t avx512dq = uint32_t(1) << 17;
constexpr uint32_t avx512ifma = uint32_t(1) << 21;
constexpr uint32_t avx512cd = uint32_t(1) << 28;
constexpr uint32_t avx512bw = uint32_t(1) << 30;
constexpr uint32_t avx512vl = uint32_t(1) << 31;
} // namespace ebx

namespace ecx {
constexpr uint32_t avx512vbmi = uint32_t(1) << 1;
constexpr uint32_t avx512vbmi2 = uint32_t(1) << 6;
constexpr uint32_t avx512vnni = uint32_t(1) << 11;
constexpr uint32_t avx512bitalg = uint32_t(1) << 12;
constexpr uint32_t avx512vpopcnt = uint32_t(1) << 14;
} // namespace ecx
namespace edx {
constexpr uint32_t avx512vp2intersect = uint32_t(1) << 8;
}
namespace xcr0_bit {
constexpr uint64_t avx256_saved = uint64_t(1) << 2; ///< @private bit 2 = AVX
constexpr uint64_t avx512_saved =
    uint64_t(7) << 5; ///< @private bits 5,6,7 = opmask, ZMM_hi256, hi16_ZMM
} // namespace xcr0_bit
} // namespace cpuid_bit
} // namespace

static inline void cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx,
                         uint32_t *edx) {
  #if defined(_MSC_VER)
  int cpu_info[4];
  __cpuidex(cpu_info, *eax, *ecx);
  *eax = cpu_info[0];
  *ebx = cpu_info[1];
  *ecx = cpu_info[2];
  *edx = cpu_info[3];
  #elif defined(HAVE_GCC_GET_CPUID) && defined(USE_GCC_GET_CPUID)
  uint32_t level = *eax;
  __get_cpuid(level, eax, ebx, ecx, edx);
  #else
  uint32_t a = *eax, b, c = *ecx, d;
  asm volatile("cpuid\n\t" : "+a"(a), "=b"(b), "+c"(c), "=d"(d));
  *eax = a;
  *ebx = b;
  *ecx = c;
  *edx = d;
  #endif
}

static inline uint64_t xgetbv() {
  #if defined(_MSC_VER)
  return _xgetbv(0);
  #else
  uint32_t xcr0_lo, xcr0_hi;
  asm volatile("xgetbv\n\t" : "=a"(xcr0_lo), "=d"(xcr0_hi) : "c"(0));
  return xcr0_lo | ((uint64_t)xcr0_hi << 32);
  #endif
}

static inline uint32_t detect_supported_architectures() {
  uint32_t eax;
  uint32_t ebx = 0;
  uint32_t ecx = 0;
  uint32_t edx = 0;
  uint32_t host_isa = 0x0;

  // EBX for EAX=0x1
  eax = 0x1;
  cpuid(&eax, &ebx, &ecx, &edx);

  if (ecx & cpuid_bit::sse42) {
    host_isa |= instruction_set::SSE42;
  }

  if (ecx & cpuid_bit::pclmulqdq) {
    host_isa |= instruction_set::PCLMULQDQ;
  }

  if ((ecx & cpuid_bit::osxsave) != cpuid_bit::osxsave) {
    return host_isa;
  }

  // xgetbv for checking if the OS saves registers
  uint64_t xcr0 = xgetbv();

  if ((xcr0 & cpuid_bit::xcr0_bit::avx256_saved) == 0) {
    return host_isa;
  }
  // ECX for EAX=0x7
  eax = 0x7;
  ecx = 0x0; // Sub-leaf = 0
  cpuid(&eax, &ebx, &ecx, &edx);
  if (ebx & cpuid_bit::ebx::avx2) {
    host_isa |= instruction_set::AVX2;
  }
  if (ebx & cpuid_bit::ebx::bmi1) {
    host_isa |= instruction_set::BMI1;
  }
  if (ebx & cpuid_bit::ebx::bmi2) {
    host_isa |= instruction_set::BMI2;
  }
  if (!((xcr0 & cpuid_bit::xcr0_bit::avx512_saved) ==
        cpuid_bit::xcr0_bit::avx512_saved)) {
    return host_isa;
  }
  if (ebx & cpuid_bit::ebx::avx512f) {
    host_isa |= instruction_set::AVX512F;
  }
  if (ebx & cpuid_bit::ebx::avx512bw) {
    host_isa |= instruction_set::AVX512BW;
  }
  if (ebx & cpuid_bit::ebx::avx512cd) {
    host_isa |= instruction_set::AVX512CD;
  }
  if (ebx & cpuid_bit::ebx::avx512dq) {
    host_isa |= instruction_set::AVX512DQ;
  }
  if (ebx & cpuid_bit::ebx::avx512vl) {
    host_isa |= instruction_set::AVX512VL;
  }
  if (ecx & cpuid_bit::ecx::avx512vbmi2) {
    host_isa |= instruction_set::AVX512VBMI2;
  }
  if (ecx & cpuid_bit::ecx::avx512vpopcnt) {
    host_isa |= instruction_set::AVX512VPOPCNTDQ;
  }
  return host_isa;
}
#elif defined(__loongarch__)

static inline uint32_t detect_supported_architectures() {
  uint32_t host_isa = instruction_set::DEFAULT;
  #if defined(__linux__)
  uint64_t hwcap = 0;
  hwcap = getauxval(AT_HWCAP);
  if (hwcap & HWCAP_LOONGARCH_LSX) {
    host_isa |= instruction_set::LSX;
  }
  if (hwcap & HWCAP_LOONGARCH_LASX) {
    host_isa |= instruction_set::LASX;
  }
  #endif
  return host_isa;
}
#else // fallback

// includes 32-bit ARM.
static inline uint32_t detect_supported_architectures() {
  return instruction_set::DEFAULT;
}

#endif // end SIMD extension detection code

} // namespace internal
} // namespace simdutf

#endif // SIMDutf_INTERNAL_ISADETECTION_H
/* end file include/simdutf/internal/isadetection.h */

#include <string_view>
#if SIMDUTF_SPAN
  #include <concepts>
  #include <type_traits>
  #include <span>
  #include <tuple>
  #include <utility> // for std::unreachable
#endif
// The following defines are conditionally enabled/disabled during amalgamation.
// By default all features are enabled, regular code shouldn't check them. Only
// when user code really relies of a selected subset, it's good to verify these
// flags, like:
//
//      #if !SIMDUTF_FEATURE_UTF16
//      #   error("Please amalgamate simdutf with UTF-16 support")
//      #endif
//
#define SIMDUTF_FEATURE_DETECT_ENCODING 0
#define SIMDUTF_FEATURE_ASCII 0
#define SIMDUTF_FEATURE_LATIN1 0
#define SIMDUTF_FEATURE_UTF8 0
#define SIMDUTF_FEATURE_UTF16 0
#define SIMDUTF_FEATURE_UTF32 0
#define SIMDUTF_FEATURE_BASE64 1

#if SIMDUTF_CPLUSPLUS23
/* begin file include/simdutf/constexpr_ptr.h */
#ifndef SIMDUTF_CONSTEXPR_PTR_H
#define SIMDUTF_CONSTEXPR_PTR_H

#include <cstddef>

namespace simdutf {
namespace detail {
/**
 * The constexpr_ptr class is a workaround for reinterpret_cast not being
 * allowed during constant evaluation.
 */
template <typename to, typename from>
  requires(sizeof(to) == sizeof(from))
struct constexpr_ptr {
  const from *p;

  constexpr explicit constexpr_ptr(const from *ptr) noexcept : p(ptr) {}

  constexpr to operator*() const noexcept { return static_cast<to>(*p); }

  constexpr constexpr_ptr &operator++() noexcept {
    ++p;
    return *this;
  }

  constexpr constexpr_ptr operator++(int) noexcept {
    auto old = *this;
    ++p;
    return old;
  }

  constexpr constexpr_ptr &operator--() noexcept {
    --p;
    return *this;
  }

  constexpr constexpr_ptr operator--(int) noexcept {
    auto old = *this;
    --p;
    return old;
  }

  constexpr constexpr_ptr &operator+=(std::ptrdiff_t n) noexcept {
    p += n;
    return *this;
  }

  constexpr constexpr_ptr &operator-=(std::ptrdiff_t n) noexcept {
    p -= n;
    return *this;
  }

  constexpr constexpr_ptr operator+(std::ptrdiff_t n) const noexcept {
    return constexpr_ptr{p + n};
  }

  constexpr constexpr_ptr operator-(std::ptrdiff_t n) const noexcept {
    return constexpr_ptr{p - n};
  }

  constexpr std::ptrdiff_t operator-(const constexpr_ptr &o) const noexcept {
    return p - o.p;
  }

  constexpr to operator[](std::ptrdiff_t n) const noexcept {
    return static_cast<to>(*(p + n));
  }

  // to prevent compilation errors for memcpy, even if it is never
  // called during constant evaluation
  constexpr operator const void *() const noexcept { return p; }
};

template <typename to, typename from>
constexpr constexpr_ptr<to, from> constexpr_cast_ptr(from *p) noexcept {
  return constexpr_ptr<to, from>{p};
}

/**
 * helper type for constexpr_writeptr, so it is possible to
 * do "*ptr = val;"
 */
template <typename SrcType, typename TargetType>
struct constexpr_write_ptr_proxy {

  constexpr explicit constexpr_write_ptr_proxy(TargetType *raw) : p(raw) {}

  constexpr constexpr_write_ptr_proxy &operator=(SrcType v) {
    *p = static_cast<TargetType>(v);
    return *this;
  }

  TargetType *p;
};

/**
 * helper for working around reinterpret_cast not being allowed during constexpr
 * evaluation. will try to act as a SrcType* but actually write to the pointer
 * given in the constructor, which is of another type TargetType
 */
template <typename SrcType, typename TargetType> struct constexpr_write_ptr {
  constexpr explicit constexpr_write_ptr(TargetType *raw) : p(raw) {}

  constexpr constexpr_write_ptr_proxy<SrcType, TargetType> operator*() const {
    return constexpr_write_ptr_proxy<SrcType, TargetType>{p};
  }

  constexpr constexpr_write_ptr_proxy<SrcType, TargetType>
  operator[](std::ptrdiff_t n) const {
    return constexpr_write_ptr_proxy<SrcType, TargetType>{p + n};
  }

  constexpr constexpr_write_ptr &operator++() {
    ++p;
    return *this;
  }

  constexpr constexpr_write_ptr operator++(int) {
    constexpr_write_ptr old = *this;
    ++p;
    return old;
  }

  constexpr std::ptrdiff_t operator-(const constexpr_write_ptr &other) const {
    return p - other.p;
  }

  TargetType *p;
};

template <typename SrcType, typename TargetType>
constexpr auto constexpr_cast_writeptr(TargetType *raw) {
  return constexpr_write_ptr<SrcType, TargetType>{raw};
}

} // namespace detail
} // namespace simdutf
#endif
/* end file include/simdutf/constexpr_ptr.h */
#endif

#if SIMDUTF_SPAN
/// helpers placed in namespace detail are not a part of the public API
namespace simdutf {
namespace detail {
/**
 * matches a byte, in the many ways C++ allows. note that these
 * are all distinct types.
 */
template <typename T>
concept byte_like = std::is_same_v<T, std::byte> ||     //
                    std::is_same_v<T, char> ||          //
                    std::is_same_v<T, signed char> ||   //
                    std::is_same_v<T, unsigned char> || //
                    std::is_same_v<T, char8_t>;

template <typename T>
concept is_byte_like = byte_like<std::remove_cvref_t<T>>;

template <typename T>
concept is_pointer = std::is_pointer_v<T>;

/**
 * matches anything that behaves like std::span and points to character-like
 * data such as: std::byte, char, unsigned char, signed char, std::int8_t,
 * std::uint8_t
 */
template <typename T>
concept input_span_of_byte_like = requires(const T &t) {
  { t.size() } noexcept -> std::convertible_to<std::size_t>;
  { t.data() } noexcept -> is_pointer;
  { *t.data() } noexcept -> is_byte_like;
};

template <typename T>
concept is_mutable = !std::is_const_v<std::remove_reference_t<T>>;

/**
 * like span_of_byte_like, but for an output span (intended to be written to)
 */
template <typename T>
concept output_span_of_byte_like = requires(T &t) {
  { t.size() } noexcept -> std::convertible_to<std::size_t>;
  { t.data() } noexcept -> is_pointer;
  { *t.data() } noexcept -> is_byte_like;
  { *t.data() } noexcept -> is_mutable;
};

/**
 * a pointer like object, when indexed, results in a byte like result.
 * valid examples: char*, const char*, std::array<char,10>
 * invalid examples: int*, std::array<int,10>
 */
template <class InputPtr>
concept indexes_into_byte_like = requires(InputPtr p) {
  { std::decay_t<decltype(p[0])>{} } -> simdutf::detail::byte_like;
};
template <class InputPtr>
concept indexes_into_utf16 = requires(InputPtr p) {
  { std::decay_t<decltype(p[0])>{} } -> std::same_as<char16_t>;
};
template <class InputPtr>
concept indexes_into_utf32 = requires(InputPtr p) {
  { std::decay_t<decltype(p[0])>{} } -> std::same_as<char32_t>;
};

template <class InputPtr>
concept index_assignable_from_char = requires(InputPtr p, char s) {
  { p[0] = s };
};

/**
 * a pointer like object that results in a uint32_t when indexed.
 * valid examples: uint32_t*
 */
template <class InputPtr>
concept indexes_into_uint32 = requires(InputPtr p) {
  { std::decay_t<decltype(p[0])>{} } -> std::same_as<std::uint32_t>;
};
} // namespace detail
} // namespace simdutf
#endif // SIMDUTF_SPAN

// these includes are needed for constexpr support. they are
// not part of the public api.
/* begin file include/simdutf/scalar/swap_bytes.h */
#ifndef SIMDUTF_SWAP_BYTES_H
#define SIMDUTF_SWAP_BYTES_H

namespace simdutf {
namespace scalar {

constexpr inline simdutf_warn_unused uint16_t
u16_swap_bytes(const uint16_t word) {
  return uint16_t((word >> 8) | (word << 8));
}

constexpr inline simdutf_warn_unused uint32_t
u32_swap_bytes(const uint32_t word) {
  return ((word >> 24) & 0xff) |      // move byte 3 to byte 0
         ((word << 8) & 0xff0000) |   // move byte 1 to byte 2
         ((word >> 8) & 0xff00) |     // move byte 2 to byte 1
         ((word << 24) & 0xff000000); // byte 0 to byte 3
}

namespace utf32 {
template <endianness big_endian> constexpr uint32_t swap_if_needed(uint32_t c) {
  return !match_system(big_endian) ? scalar::u32_swap_bytes(c) : c;
}
} // namespace utf32

namespace utf16 {
template <endianness big_endian> constexpr uint16_t swap_if_needed(uint16_t c) {
  return !match_system(big_endian) ? scalar::u16_swap_bytes(c) : c;
}
} // namespace utf16

} // namespace scalar
} // namespace simdutf

#endif
/* end file include/simdutf/scalar/swap_bytes.h */

namespace simdutf {

constexpr size_t default_line_length =
    76; ///< default line length for base64 encoding with lines

  #ifndef SIMDUTF_NEED_TRAILING_ZEROES
    #define SIMDUTF_NEED_TRAILING_ZEROES 1
  #endif

// base64_options are used to specify the base64 encoding options.
// ASCII spaces are ' ', '\t', '\n', '\r', '\f'
// garbage characters are characters that are not part of the base64 alphabet
// nor ASCII spaces.
constexpr uint64_t base64_reverse_padding =
    2; /* modifier for base64_default and base64_url */
enum base64_options : uint64_t {
  base64_default = 0, /* standard base64 format (with padding) */
  base64_url = 1,     /* base64url format (no padding) */
  base64_default_no_padding =
      base64_default |
      base64_reverse_padding, /* standard base64 format without padding */
  base64_url_with_padding =
      base64_url | base64_reverse_padding, /* base64url with padding */
  base64_default_accept_garbage =
      4, /* standard base64 format accepting garbage characters, the input stops
            with the first '=' if any */
  base64_url_accept_garbage =
      5, /* base64url format accepting garbage characters, the input stops with
            the first '=' if any */
  base64_default_or_url =
      8, /* standard/base64url hybrid format (only meaningful for decoding!) */
  base64_default_or_url_accept_garbage =
      12, /* standard/base64url hybrid format accepting garbage characters
             (only meaningful for decoding!), the input stops with the first '='
             if any */
};

// last_chunk_handling_options are used to specify the handling of the last
// chunk in base64 decoding.
// https://tc39.es/proposal-arraybuffer-base64/spec/#sec-frombase64
enum last_chunk_handling_options : uint64_t {
  loose = 0,  /* standard base64 format, decode partial final chunk */
  strict = 1, /* error when the last chunk is partial, 2 or 3 chars, and
                 unpadded, or non-zero bit padding */
  stop_before_partial =
      2, /* if the last chunk is partial, ignore it (no error) */
  only_full_chunks =
      3 /* only decode full blocks (4 base64 characters, no padding) */
};

inline simdutf_constexpr23 bool
is_partial(last_chunk_handling_options options) {
  return (options == stop_before_partial) || (options == only_full_chunks);
}

namespace detail {
simdutf_warn_unused const char *find(const char *start, const char *end,
                                     char character) noexcept;
simdutf_warn_unused const char16_t *
find(const char16_t *start, const char16_t *end, char16_t character) noexcept;
} // namespace detail

/**
 * Find the first occurrence of a character in a string. If the character is
 * not found, return a pointer to the end of the string.
 * @param start        the start of the string
 * @param end          the end of the string
 * @param character    the character to find
 * @return a pointer to the first occurrence of the character in the string,
 * or a pointer to the end of the string if the character is not found.
 *
 */
simdutf_warn_unused simdutf_really_inline simdutf_constexpr23 const char *
find(const char *start, const char *end, char character) noexcept {
  #if SIMDUTF_CPLUSPLUS23
  if consteval {
    for (; start != end; ++start)
      if (*start == character)
        return start;
    return end;
  } else
  #endif
  {
    return detail::find(start, end, character);
  }
}
simdutf_warn_unused simdutf_really_inline simdutf_constexpr23 const char16_t *
find(const char16_t *start, const char16_t *end, char16_t character) noexcept {
    // implementation note: this is repeated instead of a template, to ensure
    // the api is still a function and compiles without concepts
  #if SIMDUTF_CPLUSPLUS23
  if consteval {
    for (; start != end; ++start)
      if (*start == character)
        return start;
    return end;
  } else
  #endif
  {
    return detail::find(start, end, character);
  }
}
}
  // We include base64_tables once.
/* begin file include/simdutf/base64_tables.h */
#ifndef SIMDUTF_BASE64_TABLES_H
#define SIMDUTF_BASE64_TABLES_H
#include <cstdint>

namespace simdutf {
namespace {
namespace tables {
namespace base64 {
namespace base64_default {

constexpr char e0[256] = {
    'A', 'A', 'A', 'A', 'B', 'B', 'B', 'B', 'C', 'C', 'C', 'C', 'D', 'D', 'D',
    'D', 'E', 'E', 'E', 'E', 'F', 'F', 'F', 'F', 'G', 'G', 'G', 'G', 'H', 'H',
    'H', 'H', 'I', 'I', 'I', 'I', 'J', 'J', 'J', 'J', 'K', 'K', 'K', 'K', 'L',
    'L', 'L', 'L', 'M', 'M', 'M', 'M', 'N', 'N', 'N', 'N', 'O', 'O', 'O', 'O',
    'P', 'P', 'P', 'P', 'Q', 'Q', 'Q', 'Q', 'R', 'R', 'R', 'R', 'S', 'S', 'S',
    'S', 'T', 'T', 'T', 'T', 'U', 'U', 'U', 'U', 'V', 'V', 'V', 'V', 'W', 'W',
    'W', 'W', 'X', 'X', 'X', 'X', 'Y', 'Y', 'Y', 'Y', 'Z', 'Z', 'Z', 'Z', 'a',
    'a', 'a', 'a', 'b', 'b', 'b', 'b', 'c', 'c', 'c', 'c', 'd', 'd', 'd', 'd',
    'e', 'e', 'e', 'e', 'f', 'f', 'f', 'f', 'g', 'g', 'g', 'g', 'h', 'h', 'h',
    'h', 'i', 'i', 'i', 'i', 'j', 'j', 'j', 'j', 'k', 'k', 'k', 'k', 'l', 'l',
    'l', 'l', 'm', 'm', 'm', 'm', 'n', 'n', 'n', 'n', 'o', 'o', 'o', 'o', 'p',
    'p', 'p', 'p', 'q', 'q', 'q', 'q', 'r', 'r', 'r', 'r', 's', 's', 's', 's',
    't', 't', 't', 't', 'u', 'u', 'u', 'u', 'v', 'v', 'v', 'v', 'w', 'w', 'w',
    'w', 'x', 'x', 'x', 'x', 'y', 'y', 'y', 'y', 'z', 'z', 'z', 'z', '0', '0',
    '0', '0', '1', '1', '1', '1', '2', '2', '2', '2', '3', '3', '3', '3', '4',
    '4', '4', '4', '5', '5', '5', '5', '6', '6', '6', '6', '7', '7', '7', '7',
    '8', '8', '8', '8', '9', '9', '9', '9', '+', '+', '+', '+', '/', '/', '/',
    '/'};

constexpr char e1[256] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
    't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '+', '/', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
    'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
    'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
    'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', 'A', 'B', 'C',
    'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
    'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+',
    '/'};

constexpr char e2[256] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
    't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '+', '/', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
    'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
    'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
    'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', 'A', 'B', 'C',
    'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
    'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+',
    '/'};

constexpr uint32_t d0[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x000000f8, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x000000fc,
    0x000000d0, 0x000000d4, 0x000000d8, 0x000000dc, 0x000000e0, 0x000000e4,
    0x000000e8, 0x000000ec, 0x000000f0, 0x000000f4, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00000004, 0x00000008, 0x0000000c, 0x00000010, 0x00000014, 0x00000018,
    0x0000001c, 0x00000020, 0x00000024, 0x00000028, 0x0000002c, 0x00000030,
    0x00000034, 0x00000038, 0x0000003c, 0x00000040, 0x00000044, 0x00000048,
    0x0000004c, 0x00000050, 0x00000054, 0x00000058, 0x0000005c, 0x00000060,
    0x00000064, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x00000068, 0x0000006c, 0x00000070, 0x00000074, 0x00000078,
    0x0000007c, 0x00000080, 0x00000084, 0x00000088, 0x0000008c, 0x00000090,
    0x00000094, 0x00000098, 0x0000009c, 0x000000a0, 0x000000a4, 0x000000a8,
    0x000000ac, 0x000000b0, 0x000000b4, 0x000000b8, 0x000000bc, 0x000000c0,
    0x000000c4, 0x000000c8, 0x000000cc, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};

constexpr uint32_t d1[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x0000e003, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x0000f003,
    0x00004003, 0x00005003, 0x00006003, 0x00007003, 0x00008003, 0x00009003,
    0x0000a003, 0x0000b003, 0x0000c003, 0x0000d003, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00001000, 0x00002000, 0x00003000, 0x00004000, 0x00005000, 0x00006000,
    0x00007000, 0x00008000, 0x00009000, 0x0000a000, 0x0000b000, 0x0000c000,
    0x0000d000, 0x0000e000, 0x0000f000, 0x00000001, 0x00001001, 0x00002001,
    0x00003001, 0x00004001, 0x00005001, 0x00006001, 0x00007001, 0x00008001,
    0x00009001, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x0000a001, 0x0000b001, 0x0000c001, 0x0000d001, 0x0000e001,
    0x0000f001, 0x00000002, 0x00001002, 0x00002002, 0x00003002, 0x00004002,
    0x00005002, 0x00006002, 0x00007002, 0x00008002, 0x00009002, 0x0000a002,
    0x0000b002, 0x0000c002, 0x0000d002, 0x0000e002, 0x0000f002, 0x00000003,
    0x00001003, 0x00002003, 0x00003003, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};

constexpr uint32_t d2[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x00800f00, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00c00f00,
    0x00000d00, 0x00400d00, 0x00800d00, 0x00c00d00, 0x00000e00, 0x00400e00,
    0x00800e00, 0x00c00e00, 0x00000f00, 0x00400f00, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00400000, 0x00800000, 0x00c00000, 0x00000100, 0x00400100, 0x00800100,
    0x00c00100, 0x00000200, 0x00400200, 0x00800200, 0x00c00200, 0x00000300,
    0x00400300, 0x00800300, 0x00c00300, 0x00000400, 0x00400400, 0x00800400,
    0x00c00400, 0x00000500, 0x00400500, 0x00800500, 0x00c00500, 0x00000600,
    0x00400600, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x00800600, 0x00c00600, 0x00000700, 0x00400700, 0x00800700,
    0x00c00700, 0x00000800, 0x00400800, 0x00800800, 0x00c00800, 0x00000900,
    0x00400900, 0x00800900, 0x00c00900, 0x00000a00, 0x00400a00, 0x00800a00,
    0x00c00a00, 0x00000b00, 0x00400b00, 0x00800b00, 0x00c00b00, 0x00000c00,
    0x00400c00, 0x00800c00, 0x00c00c00, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};

constexpr uint32_t d3[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x003e0000, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x003f0000,
    0x00340000, 0x00350000, 0x00360000, 0x00370000, 0x00380000, 0x00390000,
    0x003a0000, 0x003b0000, 0x003c0000, 0x003d0000, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00010000, 0x00020000, 0x00030000, 0x00040000, 0x00050000, 0x00060000,
    0x00070000, 0x00080000, 0x00090000, 0x000a0000, 0x000b0000, 0x000c0000,
    0x000d0000, 0x000e0000, 0x000f0000, 0x00100000, 0x00110000, 0x00120000,
    0x00130000, 0x00140000, 0x00150000, 0x00160000, 0x00170000, 0x00180000,
    0x00190000, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x001a0000, 0x001b0000, 0x001c0000, 0x001d0000, 0x001e0000,
    0x001f0000, 0x00200000, 0x00210000, 0x00220000, 0x00230000, 0x00240000,
    0x00250000, 0x00260000, 0x00270000, 0x00280000, 0x00290000, 0x002a0000,
    0x002b0000, 0x002c0000, 0x002d0000, 0x002e0000, 0x002f0000, 0x00300000,
    0x00310000, 0x00320000, 0x00330000, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};
} // namespace base64_default

namespace base64_url {

constexpr char e0[256] = {
    'A', 'A', 'A', 'A', 'B', 'B', 'B', 'B', 'C', 'C', 'C', 'C', 'D', 'D', 'D',
    'D', 'E', 'E', 'E', 'E', 'F', 'F', 'F', 'F', 'G', 'G', 'G', 'G', 'H', 'H',
    'H', 'H', 'I', 'I', 'I', 'I', 'J', 'J', 'J', 'J', 'K', 'K', 'K', 'K', 'L',
    'L', 'L', 'L', 'M', 'M', 'M', 'M', 'N', 'N', 'N', 'N', 'O', 'O', 'O', 'O',
    'P', 'P', 'P', 'P', 'Q', 'Q', 'Q', 'Q', 'R', 'R', 'R', 'R', 'S', 'S', 'S',
    'S', 'T', 'T', 'T', 'T', 'U', 'U', 'U', 'U', 'V', 'V', 'V', 'V', 'W', 'W',
    'W', 'W', 'X', 'X', 'X', 'X', 'Y', 'Y', 'Y', 'Y', 'Z', 'Z', 'Z', 'Z', 'a',
    'a', 'a', 'a', 'b', 'b', 'b', 'b', 'c', 'c', 'c', 'c', 'd', 'd', 'd', 'd',
    'e', 'e', 'e', 'e', 'f', 'f', 'f', 'f', 'g', 'g', 'g', 'g', 'h', 'h', 'h',
    'h', 'i', 'i', 'i', 'i', 'j', 'j', 'j', 'j', 'k', 'k', 'k', 'k', 'l', 'l',
    'l', 'l', 'm', 'm', 'm', 'm', 'n', 'n', 'n', 'n', 'o', 'o', 'o', 'o', 'p',
    'p', 'p', 'p', 'q', 'q', 'q', 'q', 'r', 'r', 'r', 'r', 's', 's', 's', 's',
    't', 't', 't', 't', 'u', 'u', 'u', 'u', 'v', 'v', 'v', 'v', 'w', 'w', 'w',
    'w', 'x', 'x', 'x', 'x', 'y', 'y', 'y', 'y', 'z', 'z', 'z', 'z', '0', '0',
    '0', '0', '1', '1', '1', '1', '2', '2', '2', '2', '3', '3', '3', '3', '4',
    '4', '4', '4', '5', '5', '5', '5', '6', '6', '6', '6', '7', '7', '7', '7',
    '8', '8', '8', '8', '9', '9', '9', '9', '-', '-', '-', '-', '_', '_', '_',
    '_'};

constexpr char e1[256] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
    't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '-', '_', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
    'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '-', '_', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
    'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
    'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_', 'A', 'B', 'C',
    'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
    'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-',
    '_'};

constexpr char e2[256] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
    't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '-', '_', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
    'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '-', '_', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
    'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
    'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_', 'A', 'B', 'C',
    'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
    'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-',
    '_'};

constexpr uint32_t d0[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x000000f8, 0x01ffffff, 0x01ffffff,
    0x000000d0, 0x000000d4, 0x000000d8, 0x000000dc, 0x000000e0, 0x000000e4,
    0x000000e8, 0x000000ec, 0x000000f0, 0x000000f4, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00000004, 0x00000008, 0x0000000c, 0x00000010, 0x00000014, 0x00000018,
    0x0000001c, 0x00000020, 0x00000024, 0x00000028, 0x0000002c, 0x00000030,
    0x00000034, 0x00000038, 0x0000003c, 0x00000040, 0x00000044, 0x00000048,
    0x0000004c, 0x00000050, 0x00000054, 0x00000058, 0x0000005c, 0x00000060,
    0x00000064, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x000000fc,
    0x01ffffff, 0x00000068, 0x0000006c, 0x00000070, 0x00000074, 0x00000078,
    0x0000007c, 0x00000080, 0x00000084, 0x00000088, 0x0000008c, 0x00000090,
    0x00000094, 0x00000098, 0x0000009c, 0x000000a0, 0x000000a4, 0x000000a8,
    0x000000ac, 0x000000b0, 0x000000b4, 0x000000b8, 0x000000bc, 0x000000c0,
    0x000000c4, 0x000000c8, 0x000000cc, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};
constexpr uint32_t d1[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x0000e003, 0x01ffffff, 0x01ffffff,
    0x00004003, 0x00005003, 0x00006003, 0x00007003, 0x00008003, 0x00009003,
    0x0000a003, 0x0000b003, 0x0000c003, 0x0000d003, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00001000, 0x00002000, 0x00003000, 0x00004000, 0x00005000, 0x00006000,
    0x00007000, 0x00008000, 0x00009000, 0x0000a000, 0x0000b000, 0x0000c000,
    0x0000d000, 0x0000e000, 0x0000f000, 0x00000001, 0x00001001, 0x00002001,
    0x00003001, 0x00004001, 0x00005001, 0x00006001, 0x00007001, 0x00008001,
    0x00009001, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x0000f003,
    0x01ffffff, 0x0000a001, 0x0000b001, 0x0000c001, 0x0000d001, 0x0000e001,
    0x0000f001, 0x00000002, 0x00001002, 0x00002002, 0x00003002, 0x00004002,
    0x00005002, 0x00006002, 0x00007002, 0x00008002, 0x00009002, 0x0000a002,
    0x0000b002, 0x0000c002, 0x0000d002, 0x0000e002, 0x0000f002, 0x00000003,
    0x00001003, 0x00002003, 0x00003003, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};
constexpr uint32_t d2[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00800f00, 0x01ffffff, 0x01ffffff,
    0x00000d00, 0x00400d00, 0x00800d00, 0x00c00d00, 0x00000e00, 0x00400e00,
    0x00800e00, 0x00c00e00, 0x00000f00, 0x00400f00, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00400000, 0x00800000, 0x00c00000, 0x00000100, 0x00400100, 0x00800100,
    0x00c00100, 0x00000200, 0x00400200, 0x00800200, 0x00c00200, 0x00000300,
    0x00400300, 0x00800300, 0x00c00300, 0x00000400, 0x00400400, 0x00800400,
    0x00c00400, 0x00000500, 0x00400500, 0x00800500, 0x00c00500, 0x00000600,
    0x00400600, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00c00f00,
    0x01ffffff, 0x00800600, 0x00c00600, 0x00000700, 0x00400700, 0x00800700,
    0x00c00700, 0x00000800, 0x00400800, 0x00800800, 0x00c00800, 0x00000900,
    0x00400900, 0x00800900, 0x00c00900, 0x00000a00, 0x00400a00, 0x00800a00,
    0x00c00a00, 0x00000b00, 0x00400b00, 0x00800b00, 0x00c00b00, 0x00000c00,
    0x00400c00, 0x00800c00, 0x00c00c00, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};
constexpr uint32_t d3[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x003e0000, 0x01ffffff, 0x01ffffff,
    0x00340000, 0x00350000, 0x00360000, 0x00370000, 0x00380000, 0x00390000,
    0x003a0000, 0x003b0000, 0x003c0000, 0x003d0000, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00010000, 0x00020000, 0x00030000, 0x00040000, 0x00050000, 0x00060000,
    0x00070000, 0x00080000, 0x00090000, 0x000a0000, 0x000b0000, 0x000c0000,
    0x000d0000, 0x000e0000, 0x000f0000, 0x00100000, 0x00110000, 0x00120000,
    0x00130000, 0x00140000, 0x00150000, 0x00160000, 0x00170000, 0x00180000,
    0x00190000, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x003f0000,
    0x01ffffff, 0x001a0000, 0x001b0000, 0x001c0000, 0x001d0000, 0x001e0000,
    0x001f0000, 0x00200000, 0x00210000, 0x00220000, 0x00230000, 0x00240000,
    0x00250000, 0x00260000, 0x00270000, 0x00280000, 0x00290000, 0x002a0000,
    0x002b0000, 0x002c0000, 0x002d0000, 0x002e0000, 0x002f0000, 0x00300000,
    0x00310000, 0x00320000, 0x00330000, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};
} // namespace base64_url

namespace base64_default_or_url {
constexpr uint32_t d0[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x000000f8, 0x01ffffff, 0x000000f8, 0x01ffffff, 0x000000fc,
    0x000000d0, 0x000000d4, 0x000000d8, 0x000000dc, 0x000000e0, 0x000000e4,
    0x000000e8, 0x000000ec, 0x000000f0, 0x000000f4, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00000004, 0x00000008, 0x0000000c, 0x00000010, 0x00000014, 0x00000018,
    0x0000001c, 0x00000020, 0x00000024, 0x00000028, 0x0000002c, 0x00000030,
    0x00000034, 0x00000038, 0x0000003c, 0x00000040, 0x00000044, 0x00000048,
    0x0000004c, 0x00000050, 0x00000054, 0x00000058, 0x0000005c, 0x00000060,
    0x00000064, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x000000fc,
    0x01ffffff, 0x00000068, 0x0000006c, 0x00000070, 0x00000074, 0x00000078,
    0x0000007c, 0x00000080, 0x00000084, 0x00000088, 0x0000008c, 0x00000090,
    0x00000094, 0x00000098, 0x0000009c, 0x000000a0, 0x000000a4, 0x000000a8,
    0x000000ac, 0x000000b0, 0x000000b4, 0x000000b8, 0x000000bc, 0x000000c0,
    0x000000c4, 0x000000c8, 0x000000cc, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};
constexpr uint32_t d1[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x0000e003, 0x01ffffff, 0x0000e003, 0x01ffffff, 0x0000f003,
    0x00004003, 0x00005003, 0x00006003, 0x00007003, 0x00008003, 0x00009003,
    0x0000a003, 0x0000b003, 0x0000c003, 0x0000d003, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00001000, 0x00002000, 0x00003000, 0x00004000, 0x00005000, 0x00006000,
    0x00007000, 0x00008000, 0x00009000, 0x0000a000, 0x0000b000, 0x0000c000,
    0x0000d000, 0x0000e000, 0x0000f000, 0x00000001, 0x00001001, 0x00002001,
    0x00003001, 0x00004001, 0x00005001, 0x00006001, 0x00007001, 0x00008001,
    0x00009001, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x0000f003,
    0x01ffffff, 0x0000a001, 0x0000b001, 0x0000c001, 0x0000d001, 0x0000e001,
    0x0000f001, 0x00000002, 0x00001002, 0x00002002, 0x00003002, 0x00004002,
    0x00005002, 0x00006002, 0x00007002, 0x00008002, 0x00009002, 0x0000a002,
    0x0000b002, 0x0000c002, 0x0000d002, 0x0000e002, 0x0000f002, 0x00000003,
    0x00001003, 0x00002003, 0x00003003, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};
constexpr uint32_t d2[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x00800f00, 0x01ffffff, 0x00800f00, 0x01ffffff, 0x00c00f00,
    0x00000d00, 0x00400d00, 0x00800d00, 0x00c00d00, 0x00000e00, 0x00400e00,
    0x00800e00, 0x00c00e00, 0x00000f00, 0x00400f00, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00400000, 0x00800000, 0x00c00000, 0x00000100, 0x00400100, 0x00800100,
    0x00c00100, 0x00000200, 0x00400200, 0x00800200, 0x00c00200, 0x00000300,
    0x00400300, 0x00800300, 0x00c00300, 0x00000400, 0x00400400, 0x00800400,
    0x00c00400, 0x00000500, 0x00400500, 0x00800500, 0x00c00500, 0x00000600,
    0x00400600, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00c00f00,
    0x01ffffff, 0x00800600, 0x00c00600, 0x00000700, 0x00400700, 0x00800700,
    0x00c00700, 0x00000800, 0x00400800, 0x00800800, 0x00c00800, 0x00000900,
    0x00400900, 0x00800900, 0x00c00900, 0x00000a00, 0x00400a00, 0x00800a00,
    0x00c00a00, 0x00000b00, 0x00400b00, 0x00800b00, 0x00c00b00, 0x00000c00,
    0x00400c00, 0x00800c00, 0x00c00c00, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};
constexpr uint32_t d3[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x003e0000, 0x01ffffff, 0x003e0000, 0x01ffffff, 0x003f0000,
    0x00340000, 0x00350000, 0x00360000, 0x00370000, 0x00380000, 0x00390000,
    0x003a0000, 0x003b0000, 0x003c0000, 0x003d0000, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00010000, 0x00020000, 0x00030000, 0x00040000, 0x00050000, 0x00060000,
    0x00070000, 0x00080000, 0x00090000, 0x000a0000, 0x000b0000, 0x000c0000,
    0x000d0000, 0x000e0000, 0x000f0000, 0x00100000, 0x00110000, 0x00120000,
    0x00130000, 0x00140000, 0x00150000, 0x00160000, 0x00170000, 0x00180000,
    0x00190000, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x003f0000,
    0x01ffffff, 0x001a0000, 0x001b0000, 0x001c0000, 0x001d0000, 0x001e0000,
    0x001f0000, 0x00200000, 0x00210000, 0x00220000, 0x00230000, 0x00240000,
    0x00250000, 0x00260000, 0x00270000, 0x00280000, 0x00290000, 0x002a0000,
    0x002b0000, 0x002c0000, 0x002d0000, 0x002e0000, 0x002f0000, 0x00300000,
    0x00310000, 0x00320000, 0x00330000, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff};
} // namespace base64_default_or_url
constexpr uint64_t thintable_epi8[256] = {
    0x0706050403020100, 0x0007060504030201, 0x0007060504030200,
    0x0000070605040302, 0x0007060504030100, 0x0000070605040301,
    0x0000070605040300, 0x0000000706050403, 0x0007060504020100,
    0x0000070605040201, 0x0000070605040200, 0x0000000706050402,
    0x0000070605040100, 0x0000000706050401, 0x0000000706050400,
    0x0000000007060504, 0x0007060503020100, 0x0000070605030201,
    0x0000070605030200, 0x0000000706050302, 0x0000070605030100,
    0x0000000706050301, 0x0000000706050300, 0x0000000007060503,
    0x0000070605020100, 0x0000000706050201, 0x0000000706050200,
    0x0000000007060502, 0x0000000706050100, 0x0000000007060501,
    0x0000000007060500, 0x0000000000070605, 0x0007060403020100,
    0x0000070604030201, 0x0000070604030200, 0x0000000706040302,
    0x0000070604030100, 0x0000000706040301, 0x0000000706040300,
    0x0000000007060403, 0x0000070604020100, 0x0000000706040201,
    0x0000000706040200, 0x0000000007060402, 0x0000000706040100,
    0x0000000007060401, 0x0000000007060400, 0x0000000000070604,
    0x0000070603020100, 0x0000000706030201, 0x0000000706030200,
    0x0000000007060302, 0x0000000706030100, 0x0000000007060301,
    0x0000000007060300, 0x0000000000070603, 0x0000000706020100,
    0x0000000007060201, 0x0000000007060200, 0x0000000000070602,
    0x0000000007060100, 0x0000000000070601, 0x0000000000070600,
    0x0000000000000706, 0x0007050403020100, 0x0000070504030201,
    0x0000070504030200, 0x0000000705040302, 0x0000070504030100,
    0x0000000705040301, 0x0000000705040300, 0x0000000007050403,
    0x0000070504020100, 0x0000000705040201, 0x0000000705040200,
    0x0000000007050402, 0x0000000705040100, 0x0000000007050401,
    0x0000000007050400, 0x0000000000070504, 0x0000070503020100,
    0x0000000705030201, 0x0000000705030200, 0x0000000007050302,
    0x0000000705030100, 0x0000000007050301, 0x0000000007050300,
    0x0000000000070503, 0x0000000705020100, 0x0000000007050201,
    0x0000000007050200, 0x0000000000070502, 0x0000000007050100,
    0x0000000000070501, 0x0000000000070500, 0x0000000000000705,
    0x0000070403020100, 0x0000000704030201, 0x0000000704030200,
    0x0000000007040302, 0x0000000704030100, 0x0000000007040301,
    0x0000000007040300, 0x0000000000070403, 0x0000000704020100,
    0x0000000007040201, 0x0000000007040200, 0x0000000000070402,
    0x0000000007040100, 0x0000000000070401, 0x0000000000070400,
    0x0000000000000704, 0x0000000703020100, 0x0000000007030201,
    0x0000000007030200, 0x0000000000070302, 0x0000000007030100,
    0x0000000000070301, 0x0000000000070300, 0x0000000000000703,
    0x0000000007020100, 0x0000000000070201, 0x0000000000070200,
    0x0000000000000702, 0x0000000000070100, 0x0000000000000701,
    0x0000000000000700, 0x0000000000000007, 0x0006050403020100,
    0x0000060504030201, 0x0000060504030200, 0x0000000605040302,
    0x0000060504030100, 0x0000000605040301, 0x0000000605040300,
    0x0000000006050403, 0x0000060504020100, 0x0000000605040201,
    0x0000000605040200, 0x0000000006050402, 0x0000000605040100,
    0x0000000006050401, 0x0000000006050400, 0x0000000000060504,
    0x0000060503020100, 0x0000000605030201, 0x0000000605030200,
    0x0000000006050302, 0x0000000605030100, 0x0000000006050301,
    0x0000000006050300, 0x0000000000060503, 0x0000000605020100,
    0x0000000006050201, 0x0000000006050200, 0x0000000000060502,
    0x0000000006050100, 0x0000000000060501, 0x0000000000060500,
    0x0000000000000605, 0x0000060403020100, 0x0000000604030201,
    0x0000000604030200, 0x0000000006040302, 0x0000000604030100,
    0x0000000006040301, 0x0000000006040300, 0x0000000000060403,
    0x0000000604020100, 0x0000000006040201, 0x0000000006040200,
    0x0000000000060402, 0x0000000006040100, 0x0000000000060401,
    0x0000000000060400, 0x0000000000000604, 0x0000000603020100,
    0x0000000006030201, 0x0000000006030200, 0x0000000000060302,
    0x0000000006030100, 0x0000000000060301, 0x0000000000060300,
    0x0000000000000603, 0x0000000006020100, 0x0000000000060201,
    0x0000000000060200, 0x0000000000000602, 0x0000000000060100,
    0x0000000000000601, 0x0000000000000600, 0x0000000000000006,
    0x0000050403020100, 0x0000000504030201, 0x0000000504030200,
    0x0000000005040302, 0x0000000504030100, 0x0000000005040301,
    0x0000000005040300, 0x0000000000050403, 0x0000000504020100,
    0x0000000005040201, 0x0000000005040200, 0x0000000000050402,
    0x0000000005040100, 0x0000000000050401, 0x0000000000050400,
    0x0000000000000504, 0x0000000503020100, 0x0000000005030201,
    0x0000000005030200, 0x0000000000050302, 0x0000000005030100,
    0x0000000000050301, 0x0000000000050300, 0x0000000000000503,
    0x0000000005020100, 0x0000000000050201, 0x0000000000050200,
    0x0000000000000502, 0x0000000000050100, 0x0000000000000501,
    0x0000000000000500, 0x0000000000000005, 0x0000000403020100,
    0x0000000004030201, 0x0000000004030200, 0x0000000000040302,
    0x0000000004030100, 0x0000000000040301, 0x0000000000040300,
    0x0000000000000403, 0x0000000004020100, 0x0000000000040201,
    0x0000000000040200, 0x0000000000000402, 0x0000000000040100,
    0x0000000000000401, 0x0000000000000400, 0x0000000000000004,
    0x0000000003020100, 0x0000000000030201, 0x0000000000030200,
    0x0000000000000302, 0x0000000000030100, 0x0000000000000301,
    0x0000000000000300, 0x0000000000000003, 0x0000000000020100,
    0x0000000000000201, 0x0000000000000200, 0x0000000000000002,
    0x0000000000000100, 0x0000000000000001, 0x0000000000000000,
    0x0000000000000000,
};

constexpr uint8_t pshufb_combine_table[272] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0e, 0x0f, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
    0x0f, 0xff, 0xff, 0xff, 0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01, 0x02, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x01, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x00, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
    0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

constexpr unsigned char BitsSetTable256mul2[256] = {
    0,  2,  2,  4,  2,  4,  4,  6,  2,  4,  4,  6,  4,  6,  6,  8,  2,  4,  4,
    6,  4,  6,  6,  8,  4,  6,  6,  8,  6,  8,  8,  10, 2,  4,  4,  6,  4,  6,
    6,  8,  4,  6,  6,  8,  6,  8,  8,  10, 4,  6,  6,  8,  6,  8,  8,  10, 6,
    8,  8,  10, 8,  10, 10, 12, 2,  4,  4,  6,  4,  6,  6,  8,  4,  6,  6,  8,
    6,  8,  8,  10, 4,  6,  6,  8,  6,  8,  8,  10, 6,  8,  8,  10, 8,  10, 10,
    12, 4,  6,  6,  8,  6,  8,  8,  10, 6,  8,  8,  10, 8,  10, 10, 12, 6,  8,
    8,  10, 8,  10, 10, 12, 8,  10, 10, 12, 10, 12, 12, 14, 2,  4,  4,  6,  4,
    6,  6,  8,  4,  6,  6,  8,  6,  8,  8,  10, 4,  6,  6,  8,  6,  8,  8,  10,
    6,  8,  8,  10, 8,  10, 10, 12, 4,  6,  6,  8,  6,  8,  8,  10, 6,  8,  8,
    10, 8,  10, 10, 12, 6,  8,  8,  10, 8,  10, 10, 12, 8,  10, 10, 12, 10, 12,
    12, 14, 4,  6,  6,  8,  6,  8,  8,  10, 6,  8,  8,  10, 8,  10, 10, 12, 6,
    8,  8,  10, 8,  10, 10, 12, 8,  10, 10, 12, 10, 12, 12, 14, 6,  8,  8,  10,
    8,  10, 10, 12, 8,  10, 10, 12, 10, 12, 12, 14, 8,  10, 10, 12, 10, 12, 12,
    14, 10, 12, 12, 14, 12, 14, 14, 16};

constexpr uint8_t to_base64_value[] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 64,  64,  255, 64,  64,  255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 64,  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 62,  255,
    255, 255, 63,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  255, 255,
    255, 255, 255, 255, 255, 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
    10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
    25,  255, 255, 255, 255, 255, 255, 26,  27,  28,  29,  30,  31,  32,  33,
    34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
    49,  50,  51,  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255};

constexpr uint8_t to_base64_url_value[] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 64,  64,  255, 64,  64,  255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 64,  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    62,  255, 255, 52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  255, 255,
    255, 255, 255, 255, 255, 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
    10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
    25,  255, 255, 255, 255, 63,  255, 26,  27,  28,  29,  30,  31,  32,  33,
    34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
    49,  50,  51,  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255};

constexpr uint8_t to_base64_default_or_url_value[] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 64,  64,  255, 64,  64,  255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 64,  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 62,  255,
    62,  255, 63,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  255, 255,
    255, 255, 255, 255, 255, 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
    10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
    25,  255, 255, 255, 255, 63,  255, 26,  27,  28,  29,  30,  31,  32,  33,
    34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
    49,  50,  51,  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255};

static_assert(sizeof(to_base64_value) == 256,
              "to_base64_value must have 256 elements");
static_assert(sizeof(to_base64_url_value) == 256,
              "to_base64_url_value must have 256 elements");
static_assert(to_base64_value[uint8_t(' ')] == 64,
              "space must be == 64 in to_base64_value");
static_assert(to_base64_url_value[uint8_t(' ')] == 64,
              "space must be == 64 in to_base64_url_value");
static_assert(to_base64_value[uint8_t('\t')] == 64,
              "tab must be == 64 in to_base64_value");
static_assert(to_base64_url_value[uint8_t('\t')] == 64,
              "tab must be == 64 in to_base64_url_value");
static_assert(to_base64_value[uint8_t('\r')] == 64,
              "cr must be == 64 in to_base64_value");
static_assert(to_base64_url_value[uint8_t('\r')] == 64,
              "cr must be == 64 in to_base64_url_value");
static_assert(to_base64_value[uint8_t('\n')] == 64,
              "lf must be == 64 in to_base64_value");
static_assert(to_base64_url_value[uint8_t('\n')] == 64,
              "lf must be == 64 in to_base64_url_value");
static_assert(to_base64_value[uint8_t('\f')] == 64,
              "ff must be == 64 in to_base64_value");
static_assert(to_base64_url_value[uint8_t('\f')] == 64,
              "ff must be == 64 in to_base64_url_value");
static_assert(to_base64_value[uint8_t('+')] == 62,
              "+ must be == 62 in to_base64_value");
static_assert(to_base64_url_value[uint8_t('-')] == 62,
              "- must be == 62 in to_base64_url_value");
static_assert(to_base64_value[uint8_t('/')] == 63,
              "/ must be == 63 in to_base64_value");
static_assert(to_base64_url_value[uint8_t('_')] == 63,
              "_ must be == 63 in to_base64_url_value");
} // namespace base64
} // namespace tables
} // unnamed namespace
} // namespace simdutf

#endif // SIMDUTF_BASE64_TABLES_H
/* end file include/simdutf/base64_tables.h */
/* begin file include/simdutf/scalar/base64.h */
#ifndef SIMDUTF_BASE64_H
#define SIMDUTF_BASE64_H

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace simdutf {
namespace scalar {
namespace {
namespace base64 {

// This function is not expected to be fast. Do not use in long loops.
// In most instances you should be using is_ignorable.
template <class char_type> bool is_ascii_white_space(char_type c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f';
}

template <class char_type> simdutf_constexpr23 bool is_eight_byte(char_type c) {
  if constexpr (sizeof(char_type) == 1) {
    return true;
  }
  return uint8_t(c) == c;
}

template <class char_type>
simdutf_constexpr23 bool is_ignorable(char_type c,
                                      simdutf::base64_options options) {
  const uint8_t *to_base64 =
      (options & base64_default_or_url)
          ? tables::base64::to_base64_default_or_url_value
          : ((options & base64_url) ? tables::base64::to_base64_url_value
                                    : tables::base64::to_base64_value);
  const bool ignore_garbage =
      (options == base64_options::base64_url_accept_garbage) ||
      (options == base64_options::base64_default_accept_garbage) ||
      (options == base64_options::base64_default_or_url_accept_garbage);
  uint8_t code = to_base64[uint8_t(c)];
  if (is_eight_byte(c) && code <= 63) {
    return false;
  }
  if (is_eight_byte(c) && code == 64) {
    return true;
  }
  return ignore_garbage;
}
template <class char_type>
simdutf_constexpr23 bool is_base64(char_type c,
                                   simdutf::base64_options options) {
  const uint8_t *to_base64 =
      (options & base64_default_or_url)
          ? tables::base64::to_base64_default_or_url_value
          : ((options & base64_url) ? tables::base64::to_base64_url_value
                                    : tables::base64::to_base64_value);
  uint8_t code = to_base64[uint8_t(c)];
  if (is_eight_byte(c) && code <= 63) {
    return true;
  }
  return false;
}

template <class char_type>
simdutf_constexpr23 bool is_base64_or_padding(char_type c,
                                              simdutf::base64_options options) {
  const uint8_t *to_base64 =
      (options & base64_default_or_url)
          ? tables::base64::to_base64_default_or_url_value
          : ((options & base64_url) ? tables::base64::to_base64_url_value
                                    : tables::base64::to_base64_value);
  if (c == '=') {
    return true;
  }
  uint8_t code = to_base64[uint8_t(c)];
  if (is_eight_byte(c) && code <= 63) {
    return true;
  }
  return false;
}

template <class char_type>
bool is_ignorable_or_padding(char_type c, simdutf::base64_options options) {
  return is_ignorable(c, options) || c == '=';
}

struct reduced_input {
  size_t equalsigns;    // number of padding characters '=', typically 0, 1, 2.
  size_t equallocation; // location of the first padding character if any
  size_t srclen;        // length of the input buffer before padding
  size_t full_input_length; // length of the input buffer with padding but
                            // without ignorable characters
};

// find the end of the base64 input buffer
// It returns the number of padding characters, the location of the first
// padding character if any, the length of the input buffer before padding
// and the length of the input buffer with padding. The input buffer is not
// modified. The function assumes that there are at most two padding characters.
template <class char_type>
simdutf_constexpr23 reduced_input find_end(const char_type *src, size_t srclen,
                                           simdutf::base64_options options) {
  const uint8_t *to_base64 =
      (options & base64_default_or_url)
          ? tables::base64::to_base64_default_or_url_value
          : ((options & base64_url) ? tables::base64::to_base64_url_value
                                    : tables::base64::to_base64_value);
  const bool ignore_garbage =
      (options == base64_options::base64_url_accept_garbage) ||
      (options == base64_options::base64_default_accept_garbage) ||
      (options == base64_options::base64_default_or_url_accept_garbage);

  size_t equalsigns = 0;
  // We intentionally include trailing spaces in the full input length.
  // See https://github.com/simdutf/simdutf/issues/824
  size_t full_input_length = srclen;
  // skip trailing spaces
  while (!ignore_garbage && srclen > 0 &&
         scalar::base64::is_eight_byte(src[srclen - 1]) &&
         to_base64[uint8_t(src[srclen - 1])] == 64) {
    srclen--;
  }
  size_t equallocation =
      srclen; // location of the first padding character if any
  if (ignore_garbage) {
    // Technically, we don't need to find the first padding character, we can
    // just change our algorithms, but it adds substantial complexity.
    auto it = simdutf::find(src, src + srclen, '=');
    if (it != src + srclen) {
      equallocation = it - src;
      equalsigns = 1;
      srclen = equallocation;
      full_input_length = equallocation + 1;
    }
    return {equalsigns, equallocation, srclen, full_input_length};
  }
  if (!ignore_garbage && srclen > 0 && src[srclen - 1] == '=') {
    // This is the last '=' sign.
    equallocation = srclen - 1;
    srclen--;
    equalsigns = 1;
    // skip trailing spaces
    while (srclen > 0 && scalar::base64::is_eight_byte(src[srclen - 1]) &&
           to_base64[uint8_t(src[srclen - 1])] == 64) {
      srclen--;
    }
    if (srclen > 0 && src[srclen - 1] == '=') {
      // This is the second '=' sign.
      equallocation = srclen - 1;
      srclen--;
      equalsigns = 2;
    }
  }
  return {equalsigns, equallocation, srclen, full_input_length};
}

// Returns true upon success. The destination buffer must be large enough.
// This functions assumes that the padding (=) has been removed.
// if check_capacity is true, it will check that the destination buffer is
// large enough. If it is not, it will return OUTPUT_BUFFER_TOO_SMALL.
template <bool check_capacity, class char_type>
simdutf_constexpr23 full_result base64_tail_decode_impl(
    char *dst, size_t outlen, const char_type *src, size_t length,
    size_t padding_characters, // number of padding characters
                               // '=', typically 0, 1, 2.
    base64_options options, last_chunk_handling_options last_chunk_options) {
  char *dstend = dst + outlen;
  (void)dstend;
  // This looks like 10 branches, but we expect the compiler to resolve this to
  // two branches (easily predicted):
  const uint8_t *to_base64 =
      (options & base64_default_or_url)
          ? tables::base64::to_base64_default_or_url_value
          : ((options & base64_url) ? tables::base64::to_base64_url_value
                                    : tables::base64::to_base64_value);
  const uint32_t *d0 =
      (options & base64_default_or_url)
          ? tables::base64::base64_default_or_url::d0
          : ((options & base64_url) ? tables::base64::base64_url::d0
                                    : tables::base64::base64_default::d0);
  const uint32_t *d1 =
      (options & base64_default_or_url)
          ? tables::base64::base64_default_or_url::d1
          : ((options & base64_url) ? tables::base64::base64_url::d1
                                    : tables::base64::base64_default::d1);
  const uint32_t *d2 =
      (options & base64_default_or_url)
          ? tables::base64::base64_default_or_url::d2
          : ((options & base64_url) ? tables::base64::base64_url::d2
                                    : tables::base64::base64_default::d2);
  const uint32_t *d3 =
      (options & base64_default_or_url)
          ? tables::base64::base64_default_or_url::d3
          : ((options & base64_url) ? tables::base64::base64_url::d3
                                    : tables::base64::base64_default::d3);
  const bool ignore_garbage =
      (options == base64_options::base64_url_accept_garbage) ||
      (options == base64_options::base64_default_accept_garbage) ||
      (options == base64_options::base64_default_or_url_accept_garbage);

  const char_type *srcend = src + length;
  const char_type *srcinit = src;
  const char *dstinit = dst;

  uint32_t x;
  size_t idx;
  uint8_t buffer[4];
  while (true) {
    while (srcend - src >= 4 && is_eight_byte(src[0]) &&
           is_eight_byte(src[1]) && is_eight_byte(src[2]) &&
           is_eight_byte(src[3]) &&
           (x = d0[uint8_t(src[0])] | d1[uint8_t(src[1])] |
                d2[uint8_t(src[2])] | d3[uint8_t(src[3])]) < 0x01FFFFFF) {
      if (check_capacity && dstend - dst < 3) {
        return {OUTPUT_BUFFER_TOO_SMALL, size_t(src - srcinit),
                size_t(dst - dstinit)};
      }
      *dst++ = static_cast<char>(x & 0xFF);
      *dst++ = static_cast<char>((x >> 8) & 0xFF);
      *dst++ = static_cast<char>((x >> 16) & 0xFF);
      src += 4;
    }
    const char_type *srccur = src;
    idx = 0;
    // we need at least four characters.
#ifdef __clang__
    // If possible, we read four characters at a time. (It is an optimization.)
    if (ignore_garbage && src + 4 <= srcend) {
      char_type c0 = src[0];
      char_type c1 = src[1];
      char_type c2 = src[2];
      char_type c3 = src[3];

      uint8_t code0 = to_base64[uint8_t(c0)];
      uint8_t code1 = to_base64[uint8_t(c1)];
      uint8_t code2 = to_base64[uint8_t(c2)];
      uint8_t code3 = to_base64[uint8_t(c3)];

      buffer[idx] = code0;
      idx += (is_eight_byte(c0) && code0 <= 63);
      buffer[idx] = code1;
      idx += (is_eight_byte(c1) && code1 <= 63);
      buffer[idx] = code2;
      idx += (is_eight_byte(c2) && code2 <= 63);
      buffer[idx] = code3;
      idx += (is_eight_byte(c3) && code3 <= 63);
      src += 4;
    }
#endif
    while ((idx < 4) && (src < srcend)) {
      char_type c = *src;

      uint8_t code = to_base64[uint8_t(c)];
      buffer[idx] = uint8_t(code);
      if (is_eight_byte(c) && code <= 63) {
        idx++;
      } else if (!ignore_garbage &&
                 (code > 64 || !scalar::base64::is_eight_byte(c))) {
        return {INVALID_BASE64_CHARACTER, size_t(src - srcinit),
                size_t(dst - dstinit)};
      } else {
        // We have a space or a newline or garbage. We ignore it.
      }
      src++;
    }
    if (idx != 4) {
      simdutf_log_assert(idx < 4, "idx should be less than 4");
      // We never should have that the number of base64 characters + the
      // number of padding characters is more than 4.
      if (!ignore_garbage && (idx + padding_characters > 4)) {
        return {INVALID_BASE64_CHARACTER, size_t(src - srcinit),
                size_t(dst - dstinit), true};
      }

      // The idea here is that in loose mode,
      // if there is padding at all, it must be used
      // to form 4-wise chunk. However, in loose mode,
      // we do accept no padding at all.
      if (!ignore_garbage &&
          last_chunk_options == last_chunk_handling_options::loose &&
          (idx >= 2) && padding_characters > 0 &&
          ((idx + padding_characters) & 3) != 0) {
        return {INVALID_BASE64_CHARACTER, size_t(src - srcinit),
                size_t(dst - dstinit), true};
      } else

        // The idea here is that in strict mode, we do not want to accept
        // incomplete base64 chunks. So if the chunk was otherwise valid, we
        // return BASE64_INPUT_REMAINDER.
        if (!ignore_garbage &&
            last_chunk_options == last_chunk_handling_options::strict &&
            (idx >= 2) && ((idx + padding_characters) & 3) != 0) {
          // The partial chunk was at src - idx
          return {BASE64_INPUT_REMAINDER, size_t(src - srcinit),
                  size_t(dst - dstinit), true};
        } else
          // If there is a partial chunk with insufficient padding, with
          // stop_before_partial, we need to just ignore it. In "only full"
          // mode, skip the minute there are padding characters.
          if ((last_chunk_options ==
                   last_chunk_handling_options::stop_before_partial &&
               (padding_characters + idx < 4) && (idx != 0) &&
               (idx >= 2 || padding_characters == 0)) ||
              (last_chunk_options ==
                   last_chunk_handling_options::only_full_chunks &&
               (idx >= 2 || padding_characters == 0))) {
            // partial means that we are *not* going to consume the read
            // characters. We need to rewind the src pointer.
            src = srccur;
            return {SUCCESS, size_t(src - srcinit), size_t(dst - dstinit)};
          } else {
            if (idx == 2) {
              uint32_t triple = (uint32_t(buffer[0]) << 3 * 6) +
                                (uint32_t(buffer[1]) << 2 * 6);
              if (!ignore_garbage &&
                  (last_chunk_options == last_chunk_handling_options::strict) &&
                  (triple & 0xffff)) {
                return {BASE64_EXTRA_BITS, size_t(src - srcinit),
                        size_t(dst - dstinit)};
              }
              if (check_capacity && dstend - dst < 1) {
                return {OUTPUT_BUFFER_TOO_SMALL, size_t(srccur - srcinit),
                        size_t(dst - dstinit)};
              }
              *dst++ = static_cast<char>((triple >> 16) & 0xFF);
            } else if (idx == 3) {
              uint32_t triple = (uint32_t(buffer[0]) << 3 * 6) +
                                (uint32_t(buffer[1]) << 2 * 6) +
                                (uint32_t(buffer[2]) << 1 * 6);
              if (!ignore_garbage &&
                  (last_chunk_options == last_chunk_handling_options::strict) &&
                  (triple & 0xff)) {
                return {BASE64_EXTRA_BITS, size_t(src - srcinit),
                        size_t(dst - dstinit)};
              }
              if (check_capacity && dstend - dst < 2) {
                return {OUTPUT_BUFFER_TOO_SMALL, size_t(srccur - srcinit),
                        size_t(dst - dstinit)};
              }
              *dst++ = static_cast<char>((triple >> 16) & 0xFF);
              *dst++ = static_cast<char>((triple >> 8) & 0xFF);
            } else if (!ignore_garbage && idx == 1 &&
                       (!is_partial(last_chunk_options) ||
                        (is_partial(last_chunk_options) &&
                         padding_characters > 0))) {
              return {BASE64_INPUT_REMAINDER, size_t(src - srcinit),
                      size_t(dst - dstinit)};
            } else if (!ignore_garbage && idx == 0 && padding_characters > 0) {
              return {INVALID_BASE64_CHARACTER, size_t(src - srcinit),
                      size_t(dst - dstinit), true};
            }
            return {SUCCESS, size_t(src - srcinit), size_t(dst - dstinit)};
          }
    }
    if (check_capacity && dstend - dst < 3) {
      return {OUTPUT_BUFFER_TOO_SMALL, size_t(srccur - srcinit),
              size_t(dst - dstinit)};
    }
    uint32_t triple =
        (uint32_t(buffer[0]) << 3 * 6) + (uint32_t(buffer[1]) << 2 * 6) +
        (uint32_t(buffer[2]) << 1 * 6) + (uint32_t(buffer[3]) << 0 * 6);
    *dst++ = static_cast<char>((triple >> 16) & 0xFF);
    *dst++ = static_cast<char>((triple >> 8) & 0xFF);
    *dst++ = static_cast<char>(triple & 0xFF);
  }
}

template <class char_type>
simdutf_constexpr23 full_result base64_tail_decode(
    char *dst, const char_type *src, size_t length,
    size_t padding_characters, // number of padding characters
                               // '=', typically 0, 1, 2.
    base64_options options, last_chunk_handling_options last_chunk_options) {
  return base64_tail_decode_impl<false>(dst, 0, src, length, padding_characters,
                                        options, last_chunk_options);
}

// like base64_tail_decode, but it will not write past the end of the output
// buffer. The outlen parameter is modified to reflect the number of bytes
// written. This functions assumes that the padding (=) has been removed.
//
template <class char_type>
simdutf_constexpr23 full_result base64_tail_decode_safe(
    char *dst, size_t outlen, const char_type *src, size_t length,
    size_t padding_characters, // number of padding characters
                               // '=', typically 0, 1, 2.
    base64_options options, last_chunk_handling_options last_chunk_options) {
  return base64_tail_decode_impl<true>(dst, outlen, src, length,
                                       padding_characters, options,
                                       last_chunk_options);
}

inline simdutf_constexpr23 full_result
patch_tail_result(full_result r, size_t previous_input, size_t previous_output,
                  size_t equallocation, size_t full_input_length,
                  last_chunk_handling_options last_chunk_options) {
  r.input_count += previous_input;
  r.output_count += previous_output;
  if (r.padding_error) {
    r.input_count = equallocation;
  }

  if (r.error == error_code::SUCCESS) {
    if (!is_partial(last_chunk_options)) {
      // A success when we are not in stop_before_partial mode.
      // means that we have consumed the whole input buffer.
      r.input_count = full_input_length;
    } else if (r.output_count % 3 != 0) {
      r.input_count = full_input_length;
    }
  }
  return r;
}

// Returns the number of bytes written. The destination buffer must be large
// enough. It will add padding (=) if needed.
template <bool use_lines = false>
simdutf_constexpr23 size_t tail_encode_base64_impl(
    char *dst, const char *src, size_t srclen, base64_options options,
    size_t line_length = simdutf::default_line_length, size_t line_offset = 0) {
  if constexpr (use_lines) {
    // sanitize line_length and starting_line_offset.
    // line_length must be greater than 3.
    if (line_length < 4) {
      line_length = 4;
    }
    simdutf_log_assert(line_offset <= line_length,
                       "line_offset should be less than line_length");
  }
  // By default, we use padding if we are not using the URL variant.
  // This is check with ((options & base64_url) == 0) which returns true if we
  // are not using the URL variant. However, we also allow 'inversion' of the
  // convention with the base64_reverse_padding option. If the
  // base64_reverse_padding option is set, we use padding if we are using the
  // URL variant, and we omit it if we are not using the URL variant. This is
  // checked with
  // ((options & base64_reverse_padding) == base64_reverse_padding).
  bool use_padding =
      ((options & base64_url) == 0) ^
      ((options & base64_reverse_padding) == base64_reverse_padding);
  // This looks like 3 branches, but we expect the compiler to resolve this to
  // a single branch:
  const char *e0 = (options & base64_url) ? tables::base64::base64_url::e0
                                          : tables::base64::base64_default::e0;
  const char *e1 = (options & base64_url) ? tables::base64::base64_url::e1
                                          : tables::base64::base64_default::e1;
  const char *e2 = (options & base64_url) ? tables::base64::base64_url::e2
                                          : tables::base64::base64_default::e2;
  char *out = dst;
  size_t i = 0;
  uint8_t t1, t2, t3;
  for (; i + 2 < srclen; i += 3) {
    t1 = uint8_t(src[i]);
    t2 = uint8_t(src[i + 1]);
    t3 = uint8_t(src[i + 2]);
    if constexpr (use_lines) {
      if (line_offset + 3 >= line_length) {
        if (line_offset == line_length) {
          *out++ = '\n';
          *out++ = e0[t1];
          *out++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
          *out++ = e1[((t2 & 0x0F) << 2) | ((t3 >> 6) & 0x03)];
          *out++ = e2[t3];
          line_offset = 4;
        } else if (line_offset + 1 == line_length) {
          *out++ = e0[t1];
          *out++ = '\n';
          *out++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
          *out++ = e1[((t2 & 0x0F) << 2) | ((t3 >> 6) & 0x03)];
          *out++ = e2[t3];
          line_offset = 3;
        } else if (line_offset + 2 == line_length) {
          *out++ = e0[t1];
          *out++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
          *out++ = '\n';
          *out++ = e1[((t2 & 0x0F) << 2) | ((t3 >> 6) & 0x03)];
          *out++ = e2[t3];
          line_offset = 2;
        } else if (line_offset + 3 == line_length) {
          *out++ = e0[t1];
          *out++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
          *out++ = e1[((t2 & 0x0F) << 2) | ((t3 >> 6) & 0x03)];
          *out++ = '\n';
          *out++ = e2[t3];
          line_offset = 1;
        }
      } else {
        *out++ = e0[t1];
        *out++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
        *out++ = e1[((t2 & 0x0F) << 2) | ((t3 >> 6) & 0x03)];
        *out++ = e2[t3];
        line_offset += 4;
      }
    } else {
      *out++ = e0[t1];
      *out++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
      *out++ = e1[((t2 & 0x0F) << 2) | ((t3 >> 6) & 0x03)];
      *out++ = e2[t3];
    }
  }
  switch (srclen - i) {
  case 0:
    break;
  case 1:
    t1 = uint8_t(src[i]);
    if constexpr (use_lines) {
      if (use_padding) {
        if (line_offset + 3 >= line_length) {
          if (line_offset == line_length) {
            *out++ = '\n';
            *out++ = e0[t1];
            *out++ = e1[(t1 & 0x03) << 4];
            *out++ = '=';
            *out++ = '=';
          } else if (line_offset + 1 == line_length) {
            *out++ = e0[t1];
            *out++ = '\n';
            *out++ = e1[(t1 & 0x03) << 4];
            *out++ = '=';
            *out++ = '=';
          } else if (line_offset + 2 == line_length) {
            *out++ = e0[t1];
            *out++ = e1[(t1 & 0x03) << 4];
            *out++ = '\n';
            *out++ = '=';
            *out++ = '=';
          } else if (line_offset + 3 == line_length) {
            *out++ = e0[t1];
            *out++ = e1[(t1 & 0x03) << 4];
            *out++ = '=';
            *out++ = '\n';
            *out++ = '=';
          }
        } else {
          *out++ = e0[t1];
          *out++ = e1[(t1 & 0x03) << 4];
          *out++ = '=';
          *out++ = '=';
        }
      } else {
        if (line_offset + 2 >= line_length) {
          if (line_offset == line_length) {
            *out++ = '\n';
            *out++ = e0[uint8_t(src[i])];
            *out++ = e1[(uint8_t(src[i]) & 0x03) << 4];
          } else if (line_offset + 1 == line_length) {
            *out++ = e0[uint8_t(src[i])];
            *out++ = '\n';
            *out++ = e1[(uint8_t(src[i]) & 0x03) << 4];
          } else {
            *out++ = e0[uint8_t(src[i])];
            *out++ = e1[(uint8_t(src[i]) & 0x03) << 4];
            // *out++ = '\n'; ==> no newline at the end of the output
          }
        } else {
          *out++ = e0[uint8_t(src[i])];
          *out++ = e1[(uint8_t(src[i]) & 0x03) << 4];
        }
      }
    } else {
      *out++ = e0[t1];
      *out++ = e1[(t1 & 0x03) << 4];
      if (use_padding) {
        *out++ = '=';
        *out++ = '=';
      }
    }
    break;
  default: /* case 2 */
    t1 = uint8_t(src[i]);
    t2 = uint8_t(src[i + 1]);
    if constexpr (use_lines) {
      if (use_padding) {
        if (line_offset + 3 >= line_length) {
          if (line_offset == line_length) {
            *out++ = '\n';
            *out++ = e0[t1];
            *out++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
            *out++ = e2[(t2 & 0x0F) << 2];
            *out++ = '=';
          } else if (line_offset + 1 == line_length) {
            *out++ = e0[t1];
            *out++ = '\n';
            *out++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
            *out++ = e2[(t2 & 0x0F) << 2];
            *out++ = '=';
          } else if (line_offset + 2 == line_length) {
            *out++ = e0[t1];
            *out++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
            *out++ = '\n';
            *out++ = e2[(t2 & 0x0F) << 2];
            *out++ = '=';
          } else if (line_offset + 3 == line_length) {
            *out++ = e0[t1];
            *out++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
            *out++ = e2[(t2 & 0x0F) << 2];
            *out++ = '\n';
            *out++ = '=';
          }
        } else {
          *out++ = e0[t1];
          *out++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
          *out++ = e2[(t2 & 0x0F) << 2];
          *out++ = '=';
        }
      } else {
        if (line_offset + 3 >= line_length) {
          if (line_offset == line_length) {
            *out++ = '\n';
            *out++ = e0[t1];
            *out++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
            *out++ = e2[(t2 & 0x0F) << 2];
          } else if (line_offset + 1 == line_length) {
            *out++ = e0[t1];
            *out++ = '\n';
            *out++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
            *out++ = e2[(t2 & 0x0F) << 2];
          } else if (line_offset + 2 == line_length) {
            *out++ = e0[t1];
            *out++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
            *out++ = '\n';
            *out++ = e2[(t2 & 0x0F) << 2];
          } else {
            *out++ = e0[t1];
            *out++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
            *out++ = e2[(t2 & 0x0F) << 2];
            // *out++ = '\n'; ==> no newline at the end of the output
          }
        } else {
          *out++ = e0[t1];
          *out++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
          *out++ = e2[(t2 & 0x0F) << 2];
        }
      }
    } else {
      *out++ = e0[t1];
      *out++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
      *out++ = e2[(t2 & 0x0F) << 2];
      if (use_padding) {
        *out++ = '=';
      }
    }
  }
  return (size_t)(out - dst);
}

// Returns the number of bytes written. The destination buffer must be large
// enough. It will add padding (=) if needed.
inline simdutf_constexpr23 size_t tail_encode_base64(char *dst, const char *src,
                                                     size_t srclen,
                                                     base64_options options) {
  return tail_encode_base64_impl(dst, src, srclen, options);
}

template <class InputPtr>
simdutf_warn_unused simdutf_constexpr23 size_t
maximal_binary_length_from_base64(InputPtr input, size_t length) noexcept {
  // We process the padding characters ('=') at the end to make sure
  // that we return an exact result when the input has no ignorable characters
  // (e.g., spaces).
  size_t padding = 0;
  if (length > 0) {
    if (input[length - 1] == '=') {
      padding++;
      if (length > 1 && input[length - 2] == '=') {
        padding++;
      }
    }
  }
  // The input is not otherwise processed for ignorable characters or
  // validation, so that the function runs in constant time (very fast). In
  // practice, base64 inputs without ignorable characters are common and the
  // common case are line separated inputs with relatively long lines (e.g., 76
  // characters) which leads this function to a slight (1%) overestimation of
  // the output size.
  //
  // Of course, some inputs might contain an arbitrary number of spaces or
  // newlines, which would make this function return a very pessimistic output
  // size but systems that produce base64 outputs typically do not do that and
  // if they do, they do not care much about minimizing memory usage.
  //
  // In specialized applications, users may know that their input is line
  // separated, which can be checked very quickly by by iterating (e.g., over 76
  // character chunks, looking for the linefeed characters only). We could
  // provide a specialized function for that, but it is not clear that the added
  // complexity is worth it for us.
  //
  size_t actual_length = length - padding;
  if (actual_length % 4 <= 1) {
    return actual_length / 4 * 3;
  }
  // if we have a valid input, then the remainder must be 2 or 3 adding one or
  // two extra bytes.
  return actual_length / 4 * 3 + (actual_length % 4) - 1;
}

// This function computes the binary length by iterating through the input
// and counting non-whitespace characters (excluding padding characters).
// We use a simple check (c > ' ') which is easy to parallelize and matches
// SIMD behavior. Only the last few characters are checked for padding '='.
template <class char_type>
simdutf_warn_unused simdutf_constexpr23 size_t
binary_length_from_base64(const char_type *input, size_t length) noexcept {
  // Count non-whitespace characters (c > ' ') with loop unrolling
  size_t count = 0;
  for (size_t i = 0; i < length; i++) {
    count += (input[i] > ' ');
  }

  // Check for padding '=' at the end (at most 2 padding characters)
  // Scan backwards, skipping whitespace, to find padding
  size_t padding = 0;
  size_t pos = length;
  // Skip trailing whitespace
  while (pos > 0 && padding < 2) {
    char_type c = input[--pos];
    if (c == '=') {
      padding++;
    } else if (c > ' ') {
      break;
    }
  }
  return ((count - padding) * 3) / 4;
}

template <typename char_type>
simdutf_warn_unused simdutf_constexpr23 full_result
base64_to_binary_details_impl(
    const char_type *input, size_t length, char *output, base64_options options,
    last_chunk_handling_options last_chunk_options) noexcept {
  const bool ignore_garbage =
      (options == base64_options::base64_url_accept_garbage) ||
      (options == base64_options::base64_default_accept_garbage) ||
      (options == base64_options::base64_default_or_url_accept_garbage);
  auto ri = simdutf::scalar::base64::find_end(input, length, options);
  size_t equallocation = ri.equallocation;
  size_t equalsigns = ri.equalsigns;
  length = ri.srclen;
  size_t full_input_length = ri.full_input_length;
  if (length == 0) {
    if (!ignore_garbage && equalsigns > 0) {
      return {INVALID_BASE64_CHARACTER, equallocation, 0, true};
    }
    return {SUCCESS, full_input_length, 0};
  }
  full_result r = scalar::base64::base64_tail_decode(
      output, input, length, equalsigns, options, last_chunk_options);
  r = scalar::base64::patch_tail_result(r, 0, 0, equallocation,
                                        full_input_length, last_chunk_options);
  if (!is_partial(last_chunk_options) && r.error == error_code::SUCCESS &&
      equalsigns > 0 && !ignore_garbage) {
    // additional checks
    if ((r.output_count % 3 == 0) ||
        ((r.output_count % 3) + 1 + equalsigns != 4)) {
      return {INVALID_BASE64_CHARACTER, equallocation, r.output_count, true};
    }
  }
  // When is_partial(last_chunk_options) is true, we must either end with
  // the end of the stream (beyond whitespace) or right after a non-ignorable
  // character or at the very beginning of the stream.
  // See https://tc39.es/proposal-arraybuffer-base64/spec/#sec-frombase64
  if (is_partial(last_chunk_options) && r.error == error_code::SUCCESS &&
      r.input_count < full_input_length) {
    // First check if we can extend the input to the end of the stream
    while (r.input_count < full_input_length &&
           base64_ignorable(*(input + r.input_count), options)) {
      r.input_count++;
    }
    // If we are still not at the end of the stream, then we must backtrack
    // to the last non-ignorable character.
    if (r.input_count < full_input_length) {
      while (r.input_count > 0 &&
             base64_ignorable(*(input + r.input_count - 1), options)) {
        r.input_count--;
      }
    }
  }
  return r;
}

template <typename char_type>
simdutf_constexpr23 simdutf_warn_unused full_result
base64_to_binary_details_safe_impl(
    const char_type *input, size_t length, char *output, size_t outlen,
    base64_options options,
    last_chunk_handling_options last_chunk_options) noexcept {
  const bool ignore_garbage =
      (options == base64_options::base64_url_accept_garbage) ||
      (options == base64_options::base64_default_accept_garbage) ||
      (options == base64_options::base64_default_or_url_accept_garbage);
  auto ri = simdutf::scalar::base64::find_end(input, length, options);
  size_t equallocation = ri.equallocation;
  size_t equalsigns = ri.equalsigns;
  length = ri.srclen;
  size_t full_input_length = ri.full_input_length;
  if (length == 0) {
    if (!ignore_garbage && equalsigns > 0) {
      return {INVALID_BASE64_CHARACTER, equallocation, 0};
    }
    return {SUCCESS, full_input_length, 0};
  }
  full_result r = scalar::base64::base64_tail_decode_safe(
      output, outlen, input, length, equalsigns, options, last_chunk_options);
  r = scalar::base64::patch_tail_result(r, 0, 0, equallocation,
                                        full_input_length, last_chunk_options);
  if (!is_partial(last_chunk_options) && r.error == error_code::SUCCESS &&
      equalsigns > 0 && !ignore_garbage) {
    // additional checks
    if ((r.output_count % 3 == 0) ||
        ((r.output_count % 3) + 1 + equalsigns != 4)) {
      return {INVALID_BASE64_CHARACTER, equallocation, r.output_count};
    }
  }

  // When is_partial(last_chunk_options) is true, we must either end with
  // the end of the stream (beyond whitespace) or right after a non-ignorable
  // character or at the very beginning of the stream.
  // See https://tc39.es/proposal-arraybuffer-base64/spec/#sec-frombase64
  if (is_partial(last_chunk_options) && r.error == error_code::SUCCESS &&
      r.input_count < full_input_length) {
    // First check if we can extend the input to the end of the stream
    while (r.input_count < full_input_length &&
           base64_ignorable(*(input + r.input_count), options)) {
      r.input_count++;
    }
    // If we are still not at the end of the stream, then we must backtrack
    // to the last non-ignorable character.
    if (r.input_count < full_input_length) {
      while (r.input_count > 0 &&
             base64_ignorable(*(input + r.input_count - 1), options)) {
        r.input_count--;
      }
    }
  }
  return r;
}

simdutf_warn_unused simdutf_constexpr23 size_t
base64_length_from_binary(size_t length, base64_options options) noexcept {
  // By default, we use padding if we are not using the URL variant.
  // This is check with ((options & base64_url) == 0) which returns true if we
  // are not using the URL variant. However, we also allow 'inversion' of the
  // convention with the base64_reverse_padding option. If the
  // base64_reverse_padding option is set, we use padding if we are using the
  // URL variant, and we omit it if we are not using the URL variant. This is
  // checked with
  // ((options & base64_reverse_padding) == base64_reverse_padding).
  bool use_padding =
      ((options & base64_url) == 0) ^
      ((options & base64_reverse_padding) == base64_reverse_padding);
  if (!use_padding) {
    return length / 3 * 4 + ((length % 3) ? (length % 3) + 1 : 0);
  }
  return (length + 2) / 3 *
         4; // We use padding to make the length a multiple of 4.
}

simdutf_warn_unused simdutf_constexpr23 size_t
base64_length_from_binary_with_lines(size_t length, base64_options options,
                                     size_t line_length) noexcept {
  if (length == 0) {
    return 0;
  }
  size_t base64_length =
      scalar::base64::base64_length_from_binary(length, options);
  if (line_length < 4) {
    line_length = 4;
  }
  size_t lines =
      (base64_length + line_length - 1) / line_length; // number of lines
  return base64_length + lines - 1;
}

// Return the length of the prefix that contains count base64 characters.
// Thus, if count is 3, the function returns the length of the prefix
// that contains 3 base64 characters.
// The function returns (size_t)-1 if there is not enough base64 characters in
// the input.
template <typename char_type>
simdutf_warn_unused size_t prefix_length(size_t count,
                                         simdutf::base64_options options,
                                         const char_type *input,
                                         size_t length) noexcept {
  size_t i = 0;
  while (i < length && is_ignorable(input[i], options)) {
    i++;
  }
  if (count == 0) {
    return i; // duh!
  }
  for (; i < length; i++) {
    if (is_ignorable(input[i], options)) {
      continue;
    }
    // We have a base64 character or a padding character.
    count--;
    if (count == 0) {
      return i + 1;
    }
  }
  simdutf_log_assert(false, "You never get here");

  return -1; // should never happen
}

} // namespace base64
} // unnamed namespace
} // namespace scalar
} // namespace simdutf

#endif
/* end file include/simdutf/scalar/base64.h */

namespace simdutf {

inline std::string_view to_string(base64_options options) {
  switch (options) {
  case base64_default:
    return "base64_default";
  case base64_url:
    return "base64_url";
  case base64_reverse_padding:
    return "base64_reverse_padding";
  case base64_url_with_padding:
    return "base64_url_with_padding";
  case base64_default_accept_garbage:
    return "base64_default_accept_garbage";
  case base64_url_accept_garbage:
    return "base64_url_accept_garbage";
  case base64_default_or_url:
    return "base64_default_or_url";
  case base64_default_or_url_accept_garbage:
    return "base64_default_or_url_accept_garbage";
  }
  return "<unknown>";
}

inline std::string_view to_string(last_chunk_handling_options options) {
  switch (options) {
  case loose:
    return "loose";
  case strict:
    return "strict";
  case stop_before_partial:
    return "stop_before_partial";
  case only_full_chunks:
    return "only_full_chunks";
  }
  return "<unknown>";
}

/**
 * Provide the maximal binary length in bytes given the base64 input.
 * As long as the input does not contain ignorable characters (e.g., ASCII
 * spaces or linefeed characters), the result is exact. In particular, the
 * function checks for padding characters.
 *
 * The function is fast (constant time). It checks up to two characters at
 * the end of the string. The input is not otherwise validated or read.
 *
 * @param input         the base64 input to process
 * @param length        the length of the base64 input in bytes
 * @return maximum number of binary bytes
 */
simdutf_warn_unused size_t
maximal_binary_length_from_base64(const char *input, size_t length) noexcept;
  #if SIMDUTF_SPAN
simdutf_really_inline simdutf_warn_unused simdutf_constexpr23 size_t
maximal_binary_length_from_base64(
    const detail::input_span_of_byte_like auto &input) noexcept {
    #if SIMDUTF_CPLUSPLUS23
  if consteval {
    return scalar::base64::maximal_binary_length_from_base64(
        detail::constexpr_cast_ptr<uint8_t>(input.data()), input.size());
  } else
    #endif
  {
    return maximal_binary_length_from_base64(
        reinterpret_cast<const char *>(input.data()), input.size());
  }
}
  #endif // SIMDUTF_SPAN

/**
 * Provide the maximal binary length in bytes given the base64 input.
 * As long as the input does not contain ignorable characters (e.g., ASCII
 * spaces or linefeed characters), the result is exact. In particular, the
 * function checks for padding characters.
 *
 * The function is fast (constant time). It checks up to two characters at
 * the end of the string. The input is not otherwise validated or read.
 *
 * @param input         the base64 input to process, in ASCII stored as 16-bit
 * units
 * @param length        the length of the base64 input in 16-bit units
 * @return maximal number of binary bytes
 */
simdutf_warn_unused size_t maximal_binary_length_from_base64(
    const char16_t *input, size_t length) noexcept;
  #if SIMDUTF_SPAN
simdutf_really_inline simdutf_warn_unused simdutf_constexpr23 size_t
maximal_binary_length_from_base64(std::span<const char16_t> input) noexcept {
    #if SIMDUTF_CPLUSPLUS23
  if consteval {
    return scalar::base64::maximal_binary_length_from_base64(input.data(),
                                                             input.size());
  } else
    #endif
  {
    return maximal_binary_length_from_base64(input.data(), input.size());
  }
}
  #endif // SIMDUTF_SPAN

/**
 * Compute the binary length from a base64 input.
 * This function is useful for base64 inputs that may contain ASCII whitespaces
 * (such as line breaks). For such inputs, the result is exact, and for any
 * inputs the result can be used to size the output buffer passed to
 * `base64_to_binary`.
 *
 * The function ignores whitespace and does not require padding characters
 * ('=').
 *
 * @param input         the base64 input to process
 * @param length        the length of the base64 input in bytes
 * @return number of binary bytes
 */
simdutf_warn_unused size_t binary_length_from_base64(const char *input,
                                                     size_t length) noexcept;
  #if SIMDUTF_SPAN
simdutf_really_inline simdutf_warn_unused simdutf_constexpr23 size_t
binary_length_from_base64(
    const detail::input_span_of_byte_like auto &input) noexcept {
    #if SIMDUTF_CPLUSPLUS23
  if consteval {
    return scalar::base64::binary_length_from_base64(input.data(),
                                                     input.size());
  } else
    #endif
  {
    return binary_length_from_base64(
        reinterpret_cast<const char *>(input.data()), input.size());
  }
}
  #endif // SIMDUTF_SPAN

/**
 * Compute the binary length from a base64 input.
 * This function is useful for base64 inputs that may contain ASCII whitespaces
 * (such as line breaks). For such inputs, the result is exact, and for any
 * inputs the result can be used to size the output buffer passed to
 * `base64_to_binary`.
 *
 * The function ignores whitespace and does not require padding characters
 * ('=').
 *
 * @param input         the base64 input to process, in ASCII stored as 16-bit
 * units
 * @param length        the length of the base64 input in 16-bit units
 * @return number of binary bytes
 */
simdutf_warn_unused size_t binary_length_from_base64(const char16_t *input,
                                                     size_t length) noexcept;
  #if SIMDUTF_SPAN
simdutf_really_inline simdutf_warn_unused simdutf_constexpr23 size_t
binary_length_from_base64(std::span<const char16_t> input) noexcept {
    #if SIMDUTF_CPLUSPLUS23
  if consteval {
    return scalar::base64::binary_length_from_base64(input.data(),
                                                     input.size());
  } else
    #endif
  {
    return binary_length_from_base64(input.data(), input.size());
  }
}
  #endif // SIMDUTF_SPAN

/**
 * Convert a base64 input to a binary output.
 *
 * This function follows the WHATWG forgiving-base64 format, which means that it
 * will ignore any ASCII spaces in the input. You may provide a padded input
 * (with one or two equal signs at the end) or an unpadded input (without any
 * equal signs at the end).
 *
 * See https://infra.spec.whatwg.org/#forgiving-base64-decode
 *
 * This function will fail in case of invalid input. When last_chunk_options =
 * loose, there are two possible reasons for failure: the input contains a
 * number of base64 characters that when divided by 4, leaves a single remainder
 * character (BASE64_INPUT_REMAINDER), or the input contains a character that is
 * not a valid base64 character (INVALID_BASE64_CHARACTER).
 *
 * When the error is INVALID_BASE64_CHARACTER, r.count contains the index in the
 * input where the invalid character was found. When the error is
 * BASE64_INPUT_REMAINDER, then r.count contains the number of bytes decoded.
 *
 * The default option (simdutf::base64_default) expects the characters `+` and
 * `/` as part of its alphabet. The URL option (simdutf::base64_url) expects the
 * characters `-` and `_` as part of its alphabet.
 *
 * The padding (`=`) is validated if present. There may be at most two padding
 * characters at the end of the input. If there are any padding characters, the
 * total number of characters (excluding spaces but including padding
 * characters) must be divisible by four.
 *
 * You should call this function with a buffer that is at least
 * maximal_binary_length_from_base64(input, length) bytes long. If you fail to
 * provide that much space, the function may cause a buffer overflow.
 *
 * Advanced users may want to tailor how the last chunk is handled. By default,
 * we use a loose (forgiving) approach but we also support a strict approach
 * as well as a stop_before_partial approach, as per the following proposal:
 *
 * https://tc39.es/proposal-arraybuffer-base64/spec/#sec-frombase64
 *
 * @param input         the base64 string to process
 * @param length        the length of the string in bytes
 * @param output        the pointer to a buffer that can hold the conversion
 * result (should be at least maximal_binary_length_from_base64(input, length)
 * bytes long).
 * @param options       the base64 options to use, usually base64_default or
 * base64_url, and base64_default by default.
 * @param last_chunk_options the last chunk handling options,
 * last_chunk_handling_options::loose by default
 * but can also be last_chunk_handling_options::strict or
 * last_chunk_handling_options::stop_before_partial.
 * @return a result pair struct (of type simdutf::result containing the two
 * fields error and count) with an error code and either position of the error
 * (in the input in bytes) if any, or the number of bytes written if successful.
 */
simdutf_warn_unused result base64_to_binary(
    const char *input, size_t length, char *output,
    base64_options options = base64_default,
    last_chunk_handling_options last_chunk_options = loose) noexcept;
  #if SIMDUTF_SPAN
simdutf_really_inline simdutf_warn_unused simdutf_constexpr23 result
base64_to_binary(
    const detail::input_span_of_byte_like auto &input,
    detail::output_span_of_byte_like auto &&binary_output,
    base64_options options = base64_default,
    last_chunk_handling_options last_chunk_options = loose) noexcept {
    #if SIMDUTF_CPLUSPLUS23
  if consteval {
    return scalar::base64::base64_to_binary_details_impl(
        input.data(), input.size(), binary_output.data(), options,
        last_chunk_options);
  } else
    #endif
  {
    return base64_to_binary(reinterpret_cast<const char *>(input.data()),
                            input.size(),
                            reinterpret_cast<char *>(binary_output.data()),
                            options, last_chunk_options);
  }
}
  #endif // SIMDUTF_SPAN

/**
 * Provide the base64 length in bytes given the length of a binary input.
 *
 * @param length        the length of the input in bytes
 * @return number of base64 bytes
 */
inline simdutf_warn_unused simdutf_constexpr23 size_t base64_length_from_binary(
    size_t length, base64_options options = base64_default) noexcept {
  return scalar::base64::base64_length_from_binary(length, options);
}

/**
 * Provide the base64 length in bytes given the length of a binary input,
 * taking into account line breaks.
 *
 * @param length        the length of the input in bytes
 * @param line_length   the length of lines, must be at least 4 (otherwise it is
 * interpreted as 4),
 * @return number of base64 bytes
 */
inline simdutf_warn_unused simdutf_constexpr23 size_t
base64_length_from_binary_with_lines(
    size_t length, base64_options options = base64_default,
    size_t line_length = default_line_length) noexcept {
  return scalar::base64::base64_length_from_binary_with_lines(length, options,
                                                              line_length);
}

/**
 * Convert a binary input to a base64 output.
 *
 * The default option (simdutf::base64_default) uses the characters `+` and `/`
 * as part of its alphabet. Further, it adds padding (`=`) at the end of the
 * output to ensure that the output length is a multiple of four.
 *
 * The URL option (simdutf::base64_url) uses the characters `-` and `_` as part
 * of its alphabet. No padding is added at the end of the output.
 *
 * This function always succeeds.
 *
 * @param input         the binary to process
 * @param length        the length of the input in bytes
 * @param output        the pointer to a buffer that can hold the conversion
 * result (should be at least base64_length_from_binary(length) bytes long)
 * @param options       the base64 options to use, can be base64_default or
 * base64_url, is base64_default by default.
 * @return number of written bytes, will be equal to
 * base64_length_from_binary(length, options)
 */
size_t binary_to_base64(const char *input, size_t length, char *output,
                        base64_options options = base64_default) noexcept;
  #if SIMDUTF_SPAN
simdutf_really_inline simdutf_warn_unused simdutf_constexpr23 size_t
binary_to_base64(const detail::input_span_of_byte_like auto &input,
                 detail::output_span_of_byte_like auto &&binary_output,
                 base64_options options = base64_default) noexcept {
    #if SIMDUTF_CPLUSPLUS23
  if consteval {
    return scalar::base64::tail_encode_base64(
        binary_output.data(), input.data(), input.size(), options);
  } else
    #endif
  {
    return binary_to_base64(
        reinterpret_cast<const char *>(input.data()), input.size(),
        reinterpret_cast<char *>(binary_output.data()), options);
  }
}
  #endif // SIMDUTF_SPAN

/**
 * Convert a binary input to a base64 output with line breaks.
 *
 * The default option (simdutf::base64_default) uses the characters `+` and `/`
 * as part of its alphabet. Further, it adds padding (`=`) at the end of the
 * output to ensure that the output length is a multiple of four.
 *
 * The URL option (simdutf::base64_url) uses the characters `-` and `_` as part
 * of its alphabet. No padding is added at the end of the output.
 *
 * This function always succeeds.
 *
 * @param input         the binary to process
 * @param length        the length of the input in bytes
 * @param output        the pointer to a buffer that can hold the conversion
 * result (should be at least base64_length_from_binary_with_lines(length,
 * options, line_length) bytes long)
 * @param line_length   the length of lines, must be at least 4 (otherwise it is
 * interpreted as 4),
 * @param options       the base64 options to use, can be base64_default or
 * base64_url, is base64_default by default.
 * @return number of written bytes, will be equal to
 * base64_length_from_binary_with_lines(length, options)
 */
size_t
binary_to_base64_with_lines(const char *input, size_t length, char *output,
                            size_t line_length = simdutf::default_line_length,
                            base64_options options = base64_default) noexcept;
  #if SIMDUTF_SPAN
simdutf_really_inline simdutf_warn_unused simdutf_constexpr23 size_t
binary_to_base64_with_lines(
    const detail::input_span_of_byte_like auto &input,
    detail::output_span_of_byte_like auto &&binary_output,
    size_t line_length = simdutf::default_line_length,
    base64_options options = base64_default) noexcept {
    #if SIMDUTF_CPLUSPLUS23
  if consteval {
    return scalar::base64::tail_encode_base64_impl<true>(
        binary_output.data(), input.data(), input.size(), options, line_length);
  } else
    #endif
  {
    return binary_to_base64_with_lines(
        reinterpret_cast<const char *>(input.data()), input.size(),
        reinterpret_cast<char *>(binary_output.data()), line_length, options);
  }
}
  #endif // SIMDUTF_SPAN

  #if SIMDUTF_ATOMIC_REF
/**
 * Convert a binary input to a base64 output, using atomic accesses.
 * This function comes with a potentially significant performance
 * penalty, but it may be useful in some cases where the input
 * buffers are shared between threads, to avoid undefined
 * behavior in case of data races.
 *
 * The function is for advanced users. Its main use case is when
 * to silence sanitizer warnings. We have no documented use case
 * where this function is actually necessary in terms of practical correctness.
 *
 * This function is only available when simdutf is compiled with
 * C++20 support and __cpp_lib_atomic_ref >= 201806L. You may check
 * the availability of this function by checking the macro
 * SIMDUTF_ATOMIC_REF.
 *
 * The default option (simdutf::base64_default) uses the characters `+` and `/`
 * as part of its alphabet. Further, it adds padding (`=`) at the end of the
 * output to ensure that the output length is a multiple of four.
 *
 * The URL option (simdutf::base64_url) uses the characters `-` and `_` as part
 * of its alphabet. No padding is added at the end of the output.
 *
 * This function always succeeds.
 *
 * This function is considered experimental. It is not tested by default
 * (see the CMake option SIMDUTF_ATOMIC_BASE64_TESTS) nor is it fuzz tested.
 * It is not documented in the public API documentation (README). It is
 * offered on a best effort basis. We rely on the community for further
 * testing and feedback.
 *
 * @brief atomic_binary_to_base64
 * @param input         the binary to process
 * @param length        the length of the input in bytes
 * @param output        the pointer to a buffer that can hold the conversion
 * result (should be at least base64_length_from_binary(length) bytes long)
 * @param options       the base64 options to use, can be base64_default or
 * base64_url, is base64_default by default.
 * @return number of written bytes, will be equal to
 * base64_length_from_binary(length, options)
 */
size_t
atomic_binary_to_base64(const char *input, size_t length, char *output,
                        base64_options options = base64_default) noexcept;
    #if SIMDUTF_SPAN
simdutf_really_inline simdutf_warn_unused size_t
atomic_binary_to_base64(const detail::input_span_of_byte_like auto &input,
                        detail::output_span_of_byte_like auto &&binary_output,
                        base64_options options = base64_default) noexcept {
  return atomic_binary_to_base64(
      reinterpret_cast<const char *>(input.data()), input.size(),
      reinterpret_cast<char *>(binary_output.data()), options);
}
    #endif // SIMDUTF_SPAN
  #endif   // SIMDUTF_ATOMIC_REF

/**
 * Convert a base64 input to a binary output.
 *
 * This function follows the WHATWG forgiving-base64 format, which means that it
 * will ignore any ASCII spaces in the input. You may provide a padded input
 * (with one or two equal signs at the end) or an unpadded input (without any
 * equal signs at the end).
 *
 * See https://infra.spec.whatwg.org/#forgiving-base64-decode
 *
 * This function will fail in case of invalid input. When last_chunk_options =
 * loose, there are two possible reasons for failure: the input contains a
 * number of base64 characters that when divided by 4, leaves a single remainder
 * character (BASE64_INPUT_REMAINDER), or the input contains a character that is
 * not a valid base64 character (INVALID_BASE64_CHARACTER).
 *
 * When the error is INVALID_BASE64_CHARACTER, r.count contains the index in the
 * input where the invalid character was found. When the error is
 * BASE64_INPUT_REMAINDER, then r.count contains the number of bytes decoded.
 *
 * The default option (simdutf::base64_default) expects the characters `+` and
 * `/` as part of its alphabet. The URL option (simdutf::base64_url) expects the
 * characters `-` and `_` as part of its alphabet.
 *
 * The padding (`=`) is validated if present. There may be at most two padding
 * characters at the end of the input. If there are any padding characters, the
 * total number of characters (excluding spaces but including padding
 * characters) must be divisible by four.
 *
 * You should call this function with a buffer that is at least
 * maximal_binary_length_from_base64(input, length) bytes long. If you fail
 * to provide that much space, the function may cause a buffer overflow.
 *
 * Advanced users may want to tailor how the last chunk is handled. By default,
 * we use a loose (forgiving) approach but we also support a strict approach
 * as well as a stop_before_partial approach, as per the following proposal:
 *
 * https://tc39.es/proposal-arraybuffer-base64/spec/#sec-frombase64
 *
 * @param input         the base64 string to process, in ASCII stored as 16-bit
 * units
 * @param length        the length of the string in 16-bit units
 * @param output        the pointer to a buffer that can hold the conversion
 * result (should be at least maximal_binary_length_from_base64(input, length)
 * bytes long).
 * @param options       the base64 options to use, can be base64_default or
 * base64_url, is base64_default by default.
 * @param last_chunk_options the last chunk handling options,
 * last_chunk_handling_options::loose by default
 * but can also be last_chunk_handling_options::strict or
 * last_chunk_handling_options::stop_before_partial.
 * @return a result pair struct (of type simdutf::result containing the two
 * fields error and count) with an error code and position of the
 * INVALID_BASE64_CHARACTER error (in the input in units) if any, or the number
 * of bytes written if successful.
 */
simdutf_warn_unused result
base64_to_binary(const char16_t *input, size_t length, char *output,
                 base64_options options = base64_default,
                 last_chunk_handling_options last_chunk_options =
                     last_chunk_handling_options::loose) noexcept;
  #if SIMDUTF_SPAN
simdutf_really_inline simdutf_warn_unused simdutf_constexpr23 result
base64_to_binary(
    std::span<const char16_t> input,
    detail::output_span_of_byte_like auto &&binary_output,
    base64_options options = base64_default,
    last_chunk_handling_options last_chunk_options = loose) noexcept {
    #if SIMDUTF_CPLUSPLUS23
  if consteval {
    return scalar::base64::base64_to_binary_details_impl(
        input.data(), input.size(), binary_output.data(), options,
        last_chunk_options);
  } else
    #endif
  {
    return base64_to_binary(input.data(), input.size(),
                            reinterpret_cast<char *>(binary_output.data()),
                            options, last_chunk_options);
  }
}
  #endif // SIMDUTF_SPAN

/**
 * Convert a base64 input to a binary output while returning more details
 * than base64_to_binary.
 *
 * This function follows the WHATWG forgiving-base64 format, which means that it
 * will ignore any ASCII spaces in the input. You may provide a padded input
 * (with one or two equal signs at the end) or an unpadded input (without any
 * equal signs at the end).
 *
 * See https://infra.spec.whatwg.org/#forgiving-base64-decode
 *
 * Unlike base64_to_binary, this function returns a full_result with both
 * input_count and output_count, so you always know how much input was consumed
 * and how much output was written. There are three cases where the input may
 * not be fully consumed:
 *
 * 1. stop_before_partial: When last_chunk_options is set to
 *    stop_before_partial, any incomplete 4-character group at the end of the
 *    input is left unconsumed. This is useful for streaming/chunked decoding
 *    where you can carry over the unconsumed input to the next chunk.
 *
 * 2. INVALID_BASE64_CHARACTER: The input contains a character that is not a
 *    valid base64 character. In this case, input_count indicates where the
 *    invalid character was found.
 *
 * 3. BASE64_INPUT_REMAINDER: When last_chunk_options is loose, the input
 *    contains a number of base64 characters that, when divided by 4, leaves
 *    a single remainder character (which cannot encode any bytes).
 *
 * You should call this function with a buffer that is at least
 * maximal_binary_length_from_base64(input, length) bytes long. If you fail to
 * provide that much space, the function may cause a buffer overflow.
 *
 * @param input         the base64 string to process
 * @param length        the length of the string in bytes
 * @param output        the pointer to a buffer that can hold the conversion
 * result (should be at least maximal_binary_length_from_base64(input, length)
 * bytes long).
 * @param options       the base64 options to use, can be base64_default or
 * base64_url, is base64_default by default.
 * @param last_chunk_options the last chunk handling options,
 * last_chunk_handling_options::loose by default
 * but can also be last_chunk_handling_options::strict or
 * last_chunk_handling_options::stop_before_partial.
 * @return a full_result struct (of type simdutf::full_result containing the
 * three fields error, input_count and output_count).
 */
simdutf_warn_unused full_result
base64_to_binary_details(const char *input, size_t length, char *output,
                         base64_options options = base64_default,
                         last_chunk_handling_options last_chunk_options =
                             last_chunk_handling_options::loose) noexcept;
  #if SIMDUTF_SPAN
simdutf_really_inline simdutf_warn_unused simdutf_constexpr23 full_result
base64_to_binary_details(
    const detail::input_span_of_byte_like auto &input,
    detail::output_span_of_byte_like auto &&binary_output,
    base64_options options = base64_default,
    last_chunk_handling_options last_chunk_options = loose) noexcept {
    #if SIMDUTF_CPLUSPLUS23
  if consteval {
    return scalar::base64::base64_to_binary_details_impl(
        input.data(), input.size(), binary_output.data(), options,
        last_chunk_options);
  } else
    #endif
  {
    return base64_to_binary_details(
        reinterpret_cast<const char *>(input.data()), input.size(),
        reinterpret_cast<char *>(binary_output.data()), options,
        last_chunk_options);
  }
}
  #endif // SIMDUTF_SPAN

/**
 * Convert a base64 input to a binary output while returning more details
 * than base64_to_binary.
 *
 * This function follows the WHATWG forgiving-base64 format, which means that it
 * will ignore any ASCII spaces in the input. You may provide a padded input
 * (with one or two equal signs at the end) or an unpadded input (without any
 * equal signs at the end).
 *
 * See https://infra.spec.whatwg.org/#forgiving-base64-decode
 *
 * Unlike base64_to_binary, this function returns a full_result with both
 * input_count and output_count, so you always know how much input was consumed
 * and how much output was written. There are three cases where the input may
 * not be fully consumed:
 *
 * 1. stop_before_partial: When last_chunk_options is set to
 *    stop_before_partial, any incomplete 4-character group at the end of the
 *    input is left unconsumed. This is useful for streaming/chunked decoding
 *    where you can carry over the unconsumed input to the next chunk.
 *
 * 2. INVALID_BASE64_CHARACTER: The input contains a character that is not a
 *    valid base64 character. In this case, input_count indicates where the
 *    invalid character was found.
 *
 * 3. BASE64_INPUT_REMAINDER: When last_chunk_options is loose, the input
 *    contains a number of base64 characters that, when divided by 4, leaves
 *    a single remainder character (which cannot encode any bytes).
 *
 * You should call this function with a buffer that is at least
 * maximal_binary_length_from_base64(input, length) bytes long. If you fail to
 * provide that much space, the function may cause a buffer overflow.
 *
 * @param input         the base64 string to process, in ASCII stored as 16-bit
 * units
 * @param length        the length of the string in 16-bit units
 * @param output        the pointer to a buffer that can hold the conversion
 * result (should be at least maximal_binary_length_from_base64(input, length)
 * bytes long).
 * @param options       the base64 options to use, can be base64_default or
 * base64_url, is base64_default by default.
 * @param last_chunk_options the last chunk handling options,
 * last_chunk_handling_options::loose by default
 * but can also be last_chunk_handling_options::strict or
 * last_chunk_handling_options::stop_before_partial.
 * @return a full_result struct (of type simdutf::full_result containing the
 * three fields error, input_count and output_count).
 */
simdutf_warn_unused full_result
base64_to_binary_details(const char16_t *input, size_t length, char *output,
                         base64_options options = base64_default,
                         last_chunk_handling_options last_chunk_options =
                             last_chunk_handling_options::loose) noexcept;
  #if SIMDUTF_SPAN
simdutf_really_inline simdutf_warn_unused simdutf_constexpr23 full_result
base64_to_binary_details(
    std::span<const char16_t> input,
    detail::output_span_of_byte_like auto &&binary_output,
    base64_options options = base64_default,
    last_chunk_handling_options last_chunk_options = loose) noexcept {
    #if SIMDUTF_CPLUSPLUS23
  if consteval {
    return scalar::base64::base64_to_binary_details_impl(
        input.data(), input.size(), binary_output.data(), options,
        last_chunk_options);
  } else
    #endif
  {
    return base64_to_binary_details(
        input.data(), input.size(),
        reinterpret_cast<char *>(binary_output.data()), options,
        last_chunk_options);
  }
}
  #endif // SIMDUTF_SPAN

/**
 * Check if a character is an ignorable base64 character.
 * Checking a large input, character by character, is not computationally
 * efficient.
 *
 * @param input         the character to check
 * @param options       the base64 options to use, is base64_default by default.
 * @return true if the character is an ignorable base64 character, false
 * otherwise.
 */
simdutf_warn_unused simdutf_really_inline simdutf_constexpr23 bool
base64_ignorable(char input, base64_options options = base64_default) noexcept {
  return scalar::base64::is_ignorable(input, options);
}
simdutf_warn_unused simdutf_really_inline simdutf_constexpr23 bool
base64_ignorable(char16_t input,
                 base64_options options = base64_default) noexcept {
  return scalar::base64::is_ignorable(input, options);
}

/**
 * Check if a character is a valid base64 character.
 * Checking a large input, character by character, is not computationally
 * efficient.
 * Note that padding characters are not considered valid base64 characters in
 * this context, nor are spaces.
 *
 * @param input         the character to check
 * @param options       the base64 options to use, is base64_default by default.
 * @return true if the character is a base64 character, false otherwise.
 */
simdutf_warn_unused simdutf_really_inline simdutf_constexpr23 bool
base64_valid(char input, base64_options options = base64_default) noexcept {
  return scalar::base64::is_base64(input, options);
}
simdutf_warn_unused simdutf_really_inline simdutf_constexpr23 bool
base64_valid(char16_t input, base64_options options = base64_default) noexcept {
  return scalar::base64::is_base64(input, options);
}

/**
 * Check if a character is a valid base64 character or the padding character
 * ('='). Checking a large input, character by character, is not computationally
 * efficient.
 *
 * @param input         the character to check
 * @param options       the base64 options to use, is base64_default by default.
 * @return true if the character is a base64 character, false otherwise.
 */
simdutf_warn_unused simdutf_really_inline simdutf_constexpr23 bool
base64_valid_or_padding(char input,
                        base64_options options = base64_default) noexcept {
  return scalar::base64::is_base64_or_padding(input, options);
}
simdutf_warn_unused simdutf_really_inline simdutf_constexpr23 bool
base64_valid_or_padding(char16_t input,
                        base64_options options = base64_default) noexcept {
  return scalar::base64::is_base64_or_padding(input, options);
}

/**
 * Convert a base64 input to a binary output.
 *
 * This function follows the WHATWG forgiving-base64 format, which means that it
 * will ignore any ASCII spaces in the input. You may provide a padded input
 * (with one or two equal signs at the end) or an unpadded input (without any
 * equal signs at the end).
 *
 * See https://infra.spec.whatwg.org/#forgiving-base64-decode
 *
 * This function will fail in case of invalid input. When last_chunk_options =
 * loose, there are three possible reasons for failure: the input contains a
 * number of base64 characters that when divided by 4, leaves a single remainder
 * character (BASE64_INPUT_REMAINDER), the input contains a character that is
 * not a valid base64 character (INVALID_BASE64_CHARACTER), or the output buffer
 * is too small (OUTPUT_BUFFER_TOO_SMALL).
 *
 * When OUTPUT_BUFFER_TOO_SMALL, we return both the number of bytes written
 * and the number of units processed, see description of the parameters and
 * returned value.
 *
 * When the error is INVALID_BASE64_CHARACTER, r.count contains the index in the
 * input where the invalid character was found. When the error is
 * BASE64_INPUT_REMAINDER, then r.count contains the number of bytes decoded.
 *
 * The default option (simdutf::base64_default) expects the characters `+` and
 * `/` as part of its alphabet. The URL option (simdutf::base64_url) expects the
 * characters `-` and `_` as part of its alphabet.
 *
 * The padding (`=`) is validated if present. There may be at most two padding
 * characters at the end of the input. If there are any padding characters, the
 * total number of characters (excluding spaces but including padding
 * characters) must be divisible by four.
 *
 * The INVALID_BASE64_CHARACTER cases are considered fatal and you are expected
 * to discard the output unless the parameter decode_up_to_bad_char is set to
 * true. In that case, the function will decode up to the first invalid
 * character. Extra padding characters ('=') are considered invalid characters.
 *
 * Advanced users may want to tailor how the last chunk is handled. By default,
 * we use a loose (forgiving) approach but we also support a strict approach
 * as well as a stop_before_partial approach, as per the following proposal:
 *
 * https://tc39.es/proposal-arraybuffer-base64/spec/#sec-frombase64
 *
 * @param input         the base64 string to process, in ASCII stored as 8-bit
 * or 16-bit units
 * @param length        the length of the string in 8-bit or 16-bit units.
 * @param output        the pointer to a buffer that can hold the conversion
 * result.
 * @param outlen        the number of bytes that can be written in the output
 * buffer. Upon return, it is modified to reflect how many bytes were written.
 * @param options       the base64 options to use, can be base64_default or
 * base64_url, is base64_default by default.
 * @param last_chunk_options the last chunk handling options,
 * last_chunk_handling_options::loose by default
 * but can also be last_chunk_handling_options::strict or
 * last_chunk_handling_options::stop_before_partial.
 * @param decode_up_to_bad_char if true, the function will decode up to the
 * first invalid character. By default (false), it is assumed that the output
 * buffer is to be discarded. When there are multiple errors in the input,
 * using decode_up_to_bad_char might trigger a different error.
 * @return a result pair struct (of type simdutf::result containing the two
 * fields error and count) with an error code and position of the
 * INVALID_BASE64_CHARACTER error (in the input in units) if any, or the number
 * of units processed if successful.
 */
simdutf_warn_unused result
base64_to_binary_safe(const char *input, size_t length, char *output,
                      size_t &outlen, base64_options options = base64_default,
                      last_chunk_handling_options last_chunk_options =
                          last_chunk_handling_options::loose,
                      bool decode_up_to_bad_char = false) noexcept;
// the span overload has moved to the bottom of the file

simdutf_warn_unused result
base64_to_binary_safe(const char16_t *input, size_t length, char *output,
                      size_t &outlen, base64_options options = base64_default,
                      last_chunk_handling_options last_chunk_options =
                          last_chunk_handling_options::loose,
                      bool decode_up_to_bad_char = false) noexcept;
  // span overload moved to bottom of file

  #if SIMDUTF_ATOMIC_REF
/**
 * Convert a base64 input to a binary output with a size limit and using atomic
 * operations.
 *
 * Like `base64_to_binary_safe` but using atomic operations, this function is
 * thread-safe for concurrent memory access, allowing the output
 * buffers to be shared between threads without undefined behavior in case of
 * data races.
 *
 * This function comes with a potentially significant performance penalty, but
 * is useful when thread safety is needed during base64 decoding.
 *
 * This function is only available when simdutf is compiled with
 * C++20 support and __cpp_lib_atomic_ref >= 201806L. You may check
 * the availability of this function by checking the macro
 * SIMDUTF_ATOMIC_REF.
 *
 * This function is considered experimental. It is not tested by default
 * (see the CMake option SIMDUTF_ATOMIC_BASE64_TESTS) nor is it fuzz tested.
 * It is not documented in the public API documentation (README). It is
 * offered on a best effort basis. We rely on the community for further
 * testing and feedback.
 *
 * @param input         the base64 input to decode
 * @param length        the length of the input in bytes
 * @param output        the pointer to buffer that can hold the conversion
 * result
 * @param outlen        the number of bytes that can be written in the output
 * buffer. Upon return, it is modified to reflect how many bytes were written.
 * @param options       the base64 options to use (default, url, etc.)
 * @param last_chunk_options the last chunk handling options (loose, strict,
 * stop_before_partial)
 * @param decode_up_to_bad_char if true, the function will decode up to the
 * first invalid character. By default (false), it is assumed that the output
 * buffer is to be discarded. When there are multiple errors in the input,
 * using decode_up_to_bad_char might trigger a different error.
 * @return a result struct with an error code and count indicating error
 * position or success
 */
simdutf_warn_unused result atomic_base64_to_binary_safe(
    const char *input, size_t length, char *output, size_t &outlen,
    base64_options options = base64_default,
    last_chunk_handling_options last_chunk_options =
        last_chunk_handling_options::loose,
    bool decode_up_to_bad_char = false) noexcept;
simdutf_warn_unused result atomic_base64_to_binary_safe(
    const char16_t *input, size_t length, char *output, size_t &outlen,
    base64_options options = base64_default,
    last_chunk_handling_options last_chunk_options = loose,
    bool decode_up_to_bad_char = false) noexcept;
    #if SIMDUTF_SPAN
/**
 * @brief span overload
 * @return a tuple of result and outlen
 */
simdutf_really_inline simdutf_warn_unused std::tuple<result, std::size_t>
atomic_base64_to_binary_safe(
    const detail::input_span_of_byte_like auto &binary_input,
    detail::output_span_of_byte_like auto &&output,
    base64_options options = base64_default,
    last_chunk_handling_options last_chunk_options =
        last_chunk_handling_options::loose,
    bool decode_up_to_bad_char = false) noexcept {
  size_t outlen = output.size();
  auto ret = atomic_base64_to_binary_safe(
      reinterpret_cast<const char *>(binary_input.data()), binary_input.size(),
      reinterpret_cast<char *>(output.data()), outlen, options,
      last_chunk_options, decode_up_to_bad_char);
  return {ret, outlen};
}
/**
 * @brief span overload
 * @return a tuple of result and outlen
 */
simdutf_warn_unused std::tuple<result, std::size_t>
atomic_base64_to_binary_safe(
    std::span<const char16_t> base64_input,
    detail::output_span_of_byte_like auto &&binary_output,
    base64_options options = base64_default,
    last_chunk_handling_options last_chunk_options = loose,
    bool decode_up_to_bad_char = false) noexcept {
  size_t outlen = binary_output.size();
  auto ret = atomic_base64_to_binary_safe(
      base64_input.data(), base64_input.size(),
      reinterpret_cast<char *>(binary_output.data()), outlen, options,
      last_chunk_options, decode_up_to_bad_char);
  return {ret, outlen};
}
    #endif // SIMDUTF_SPAN
  #endif   // SIMDUTF_ATOMIC_REF

/**
 * An implementation of simdutf for a particular CPU architecture.
 *
 * Also used to maintain the currently active implementation. The active
 * implementation is automatically initialized on first use to the most advanced
 * implementation supported by the host.
 */
class implementation {
public:
  /**
   * The name of this implementation.
   *
   *     const implementation *impl = simdutf::active_implementation;
   *     cout << "simdutf is optimized for " << impl->name() << "(" <<
   * impl->description() << ")" << endl;
   *
   * @return the name of the implementation, e.g. "haswell", "westmere", "arm64"
   */
  virtual std::string_view name() const noexcept { return _name; }

  /**
   * The description of this implementation.
   *
   *     const implementation *impl = simdutf::active_implementation;
   *     cout << "simdutf is optimized for " << impl->name() << "(" <<
   * impl->description() << ")" << endl;
   *
   * @return the name of the implementation, e.g. "haswell", "westmere", "arm64"
   */
  virtual std::string_view description() const noexcept { return _description; }

  /**
   * The instruction sets this implementation is compiled against
   * and the current CPU match. This function may poll the current CPU/system
   * and should therefore not be called too often if performance is a concern.
   *
   *
   * @return true if the implementation can be safely used on the current system
   * (determined at runtime)
   */
  bool supported_by_runtime_system() const;

  /**
   * @private For internal implementation use
   *
   * The instruction sets this implementation is compiled against.
   *
   * @return a mask of all required `internal::instruction_set::` values
   */
  virtual uint32_t required_instruction_sets() const {
    return _required_instruction_sets;
  }

  /**
   * Provide the maximal binary length in bytes given the base64 input.
   * As long as the input does not contain ignorable characters (e.g., ASCII
   * spaces or linefeed characters), the result is exact. In particular, the
   * function checks for padding characters.
   *
   * The function is fast (constant time). It checks up to two characters at
   * the end of the string. The input is not otherwise validated or read..
   *
   * @param input         the base64 input to process
   * @param length        the length of the base64 input in bytes
   * @return maximal number of binary bytes
   */
  simdutf_warn_unused size_t maximal_binary_length_from_base64(
      const char *input, size_t length) const noexcept;

  /**
   * Provide the maximal binary length in bytes given the base64 input.
   * As long as the input does not contain ignorable characters (e.g., ASCII
   * spaces or linefeed characters), the result is exact. In particular, the
   * function checks for padding characters.
   *
   * The function is fast (constant time). It checks up to two characters at
   * the end of the string. The input is not otherwise validated or read.
   *
   * @param input         the base64 input to process, in ASCII stored as 16-bit
   * units
   * @param length        the length of the base64 input in 16-bit units
   * @return maximal number of binary bytes
   */
  simdutf_warn_unused size_t maximal_binary_length_from_base64(
      const char16_t *input, size_t length) const noexcept;

  /**
   * Compute the binary length from a base64 input with ASCII spaces.
   * This function is useful for well-formed base64 inputs that may contain
   * ASCII spaces (such as line breaks). For such inputs, the result is exact.
   *
   * The function counts non-whitespace characters (ASCII value > 0x20) and
   * subtracts padding characters ('=') found at the end.
   *
   * @param input         the base64 input to process
   * @param length        the length of the base64 input in bytes
   * @return number of binary bytes
   */
  simdutf_warn_unused virtual size_t
  binary_length_from_base64(const char *input, size_t length) const noexcept;

  /**
   * Compute the binary length from a base64 input with ASCII spaces.
   * This function is useful for well-formed base64 inputs that may contain
   * ASCII spaces (such as line breaks). For such inputs, the result is exact.
   *
   * The function counts non-whitespace characters (ASCII value > 0x20) and
   * subtracts padding characters ('=') found at the end.
   *
   * @param input         the base64 input to process, in ASCII stored as 16-bit
   * units
   * @param length        the length of the base64 input in 16-bit units
   * @return number of binary bytes
   */
  simdutf_warn_unused virtual size_t
  binary_length_from_base64(const char16_t *input,
                            size_t length) const noexcept;

  /**
   * Convert a base64 input to a binary output.
   *
   * This function follows the WHATWG forgiving-base64 format, which means that
   * it will ignore any ASCII spaces in the input. You may provide a padded
   * input (with one or two equal signs at the end) or an unpadded input
   * (without any equal signs at the end).
   *
   * See https://infra.spec.whatwg.org/#forgiving-base64-decode
   *
   * This function will fail in case of invalid input. When last_chunk_options =
   * loose, there are two possible reasons for failure: the input contains a
   * number of base64 characters that when divided by 4, leaves a single
   * remainder character (BASE64_INPUT_REMAINDER), or the input contains a
   * character that is not a valid base64 character (INVALID_BASE64_CHARACTER).
   *
   * You should call this function with a buffer that is at least
   * maximal_binary_length_from_base64(input, length) bytes long. If you fail to
   * provide that much space, the function may cause a buffer overflow.
   *
   * @param input         the base64 string to process
   * @param length        the length of the string in bytes
   * @param output        the pointer to a buffer that can hold the conversion
   * result (should be at least maximal_binary_length_from_base64(input, length)
   * bytes long).
   * @param options       the base64 options to use, can be base64_default or
   * base64_url, is base64_default by default.
   * @return a result pair struct (of type simdutf::result containing the two
   * fields error and count) with an error code and either position of the error
   * (in the input in bytes) if any, or the number of bytes written if
   * successful.
   */
  simdutf_warn_unused virtual result
  base64_to_binary(const char *input, size_t length, char *output,
                   base64_options options = base64_default,
                   last_chunk_handling_options last_chunk_options =
                       last_chunk_handling_options::loose) const noexcept = 0;

  /**
   * Convert a base64 input to a binary output while returning more details
   * than base64_to_binary.
   *
   * This function follows the WHATWG forgiving-base64 format, which means that
   * it will ignore any ASCII spaces in the input. You may provide a padded
   * input (with one or two equal signs at the end) or an unpadded input
   * (without any equal signs at the end).
   *
   * See https://infra.spec.whatwg.org/#forgiving-base64-decode
   *
   * This function will fail in case of invalid input. When last_chunk_options =
   * loose, there are two possible reasons for failure: the input contains a
   * number of base64 characters that when divided by 4, leaves a single
   * remainder character (BASE64_INPUT_REMAINDER), or the input contains a
   * character that is not a valid base64 character (INVALID_BASE64_CHARACTER).
   *
   * You should call this function with a buffer that is at least
   * maximal_binary_length_from_base64(input, length) bytes long. If you fail to
   * provide that much space, the function may cause a buffer overflow.
   *
   * @param input         the base64 string to process
   * @param length        the length of the string in bytes
   * @param output        the pointer to a buffer that can hold the conversion
   * result (should be at least maximal_binary_length_from_base64(input, length)
   * bytes long).
   * @param options       the base64 options to use, can be base64_default or
   * base64_url, is base64_default by default.
   * @return a full_result pair struct (of type simdutf::result containing the
   * three fields error, input_count and output_count).
   */
  simdutf_warn_unused virtual full_result base64_to_binary_details(
      const char *input, size_t length, char *output,
      base64_options options = base64_default,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept = 0;

  /**
   * Convert a base64 input to a binary output.
   *
   * This function follows the WHATWG forgiving-base64 format, which means that
   * it will ignore any ASCII spaces in the input. You may provide a padded
   * input (with one or two equal signs at the end) or an unpadded input
   * (without any equal signs at the end).
   *
   * See https://infra.spec.whatwg.org/#forgiving-base64-decode
   *
   * This function will fail in case of invalid input. When last_chunk_options =
   * loose, there are two possible reasons for failure: the input contains a
   * number of base64 characters that when divided by 4, leaves a single
   * remainder character (BASE64_INPUT_REMAINDER), or the input contains a
   * character that is not a valid base64 character (INVALID_BASE64_CHARACTER).
   *
   * You should call this function with a buffer that is at least
   * maximal_binary_length_from_base64(input, length) bytes long. If you
   * fail to provide that much space, the function may cause a buffer overflow.
   *
   * @param input         the base64 string to process, in ASCII stored as
   * 16-bit units
   * @param length        the length of the string in 16-bit units
   * @param output        the pointer to a buffer that can hold the conversion
   * result (should be at least maximal_binary_length_from_base64(input, length)
   * bytes long).
   * @param options       the base64 options to use, can be base64_default or
   * base64_url, is base64_default by default.
   * @return a result pair struct (of type simdutf::result containing the two
   * fields error and count) with an error code and position of the
   * INVALID_BASE64_CHARACTER error (in the input in units) if any, or the
   * number of bytes written if successful.
   */
  simdutf_warn_unused virtual result
  base64_to_binary(const char16_t *input, size_t length, char *output,
                   base64_options options = base64_default,
                   last_chunk_handling_options last_chunk_options =
                       last_chunk_handling_options::loose) const noexcept = 0;

  /**
   * Convert a base64 input to a binary output while returning more details
   * than base64_to_binary.
   *
   * This function follows the WHATWG forgiving-base64 format, which means that
   * it will ignore any ASCII spaces in the input. You may provide a padded
   * input (with one or two equal signs at the end) or an unpadded input
   * (without any equal signs at the end).
   *
   * See https://infra.spec.whatwg.org/#forgiving-base64-decode
   *
   * This function will fail in case of invalid input. When last_chunk_options =
   * loose, there are two possible reasons for failure: the input contains a
   * number of base64 characters that when divided by 4, leaves a single
   * remainder character (BASE64_INPUT_REMAINDER), or the input contains a
   * character that is not a valid base64 character (INVALID_BASE64_CHARACTER).
   *
   * You should call this function with a buffer that is at least
   * maximal_binary_length_from_base64(input, length) bytes long. If you fail to
   * provide that much space, the function may cause a buffer overflow.
   *
   * @param input         the base64 string to process
   * @param length        the length of the string in bytes
   * @param output        the pointer to a buffer that can hold the conversion
   * result (should be at least maximal_binary_length_from_base64(input, length)
   * bytes long).
   * @param options       the base64 options to use, can be base64_default or
   * base64_url, is base64_default by default.
   * @return a full_result pair struct (of type simdutf::result containing the
   * three fields error, input_count and output_count).
   */
  simdutf_warn_unused virtual full_result base64_to_binary_details(
      const char16_t *input, size_t length, char *output,
      base64_options options = base64_default,
      last_chunk_handling_options last_chunk_options =
          last_chunk_handling_options::loose) const noexcept = 0;

  /**
   * Provide the base64 length in bytes given the length of a binary input.
   *
   * @param length        the length of the input in bytes
   * @param options       the base64 options to use, can be base64_default or
   * base64_url, is base64_default by default.
   * @return number of base64 bytes
   */
  simdutf_warn_unused size_t base64_length_from_binary(
      size_t length, base64_options options = base64_default) const noexcept;

  /**
   * Convert a binary input to a base64 output.
   *
   * The default option (simdutf::base64_default) uses the characters `+` and
   * `/` as part of its alphabet. Further, it adds padding (`=`) at the end of
   * the output to ensure that the output length is a multiple of four.
   *
   * The URL option (simdutf::base64_url) uses the characters `-` and `_` as
   * part of its alphabet. No padding is added at the end of the output.
   *
   * This function always succeeds.
   *
   * @param input         the binary to process
   * @param length        the length of the input in bytes
   * @param output        the pointer to a buffer that can hold the conversion
   * result (should be at least base64_length_from_binary(length) bytes long)
   * @param options       the base64 options to use, can be base64_default or
   * base64_url, is base64_default by default.
   * @return number of written bytes, will be equal to
   * base64_length_from_binary(length, options)
   */
  virtual size_t
  binary_to_base64(const char *input, size_t length, char *output,
                   base64_options options = base64_default) const noexcept = 0;

  /**
   * Convert a binary input to a base64 output with lines of given length.
   * Lines are separated by a single linefeed character.
   *
   * The default option (simdutf::base64_default) uses the characters `+` and
   * `/` as part of its alphabet. Further, it adds padding (`=`) at the end of
   * the output to ensure that the output length is a multiple of four.
   *
   * The URL option (simdutf::base64_url) uses the characters `-` and `_` as
   * part of its alphabet. No padding is added at the end of the output.
   *
   * This function always succeeds.
   *
   * @param input         the binary to process
   * @param length        the length of the input in bytes
   * @param output        the pointer to a buffer that can hold the conversion
   * result (should be at least base64_length_from_binary_with_lines(length,
   * options, line_length) bytes long)
   * @param line_length   the length of each line, values smaller than 4 are
   * interpreted as 4
   * @param options       the base64 options to use, can be base64_default or
   * base64_url, is base64_default by default.
   * @return number of written bytes, will be equal to
   * base64_length_from_binary_with_lines(length, options, line_length)
   */
  virtual size_t binary_to_base64_with_lines(
      const char *input, size_t length, char *output,
      size_t line_length = simdutf::default_line_length,
      base64_options options = base64_default) const noexcept = 0;

  /**
   * Find the first occurrence of a character in a string. If the character is
   * not found, return a pointer to the end of the string.
   * @param start        the start of the string
   * @param end          the end of the string
   * @param character    the character to find
   * @return a pointer to the first occurrence of the character in the string,
   * or a pointer to the end of the string if the character is not found.
   *
   */
  virtual const char *find(const char *start, const char *end,
                           char character) const noexcept = 0;
  virtual const char16_t *find(const char16_t *start, const char16_t *end,
                               char16_t character) const noexcept = 0;

#ifdef SIMDUTF_INTERNAL_TESTS
  // This method is exported only in developer mode, its purpose
  // is to expose some internal test procedures from the given
  // implementation and then use them through our standard test
  // framework.
  //
  // Regular users should not use it, the tests of the public
  // API are enough.

  struct TestProcedure {
    // display name
    std::string_view name;

    // procedure should return whether given test pass or not
    void (*procedure)(const implementation &);
  };

  virtual std::vector<TestProcedure> internal_tests() const;
#endif

protected:
  /** @private Construct an implementation with the given name and description.
   * For subclasses. */
  simdutf_really_inline implementation(const char *name,
                                       const char *description,
                                       uint32_t required_instruction_sets)
      : _name(name), _description(description),
        _required_instruction_sets(required_instruction_sets) {}

protected:
  ~implementation() = default;

private:
  /**
   * The name of this implementation.
   */
  const char *_name;

  /**
   * The description of this implementation.
   */
  const char *_description;

  /**
   * Instruction sets required for this implementation.
   */
  const uint32_t _required_instruction_sets;
};

/** @private */
namespace internal {

/**
 * The list of available implementations compiled into simdutf.
 */
class available_implementation_list {
public:
  /** Get the list of available implementations compiled into simdutf */
  simdutf_really_inline available_implementation_list() {}
  /** Number of implementations */
  size_t size() const noexcept;
  /** STL const begin() iterator */
  const implementation *const *begin() const noexcept;
  /** STL const end() iterator */
  const implementation *const *end() const noexcept;

  /**
   * Get the implementation with the given name.
   *
   * Case sensitive.
   *
   *     const implementation *impl =
   * simdutf::available_implementations["westmere"]; if (!impl) { exit(1); } if
   * (!imp->supported_by_runtime_system()) { exit(1); }
   *     simdutf::active_implementation = impl;
   *
   * @param name the implementation to find, e.g. "westmere", "haswell", "arm64"
   * @return the implementation, or nullptr if the parse failed.
   */
  const implementation *operator[](std::string_view name) const noexcept {
    for (const implementation *impl : *this) {
      if (impl->name() == name) {
        return impl;
      }
    }
    return nullptr;
  }

  /**
   * Detect the most advanced implementation supported by the current host.
   *
   * This is used to initialize the implementation on startup.
   *
   *     const implementation *impl =
   * simdutf::available_implementation::detect_best_supported();
   *     simdutf::active_implementation = impl;
   *
   * @return the most advanced supported implementation for the current host, or
   * an implementation that returns UNSUPPORTED_ARCHITECTURE if there is no
   * supported implementation. Will never return nullptr.
   */
  const implementation *detect_best_supported() const noexcept;
};

template <typename T> class atomic_ptr {
public:
  atomic_ptr(T *_ptr) : ptr{_ptr} {}

#if defined(SIMDUTF_NO_THREADS)
  operator const T *() const { return ptr; }
  const T &operator*() const { return *ptr; }
  const T *operator->() const { return ptr; }

  operator T *() { return ptr; }
  T &operator*() { return *ptr; }
  T *operator->() { return ptr; }
  atomic_ptr &operator=(T *_ptr) {
    ptr = _ptr;
    return *this;
  }

#else
  operator const T *() const { return ptr.load(); }
  const T &operator*() const { return *ptr; }
  const T *operator->() const { return ptr.load(); }

  operator T *() { return ptr.load(); }
  T &operator*() { return *ptr; }
  T *operator->() { return ptr.load(); }
  atomic_ptr &operator=(T *_ptr) {
    ptr = _ptr;
    return *this;
  }

#endif

private:
#if defined(SIMDUTF_NO_THREADS)
  T *ptr;
#else
  std::atomic<T *> ptr;
#endif
};

class detect_best_supported_implementation_on_first_use;

} // namespace internal

/**
 * The list of available implementations compiled into simdutf.
 */
extern SIMDUTF_DLLIMPORTEXPORT const internal::available_implementation_list &
get_available_implementations();

/**
 * The active implementation.
 *
 * Automatically initialized on first use to the most advanced implementation
 * supported by this hardware.
 */
extern SIMDUTF_DLLIMPORTEXPORT internal::atomic_ptr<const implementation> &
get_active_implementation();

} // namespace simdutf

  // this header is not part of the public api
/* begin file include/simdutf/base64_implementation.h */
#ifndef SIMDUTF_BASE64_IMPLEMENTATION_H
#define SIMDUTF_BASE64_IMPLEMENTATION_H

// this is not part of the public api

namespace simdutf {

template <typename chartype>
simdutf_warn_unused simdutf_constexpr23 result slow_base64_to_binary_safe_impl(
    const chartype *input, size_t length, char *output, size_t &outlen,
    base64_options options,
    last_chunk_handling_options last_chunk_options) noexcept {
  const bool ignore_garbage = (options & base64_default_accept_garbage) != 0;
  auto ri = simdutf::scalar::base64::find_end(input, length, options);
  size_t equallocation = ri.equallocation;
  size_t equalsigns = ri.equalsigns;
  length = ri.srclen;
  size_t full_input_length = ri.full_input_length;
  (void)full_input_length;
  if (length == 0) {
    outlen = 0;
    if (!ignore_garbage && equalsigns > 0) {
      return {INVALID_BASE64_CHARACTER, equallocation};
    }
    return {SUCCESS, 0};
  }

  // The parameters of base64_tail_decode_safe are:
  // - dst: the output buffer
  // - outlen: the size of the output buffer
  // - srcr: the input buffer
  // - length: the size of the input buffer
  // - padded_characters: the number of padding characters
  // - options: the options for the base64 decoder
  // - last_chunk_options: the options for the last chunk
  // The function will return the number of bytes written to the output buffer
  // and the number of bytes read from the input buffer.
  // The function will also return an error code if the input buffer is not
  // valid base64.
  full_result r = scalar::base64::base64_tail_decode_safe(
      output, outlen, input, length, equalsigns, options, last_chunk_options);
  r = scalar::base64::patch_tail_result(r, 0, 0, equallocation,
                                        full_input_length, last_chunk_options);
  outlen = r.output_count;
  if (!is_partial(last_chunk_options) && r.error == error_code::SUCCESS &&
      equalsigns > 0) {
    // additional checks
    if ((outlen % 3 == 0) || ((outlen % 3) + 1 + equalsigns != 4)) {
      r.error = error_code::INVALID_BASE64_CHARACTER;
    }
  }
  return {r.error, r.input_count}; // we cannot return r itself because it gets
                                   // converted to error/output_count
}

template <typename chartype>
simdutf_warn_unused simdutf_constexpr23 result base64_to_binary_safe_impl(
    const chartype *input, size_t length, char *output, size_t &outlen,
    base64_options options,
    last_chunk_handling_options last_chunk_handling_options,
    bool decode_up_to_bad_char) noexcept {
  static_assert(std::is_same<chartype, char>::value ||
                    std::is_same<chartype, char16_t>::value,
                "Only char and char16_t are supported.");
  size_t remaining_input_length = length;
  size_t remaining_output_length = outlen;
  size_t input_position = 0;
  size_t output_position = 0;

  // We also do a first pass using the fast path to decode as much as possible
  size_t safe_input = (std::min)(
      remaining_input_length,
      base64_length_from_binary(remaining_output_length / 3 * 3, options));
  bool done_with_partial = (safe_input == remaining_input_length);
  simdutf::full_result r;

#if SIMDUTF_CPLUSPLUS23
  if consteval {
    r = scalar::base64::base64_to_binary_details_impl(
        input + input_position, safe_input, output + output_position, options,
        done_with_partial
            ? last_chunk_handling_options
            : simdutf::last_chunk_handling_options::only_full_chunks);
  } else
#endif
  {
    r = get_active_implementation()->base64_to_binary_details(
        input + input_position, safe_input, output + output_position, options,
        done_with_partial
            ? last_chunk_handling_options
            : simdutf::last_chunk_handling_options::only_full_chunks);
  }
  simdutf_log_assert(r.input_count <= safe_input,
                     "You should not read more than safe_input");
  simdutf_log_assert(r.output_count <= remaining_output_length,
                     "You should not write more than remaining_output_length");
  // Technically redundant, but we want to be explicit about it.
  input_position += r.input_count;
  output_position += r.output_count;
  remaining_input_length -= r.input_count;
  remaining_output_length -= r.output_count;
  if (r.error != simdutf::error_code::SUCCESS) {
    // There is an error. We return.
    if (decode_up_to_bad_char &&
        r.error == error_code::INVALID_BASE64_CHARACTER) {
      return slow_base64_to_binary_safe_impl(
          input, length, output, outlen, options, last_chunk_handling_options);
    }
    outlen = output_position;
    return {r.error, input_position};
  }

  if (done_with_partial) {
    // We are done. We have decoded everything.
    outlen = output_position;
    return {simdutf::error_code::SUCCESS, input_position};
  }
  // We have decoded some data, but we still have some data to decode.
  // We need to decode the rest of the input buffer.
  r = simdutf::scalar::base64::base64_to_binary_details_safe_impl(
      input + input_position, remaining_input_length, output + output_position,
      remaining_output_length, options, last_chunk_handling_options);
  input_position += r.input_count;
  output_position += r.output_count;
  remaining_input_length -= r.input_count;
  remaining_output_length -= r.output_count;

  if (r.error != simdutf::error_code::SUCCESS) {
    // There is an error. We return.
    if (decode_up_to_bad_char &&
        r.error == error_code::INVALID_BASE64_CHARACTER) {
      return slow_base64_to_binary_safe_impl(
          input, length, output, outlen, options, last_chunk_handling_options);
    }
    outlen = output_position;
    return {r.error, input_position};
  }
  if (input_position < length) {
    // We cannot process the entire input in one go, so we need to
    // process it in two steps: first the fast path, then the slow path.
    // In some cases, the processing might 'eat up' trailing ignorable
    // characters in the fast path, but that can be a problem.
    // suppose we have just white space followed by a single base64 character.
    // If we first process the white space with the fast path, it will
    // eat all of it. But, by the JavaScript standard, we should consume
    // no character. See
    // https://tc39.es/proposal-arraybuffer-base64/spec/#sec-frombase64
    while (input_position > 0 &&
           base64_ignorable(input[input_position - 1], options)) {
      input_position--;
    }
  }
  outlen = output_position;
  return {simdutf::error_code::SUCCESS, input_position};
}

} // namespace simdutf
#endif // SIMDUTF_BASE64_IMPLEMENTATION_H
/* end file include/simdutf/base64_implementation.h */

namespace simdutf {
  #if SIMDUTF_SPAN
/**
 * @brief span overload
 * @return a tuple of result and outlen
 */
simdutf_really_inline
    simdutf_constexpr23 simdutf_warn_unused std::tuple<result, std::size_t>
    base64_to_binary_safe(
        const detail::input_span_of_byte_like auto &input,
        detail::output_span_of_byte_like auto &&binary_output,
        base64_options options = base64_default,
        last_chunk_handling_options last_chunk_options = loose,
        bool decode_up_to_bad_char = false) noexcept {
  size_t outlen = binary_output.size();
    #if SIMDUTF_CPLUSPLUS23
  if consteval {
    using CInput = std::decay_t<decltype(*input.data())>;
    static_assert(std::is_same_v<CInput, char>,
                  "sorry, the constexpr implementation is for now limited to "
                  "input of type char");
    using COutput = std::decay_t<decltype(*binary_output.data())>;
    static_assert(std::is_same_v<COutput, char>,
                  "sorry, the constexpr implementation is for now limited to "
                  "output of type char");
    auto r = base64_to_binary_safe_impl(
        input.data(), input.size(), binary_output.data(), outlen, options,
        last_chunk_options, decode_up_to_bad_char);
    return {r, outlen};
  } else
    #endif
  {
    auto r = base64_to_binary_safe_impl<char>(
        reinterpret_cast<const char *>(input.data()), input.size(),
        reinterpret_cast<char *>(binary_output.data()), outlen, options,
        last_chunk_options, decode_up_to_bad_char);
    return {r, outlen};
  }
}

    #if SIMDUTF_SPAN
/**
 * @brief span overload
 * @return a tuple of result and outlen
 */
simdutf_really_inline
    simdutf_warn_unused simdutf_constexpr23 std::tuple<result, std::size_t>
    base64_to_binary_safe(
        std::span<const char16_t> input,
        detail::output_span_of_byte_like auto &&binary_output,
        base64_options options = base64_default,
        last_chunk_handling_options last_chunk_options = loose,
        bool decode_up_to_bad_char = false) noexcept {
  size_t outlen = binary_output.size();
      #if SIMDUTF_CPLUSPLUS23
  if consteval {
    auto r = base64_to_binary_safe_impl(
        input.data(), input.size(), binary_output.data(), outlen, options,
        last_chunk_options, decode_up_to_bad_char);
    return {r, outlen};
  } else
      #endif
  {
    auto r = base64_to_binary_safe(
        input.data(), input.size(),
        reinterpret_cast<char *>(binary_output.data()), outlen, options,
        last_chunk_options, decode_up_to_bad_char);
    return {r, outlen};
  }
}
    #endif // SIMDUTF_SPAN

  #endif // SIMDUTF_SPAN
} // namespace simdutf

#if SIMDUTF_CPLUSPLUS23 && SIMDUTF_FEATURE_BASE64

namespace simdutf {
namespace literals {

namespace detail {

// the detail namespace is not part of the public api

template <std::size_t N> struct base64_literal_helper {
  std::array<char, N - 1> storage{};
  static constexpr std::size_t size() noexcept { return N - 1; }
  consteval base64_literal_helper(const char (&str)[N]) {
    for (std::size_t i = 0; i < size(); i++) {
      storage[i] = str[i];
    }
  }
};

template <std::size_t InputLen> struct base64_decode_result {
  static constexpr std::size_t max_out = (InputLen + 3) / 4 * 3;
  std::array<char, max_out> buffer{};
  std::size_t output_count{};
};

template <std::size_t InputLen>
consteval auto base64_decode_literal(const char *str) {
  base64_decode_result<InputLen> result{};
  auto r = scalar::base64::base64_to_binary_details_impl(
      str, InputLen, result.buffer.data(), base64_default, loose);
  if (r.error != error_code::SUCCESS) {
    std::unreachable(); // invalid base64 input in _base64 literal
  }
  result.output_count = r.output_count;
  return result;
}

template <base64_literal_helper a> consteval auto base64_make_array() {
  constexpr auto decoded = base64_decode_literal<a.size()>(a.storage.data());
  std::array<char, decoded.output_count> ret{};
  for (std::size_t i = 0; i < decoded.output_count; i++) {
    ret[i] = decoded.buffer[i];
  }
  return ret;
}

} // namespace detail

/**
 * User-defined literal for compile-time base64 decoding.
 *
 * Usage:
 *   using namespace simdutf::literals;
 *   constexpr auto decoded = "SGVsbG8gV29ybGQh"_base64;
 *   // decoded is a std::array<char, 12> containing "Hello World!"
 *
 * The input must be valid base64. Whitepace is allowed and ignored.
 * A compilation error occurs if the input is invalid.
 */
template <detail::base64_literal_helper a> consteval auto operator""_base64() {
  return detail::base64_make_array<a>();
}

} // namespace literals
} // namespace simdutf

#endif // SIMDUTF_CPLUSPLUS23 && SIMDUTF_FEATURE_BASE64

#endif // SIMDUTF_IMPLEMENTATION_H
/* end file include/simdutf/implementation.h */

// Implementation-internal files (must be included before the implementations
// themselves, to keep amalgamation working--otherwise, the first time a file is
// included, it might be put inside the #ifdef
// SIMDUTF_IMPLEMENTATION_ARM64/FALLBACK/etc., which means the other
// implementations can't compile unless that implementation is turned on).

SIMDUTF_POP_DISABLE_WARNINGS

#endif // SIMDUTF_H
/* end file include/simdutf.h */
