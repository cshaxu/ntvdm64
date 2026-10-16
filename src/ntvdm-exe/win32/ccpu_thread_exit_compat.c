#include "thread_start_compat.h"

/* Original ntthread.c remains the sole CPU-stack/list release owner.
 * Only nt_thred.c's caller is redirected by the product build graph. */
extern void __cdecl ccpu386exitthread(void);

void __cdecl opennt_ccpu_exit_thread(void)
{
    opennt_prepare_thread_exit();
    ccpu386exitthread();
}
