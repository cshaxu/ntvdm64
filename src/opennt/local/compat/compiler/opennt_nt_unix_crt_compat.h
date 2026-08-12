/*
 * Declaration-order bridge for the retained NT4 nt_unix.c host source.
 * The original x86 CRT exposed 32-bit time_t and raw _sys_errlist/_sys_nerr
 * externals. Modern UCRT headers turn the latter into access macros. Include
 * the CRT first and remove only those spelling rewrites; this header supplies
 * no error table, clock, idle, or lifecycle implementation.
 */
#ifndef NTDOS64_OPENNT_NT_UNIX_CRT_COMPAT_H
#define NTDOS64_OPENNT_NT_UNIX_CRT_COMPAT_H

#include <stdlib.h>
#include <time.h>

#ifdef _sys_errlist
#undef _sys_errlist
#endif
#ifdef _sys_nerr
#undef _sys_nerr
#endif

#endif
