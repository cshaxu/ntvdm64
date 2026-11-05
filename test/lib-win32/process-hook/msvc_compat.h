#ifndef PROCESS_HOOK_TEST_MSVC_COMPAT_H
#define PROCESS_HOOK_TEST_MSVC_COMPAT_H
/* Injected only into the shared startup-hook test translation unit. */
#if defined(_MSC_VER) && !defined(__clang__)
#define __builtin_unreachable() __assume(0)
#endif
#endif
