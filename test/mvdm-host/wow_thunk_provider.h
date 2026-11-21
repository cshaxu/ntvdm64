#include "../../src/compat/include/wow_thunk_provider.h"
INT WINAPI wow_original_dispatch_exception(DWORD, PEXCEPTION_POINTERS, struct _TD *, DWORD, BOOL);
VOID WINAPI wow_original_thunk_capture(wow_thunk_mode *);
VOID WINAPI wow_original_thunk_restore(const wow_thunk_mode *);
VOID WINAPI wow_original_thunk_frame(struct _VDMFRAME *, wow_thunk_frame *);
static const wow_thunk_provider thunk_provider = {
    wow_original_dispatch_exception, wow_original_thunk_capture,
    wow_original_thunk_restore, wow_original_thunk_frame };
