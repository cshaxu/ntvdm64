/* Original exported algorithms with a deliberately rejecting CallBack16
 * dependency. Reuse existing resolver/link seams, not its frame-test main.
 * This is an isolated result/prevalidation unit, not execution acceptance. */
#define main wow_frame_unit_not_run
#include "wow_original_callback_frame_fixture.c"
#undef main
#include "wow_user_native_extension.h"
static unsigned rejected;
/* The complete original TU retains the frame body beside these public APIs.
 * Rejected admission must not enter that body or either recovery service. */
void __cdecl wow_callback_control_capture(wow_callback_control *control)
{(void)control;ExitProcess(94);}
int __cdecl wow_callback_control_restore(const wow_callback_control *control,uint32_t stack)
{(void)control;(void)stack;ExitProcess(94);return 0;}
BOOL WINAPI wow_test_reject_callback(INT id, PPARM16 args, VPPROC procedure, PVPVOID output)
{
    (void)args;(void)procedure;
    if(id != RET_WOWCALLBACK16 || !output || *output != sizeof(DWORD)) ++errors;
    ++rejected;SetLastError(ERROR_INVALID_STATE);return FALSE;
}
int __cdecl main(void)
{
    DWORD result = 0x447affff, arguments[4] = {0};
    if(WOWCallback16(0x12370020,0x12345678) || rejected != 1 || GetLastError() != ERROR_INVALID_STATE) return 1;
    if(WOWCallback16Ex(0x12370020,0,sizeof(DWORD),arguments,&result) ||
            rejected != 2 || result != sizeof(DWORD) || GetLastError() != ERROR_INVALID_STATE) return 2;
    result = 0x447affff;
    if(WOWCallback16Ex(0,0,MAXDWORD,NULL,&result) || rejected != 2 ||
            result != 0x447affff || GetLastError() != ERROR_INVALID_PARAMETER || errors) return 3;
    puts("WOW_PUBLIC_CALLBACK_REJECTION errors=0 dword_zero=1 bool_false=1 original_store=1 invalid_size_untouched=1 no_guest_entry=1");
    return 0;
}
