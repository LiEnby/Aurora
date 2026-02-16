#ifndef _COMPAT_H
#define _COMPAT_H 1

#ifdef _DEBUG
#include <stdio.h>
#define print(...) printf(__VA_ARGS__)
#else
#define print(...) /**/
#endif

#ifdef _WIN32
#define WIN32_MEAN_AND_LEAN 1
#define unsetenv(var) _putenv_s(var, "");
#define strdup _strdup
#endif

#ifdef __GNUC__
#define PACK( declaration ) declaration __attribute__((__packed__))
#elif _MSC_VER
#define PACK( declaration ) __pragma(pack(push, 1) ) declaration __pragma(pack(pop))
#endif


#endif