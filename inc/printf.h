#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <stdarg.h>
#include <stddef.h>

int vprintf(const char * format, va_list arg);
int printf(const char * format, ... );

int snprintf (char * s, size_t n, const char * format, ...);
int vsnprintf (char * s, size_t n, const char * format, va_list arg);

#endif /*__PRINTF_H__*/