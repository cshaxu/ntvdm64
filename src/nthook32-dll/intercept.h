#ifndef NTHOOK32_INTERCEPT_H
#define NTHOOK32_INTERCEPT_H
#include "hook.h"
extern nthook_context nthook_process_context;
LONG nthook_attach(void);
LONG nthook_attach_titles(void); /* joins the caller's Detours transaction */
#endif
