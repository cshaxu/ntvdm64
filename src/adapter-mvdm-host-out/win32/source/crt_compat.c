/*
 * DIVERGENCE(ADAPTER-WIN32-036): selected OpenNT source retains the NT4 CRT
 * `sprintf` and `sscanf` entrypoint spellings, while the selected original
 * fprt.c remains the owner of its paired printf/fprintf interposition.  The
 * current UCRT no longer exports those two old spellings independently.
 * Keep the original unbounded formatting/scanning contract and bind only at
 * the modern UCRT ABI below; callers and their control flow stay unchanged.
 */
#include <stdarg.h>
#include <stddef.h>

extern int __cdecl __stdio_common_vsprintf(unsigned __int64 options,
                                            char *buffer,
                                            size_t buffer_count,
                                            const char *format,
                                            void *locale,
                                            va_list arguments);
extern int __cdecl __stdio_common_vsscanf(unsigned __int64 options,
                                           const char *buffer,
                                           size_t buffer_count,
                                           const char *format,
                                           void *locale,
                                           va_list arguments);

int __cdecl sprintf(char *buffer, const char *format, ...)
{
    va_list arguments;
    int result;

    va_start(arguments, format);
    result = __stdio_common_vsprintf(0u, buffer, (size_t)-1, format, NULL, arguments);
    va_end(arguments);
    return result;
}

int __cdecl sscanf(const char *buffer, const char *format, ...)
{
    va_list arguments;
    int result;

    va_start(arguments, format);
    result = __stdio_common_vsscanf(0u, buffer, (size_t)-1, format, NULL, arguments);
    va_end(arguments);
    return result;
}
