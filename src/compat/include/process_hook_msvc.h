#ifndef MVDM_PROCESS_HOOK_MSVC_COMPAT_H
#define MVDM_PROCESS_HOOK_MSVC_COMPAT_H
/* Injected only into the unchanged win16style startup-hook translation unit. */
#if defined(_MSC_VER) && !defined(__clang__)
#define __builtin_unreachable() __assume(0)
#endif
#endif
