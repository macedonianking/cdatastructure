#ifndef GN_EXAMPLE_PRINT_HELLO_COMPILER_H
#define GN_EXAMPLE_PRINT_HELLO_COMPILER_H

#if defined(WIN32) && !defined(__GNUC__)
#define PRINT_HELLO_EXPORT __declspec(dllexport)
#define PRINT_HELLO_IMPORT __declspec(dllimport)
#else
#define PRINT_HELLO_EXPORT __attribute((visibility("default")))
#define PRINT_HELLO_IMPORT __attribute((visibility("default")))
#endif

#ifdef PRINT_HELLO_IMPLEMENTATION
#define PRINT_HELLO_DECL PRINT_HELLO_EXPORT
#else
#define PRINT_HELLO_DECL PRINT_HELLO_IMPORT
#endif

#endif
