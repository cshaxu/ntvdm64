/* Execute the complete production dialog scope, with only its capture leaf
 * able to raise. No uncaught native WNDPROC exception is forced through USER. */
#include "wow_dialog_context_bindings.h"
unsigned wow_dialog_throw_capture,wow_dialog_capture_exceptions;
static BOOL WINAPI capture_fault(wow_dialog_context *context,BOOL font)
{
    BOOL result=wow_dialog_capture_context(context,font);
    if(result && wow_dialog_throw_capture){
        wow_dialog_throw_capture=0;++wow_dialog_capture_exceptions;
        SetLastError(0x44730012);RaiseException(0xe0420059,0,0,NULL);
    }
    return result;
}
#define wow_dialog_capture_context capture_fault
#include "wow32-dll/source/wow_dialog_finalization_binding.c"
#undef wow_dialog_capture_context
