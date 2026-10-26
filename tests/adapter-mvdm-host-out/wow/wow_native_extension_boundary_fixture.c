/* Supplementary production-wrapper unit, not a guest/scheduler substitute.
 * Original delegates and scope owners are controlled here; real guest and
 * existing original-body/CCPU groups must separately prove their composition. */
#include "precomp.h"
#include "wow_user_native_extension.h"
#include "wow_user_runtime.h"
#include "wow_user_native_call.h"
#include <stdio.h>

static wow_user_runtime_thread binding;
static wow_task_order_thread thread;
static wow_task_order_entry task;
static unsigned errors, native_depth, ready_depth, starts, finishes, callbacks;
static unsigned callback_entries, resource_entries, yields, directed;
static BOOL reject_entry, throw_callback, throw_native;
static PVDMFRAME expected_frame;
static PPARM16 expected_parameters;
static VPPROC expected_procedure;
static PVPVOID expected_output;
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr,"FAIL line=%u %s\n",__LINE__,#x); } } while (0)

wow_user_runtime_thread *WINAPI wow_user_runtime_current(void) { return &binding; }
BOOL WINAPI wow_user_native_call_begin_inline(wow_user_native_call *call)
{
    ZeroMemory(call, sizeof(*call));
    if (reject_entry) { SetLastError(ERROR_BUSY); return FALSE; }
    call->binding = &binding; ++native_depth; ++starts;
    return TRUE;
}
BOOL WINAPI wow_user_native_call_end(wow_user_native_call *call)
{
    CHECK(call->binding == &binding && native_depth && !ready_depth);
    --native_depth; ++finishes; call->binding = NULL;
    SetLastError(0x7002); /* Production wrapper must preserve delegate error. */
    return TRUE;
}
__declspec(noreturn) void WINAPI wow_user_native_call_failed(const char *stage, DWORD error)
{
    fprintf(stderr,"Unexpected terminal boundary stage=%s error=%lu\n",stage,error);
    ExitProcess(97);
}
static BOOL enter(wow_task_callback_scope *scope, BOOL resource)
{
    ZeroMemory(scope,sizeof(*scope));
    if (reject_entry || ((thread.TIF_flags & WOW_TASK_TIF_INCLEANUP) &&
            (!resource || !binding.resource_cleanup_live))) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    scope->binding = &binding; ++ready_depth;
    if (resource) ++resource_entries; else ++callback_entries;
    return TRUE;
}
BOOL WINAPI wow_task_callback_enter(wow_task_callback_scope *scope) { return enter(scope,FALSE); }
BOOL WINAPI wow_task_resource_cleanup_enter(wow_task_callback_scope *scope) { return enter(scope,TRUE); }
BOOL WINAPI wow_task_callback_leave(wow_task_callback_scope *scope)
{
    CHECK(scope->binding == &binding && ready_depth);
    scope->binding = NULL; --ready_depth; SetLastError(0x7003);
    return TRUE;
}
BOOL WINAPI wow_original_CallBack16(INT id, PPARM16 parameters, VPPROC procedure, PVPVOID output)
{
    CHECK(ready_depth && parameters == expected_parameters &&
        procedure == expected_procedure && output == expected_output);
    CHECK(id >= 0 && id <= RET_MAX);
    ++callbacks;
    SetLastError(0x1234);
    if (throw_callback) RaiseException(0xe4470079u,0,0,NULL);
    if (output) *output = 0x447a1234u;
    return TRUE;
}
VOID WINAPI wow_original_WOWYield16(VOID) { CHECK(ready_depth); ++yields; SetLastError(0x2345); }
VOID WINAPI wow_original_WOWDirectedYield16(WORD target)
{ CHECK(ready_depth && target == 0x4321); ++directed; SetLastError(0x2346); }
DWORD FASTCALL wow_original_WK32ICallProc32(PVDMFRAME frame)
{
    CHECK(native_depth == 1 && !ready_depth && frame == expected_frame);
    WOWYield16(); CHECK(!ready_depth && GetLastError() == 0x2345);
    WOWDirectedYield16(0x4321); CHECK(!ready_depth && GetLastError() == 0x2346);
    CHECK(CallBack16(RET_WOWCALLBACK16,expected_parameters,expected_procedure,expected_output));
    CHECK(!ready_depth && GetLastError() == 0x1234);
    SetLastError(0x3456); return 0x447a2001;
}
#ifndef WOW_EXTENSION_ORIGINAL_MM
ULONG FASTCALL wow_original_WMM32CallProc32(PVDMFRAME frame)
{
    CHECK(native_depth == 1 && !ready_depth && frame == expected_frame);
    SetLastError(0x4567);
    if (throw_native) RaiseException(0xe4470079u,0,0,NULL);
    return 0x447a2002;
}
#else
static unsigned directory_calls, mm_calls;
/* Complete wmmedia.c also defines public memory/GDI resolvers. MSVC resolves
 * their outgoing references before dead stripping this direct object. They
 * are outside this original-MM-call test: fail immediately if reached,
 * never provide a fabricated successful pointer or handle. Real resolver
 * behavior remains covered by the separate CCPU/guest suites. */
PVOID FASTCALL GetPModeVDMPointerAssert(DWORD address)
{ (void)address; ExitProcess(98); return NULL; }
WORD __cdecl mvdm_wow_gdi_handle_from_native(HANDLE handle)
{ (void)handle; ExitProcess(98); return 0; }
HANDLE __cdecl mvdm_wow_gdi_handle_to_native(WORD handle)
{ (void)handle; ExitProcess(98); return NULL; }
BOOL UpdateDosCurrentDirectory(UDCDFUNC direction)
{
    CHECK(native_depth == 1 && direction == DIR_DOS_TO_NT);
    ++directory_calls;return TRUE;
}
static DWORD WINAPI native_mm_target(DWORD first, DWORD second, DWORD third,
    DWORD fourth, DWORD fifth)
{
    CHECK(native_depth == 1 && !ready_depth && first == 1 && second == 2 &&
        third == 3 && fourth == 4 && fifth == 5 && directory_calls == mm_calls + 1);
    ++mm_calls;SetLastError(0x4567);
    if(throw_native) RaiseException(0xe4470079u,0,0,NULL);
    return 0x447a2002;
}
#endif

int __cdecl main(void)
{
    PARM16 parameters;
    union { ULONGLONG alignment; BYTE bytes[sizeof(VDMFRAME)+sizeof(MMCALLPROC3216)]; } frame_storage;
    PVDMFRAME frame = (PVDMFRAME)frame_storage.bytes;
#ifdef WOW_EXTENSION_ORIGINAL_MM
    PMMCALLPROC3216 mm = (PMMCALLPROC3216)(frame_storage.bytes + OFFSETOF(VDMFRAME,bArgs));
#endif
    VPVOID output = 0;
    HMODULE module;
    HMODULE winmm;
    FARPROC private_address;
    FARPROC address;
    FARPROC native_private_address;
    FARPROC native_entry;
    FARPROC extension_entry;
    typedef DWORD (WINAPI *tid32_message)(DWORD, DWORD, DWORD, DWORD, DWORD);
    static const char * const private_mm_entries[] = {
        "NotifyCallbackData", "wod32Message", "wid32Message", "mod32Message",
        "mid32Message", "aux32Message", "mci32Message", "tid32Message",
        "joy32Message", "WOW32DriverCallback", "WOW32ResolveMultiMediaHandle"
    };
    unsigned before, i, private_index;
    const INT helpers[] = {RET_LOCALFREE,RET_GLOBALUNLOCKFREE,RET_LOCKRESOURCE,
        RET_FORCESEGMENTFAULT,RET_GETEXPWINVER,RET_WOWDDEFREEHANDLE,RET_FREEDIBSEL,
        RET_GETCURDIR,RET_SETCURDIR};
    const INT app[] = {RET_WOWCALLBACK16,RET_WNDPROC,RET_HOOKPROC,RET_WINEXEC,RET_FOREGROUNDIDLE};
    ZeroMemory(&parameters,sizeof(parameters));ZeroMemory(&frame_storage,sizeof(frame_storage));
#ifdef WOW_EXTENSION_ORIGINAL_MM
    mm->fSetCurrentDirectory=1;mm->lpProcAddress=(DWORD)(ULONG_PTR)native_mm_target;
    mm->p1=5;mm->p2=4;mm->p3=3;mm->p4=2;mm->p5=1;
#endif
    binding.thread = &thread;thread.ptdb = &task;
    expected_frame = frame;expected_parameters = &parameters;
    expected_procedure = (VPPROC)0x12370020;expected_output = &output;
    CHECK(WK32ICallProc32(frame) == 0x447a2001 && GetLastError() == 0x3456);
    CHECK(output == 0x447a1234 && yields == 1 && directed == 1 && callbacks == 1);
    CHECK(WMM32CallProc32(frame) == 0x447a2002 && GetLastError() == 0x4567);
    CHECK(!native_depth && !ready_depth && starts == finishes);
    throw_native = TRUE;
    __try { (void)WMM32CallProc32(frame); CHECK(FALSE); }
    __except(GetExceptionCode()==0xe4470079u?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH) {}
    throw_native = FALSE;
    CHECK(!native_depth && !ready_depth && starts == finishes);
#ifdef WOW_EXTENSION_ORIGINAL_MM
    {
        HMODULE timer_module;
        FARPROC timer_target;
        DWORD first, second;
        timer_module = wow_extension_load_library_a("winmm.dll",NULL,
            LOAD_LIBRARY_SEARCH_SYSTEM32);
        timer_target = timer_module ? wow_extension_get_proc_address(timer_module,
            "tid32Message") : NULL;
        CHECK(timer_target != NULL);
        mm->fSetCurrentDirectory=0;
        mm->lpProcAddress=(DWORD)(ULONG_PTR)timer_target;
        mm->p1=0x000A;mm->p2=0;mm->p3=0;mm->p4=0;mm->p5=0;
        first=WMM32CallProc32(frame);second=WMM32CallProc32(frame);
        CHECK(second >= first);
        if(timer_module) CHECK(wow_extension_free_library(timer_module));
    }
    CHECK(directory_calls == 2 && mm_calls == 2);
    mm->lpProcAddress=0;SetLastError(0x5678);
    CHECK(!WMM32CallProc32(frame) && GetLastError() == 0x5678 && directory_calls == 2 && mm_calls == 2);
    mm->lpProcAddress=(DWORD)(ULONG_PTR)native_mm_target;
    puts("WOW_ORIGINAL_MM_EXTENSION errors=0 actual_body=1 five_args=1 directory_order=1 zero_target=1 exception=1 tid32_through_original=1");
#endif
    before = callbacks;throw_callback = TRUE;
    __try { (void)CallBack16(RET_WOWCALLBACK16,&parameters,expected_procedure,&output); CHECK(FALSE); }
    __except(GetExceptionCode()==0xe4470079u?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH) {}
    throw_callback = FALSE;
    CHECK(callbacks == before + 1 && !ready_depth);
    thread.TIF_flags = WOW_TASK_TIF_INCLEANUP;binding.resource_cleanup_live = TRUE;
    before = resource_entries;
    for(i=0;i<ARRAYSIZE(helpers);++i) CHECK(CallBack16(helpers[i],&parameters,expected_procedure,&output));
    CHECK(resource_entries == before + ARRAYSIZE(helpers));
    before = callbacks;
    for(i=0;i<ARRAYSIZE(app);++i) CHECK(!CallBack16(app[i],&parameters,expected_procedure,&output));
    CHECK(callbacks == before && GetLastError() == ERROR_INVALID_STATE);
    binding.resource_cleanup_live = FALSE;
    CHECK(!CallBack16(RET_LOCALFREE,&parameters,expected_procedure,&output) && callbacks == before);
    thread.TIF_flags = 0;
    reject_entry = TRUE;before = starts;
    output = 0xdeadbeef;
    CHECK(!CallBack16(RET_WOWCALLBACK16,&parameters,expected_procedure,&output) && output == 0xdeadbeef);
    WOWYield16();WOWDirectedYield16(0x4321);
    CHECK(yields == 1 && directed == 1 && !ready_depth);
    CHECK(!WK32ICallProc32(frame) && GetLastError() == ERROR_BUSY && starts == before);
    CHECK(!wow_extension_load_library_a("T447-never.dll",NULL,0) && GetLastError() == ERROR_BUSY);
    CHECK(!wow_extension_free_library((HMODULE)(ULONG_PTR)1) && GetLastError() == ERROR_BUSY);
    CHECK(!wow_extension_get_proc_address((HMODULE)(ULONG_PTR)1,"none") && GetLastError() == ERROR_BUSY);
    reject_entry = FALSE;
    module = wow_extension_load_library_a("version.dll",NULL,LOAD_LIBRARY_SEARCH_SYSTEM32);
    CHECK(module != NULL);
    address = module ? GetProcAddress(module,"GetFileVersionInfoSizeA") : NULL;
    CHECK(address && wow_extension_get_proc_address(module,"GetFileVersionInfoSizeA") == address);
    if(module) CHECK(!wow_extension_get_proc_address(module,"T447AbsentExport") && GetLastError() == ERROR_PROC_NOT_FOUND);
    winmm = wow_extension_load_library_a("winmm.dll",NULL,LOAD_LIBRARY_SEARCH_SYSTEM32);
    CHECK(winmm != NULL);
    native_private_address = winmm ? GetProcAddress(winmm,"tid32Message") : NULL;
    CHECK(native_private_address != NULL);
    private_address = winmm ? wow_extension_get_proc_address(winmm,"tid32Message") : NULL;
    CHECK(private_address == native_private_address);
    for (private_index = 0; private_index < ARRAYSIZE(private_mm_entries); ++private_index) {
        native_entry = GetProcAddress(winmm, private_mm_entries[private_index]);
        extension_entry = wow_extension_get_proc_address(winmm, private_mm_entries[private_index]);
        CHECK(native_entry != NULL && extension_entry == native_entry);
    }
    if(private_address) {
        DWORD first = ((tid32_message)private_address)(0,0,0,0,0x000A);
        DWORD second = ((tid32_message)private_address)(0,0,0,0,0x000A);
        CHECK(second >= first);
    }
    if(winmm) CHECK(wow_extension_free_library(winmm));
    if(module) CHECK(wow_extension_free_library(module));
    CHECK(!native_depth && !ready_depth && starts == finishes);
    printf("WOW_NATIVE_EXTENSION_BOUNDARY errors=%u inline_pairs=%u callbacks=%u ready_entries=%u resource_entries=%u unwind=1 reject=1 result_error=1 native_loader=1 tid32_native_sync=1 mm_private_exports=11\n",
        errors,starts,callbacks,callback_entries,resource_entries);
    return errors ? 1 : 0;
}
