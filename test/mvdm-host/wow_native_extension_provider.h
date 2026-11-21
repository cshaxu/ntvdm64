#include "../../src/compat/include/wow_native_extension_provider.h"
extern BOOL WINAPI wow_original_kernel_resource_return(INT);
extern BOOL WINAPI wow_original_callback_guarded(INT, union _PARM16 *, DWORD, DWORD *);
extern DWORD WINAPI wow_original_WK32ICallProc32(struct _VDMFRAME *);
extern ULONG WINAPI wow_original_WMM32CallProc32(struct _VDMFRAME *);
extern VOID WINAPI wow_original_WOWYield16(void);
extern VOID WINAPI wow_original_WOWDirectedYield16(WORD);
static const wow_native_extension_provider fixture_extension_provider = {
    wow_original_kernel_resource_return, wow_original_callback_guarded,
    wow_original_WK32ICallProc32, wow_original_WMM32CallProc32,
    wow_original_WOWYield16, wow_original_WOWDirectedYield16 };
