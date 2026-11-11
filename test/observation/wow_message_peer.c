/* Independent native peer for win16_message_probe.asm. Loaded by the real
 * original KERNEL generic thunk. It neither binds a WOW task nor substitutes
 * a provider. Its window is a test endpoint, never a product helper HWND. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dde.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(M4_TIMEOUT_PAYLOAD) && !defined(M4_REPLY_PAYLOAD)
#error Timeout payload group requires the complete early-reply group.
#endif
#ifdef M4_REPLY_PAYLOAD
#ifndef M4_STRUCTURES
#error Reply payload group requires the full normal structure group.
#endif
#include "wow_reply_payload_lifetime.h"
static reply_payload_lifetime reply_payload;
static volatile LONG reply_driver_done;
#endif

#ifndef MESSAGE_EXTERNAL_PROGRAM
static HANDLE peer_thread;
#endif
static HANDLE peer_ready;
static HWND peer_window;
#ifdef CLASSIC_DDE
static HANDLE dde_ack_event;
static HANDLE dde_reverse_event;
static HANDLE dde_initiate_event;
static HGLOBAL dde_payload;
static volatile LONG dde_ack, dde_released, dde_retired;
static volatile LONG dde_initiate_ack;
#ifdef CLASSIC_DDE_TASK_LOSS
static volatile LONG dde_task_loss_free;
#endif
static ATOM dde_initiate_service, dde_initiate_topic;
static volatile LONG dde_reverse_received;
static volatile LONG dde_native_control;
static HGLOBAL dde_observed_packet;
static HGLOBAL (WINAPI *dde_real_free)(HGLOBAL);
static ULONG_PTR *dde_free_slot;
static volatile LONG dde_free_calls;
static HGLOBAL dde_free_result;
static ULONG_PTR *dde_post_slot, *dde_lparam_slot;
static BOOL (WINAPI *dde_real_post)(HWND,UINT,WPARAM,LPARAM);
static BOOL (WINAPI *dde_real_free_lparam)(UINT,LPARAM);
static volatile LONG dde_failed_posts, dde_freed_lparams;
static LPARAM dde_failed_lparam;
static HWND dde_failed_window;

static ULONG_PTR *dde_find_import(const char *wanted)
{
    BYTE *base=(BYTE *)GetModuleHandleA("WOW32.DLL");
    IMAGE_DOS_HEADER *dos;
    IMAGE_NT_HEADERS32 *nt;
    IMAGE_IMPORT_DESCRIPTOR *desc;
    if(!base) return NULL;
    dos=(IMAGE_DOS_HEADER *)base;
    nt=(IMAGE_NT_HEADERS32 *)(base+dos->e_lfanew);
    if(dos->e_magic!=IMAGE_DOS_SIGNATURE || nt->Signature!=IMAGE_NT_SIGNATURE ||
       !nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress) return NULL;
    desc=(IMAGE_IMPORT_DESCRIPTOR *)(base+nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    for(;desc->Name;desc++) {
        IMAGE_THUNK_DATA32 *names,*slots;
        if(!desc->OriginalFirstThunk) continue;
        names=(IMAGE_THUNK_DATA32 *)(base+desc->OriginalFirstThunk);
        slots=(IMAGE_THUNK_DATA32 *)(base+desc->FirstThunk);
        for(;names->u1.AddressOfData;names++,slots++) {
            IMAGE_IMPORT_BY_NAME *name;
            if(IMAGE_SNAP_BY_ORDINAL32(names->u1.Ordinal)) continue;
            name=(IMAGE_IMPORT_BY_NAME *)(base+names->u1.AddressOfData);
            if(strcmp((const char *)name->Name,wanted)==0) return (ULONG_PTR *)&slots->u1.Function;
        }
    }
    return NULL;
}

static BOOL dde_replace_import(ULONG_PTR *slot,ULONG_PTR value)
{
    DWORD old,ignored;
    if(!slot || !VirtualProtect(slot,sizeof(*slot),PAGE_READWRITE,&old)) return FALSE;
    *slot=value;
    return VirtualProtect(slot,sizeof(*slot),old,&ignored);
}

static BOOL WINAPI dde_fail_post(HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam)
{
    UINT_PTR lo,hi;
    /* WOW's converted HWND may be the 16-bit value rather than the full
     * native handle; restrict the injection to this test peer in either form. */
    if(message==WM_DDE_DATA && LOWORD((ULONG_PTR)hwnd)==LOWORD((ULONG_PTR)peer_window)) {
        if(UnpackDDElParam(message,lparam,&lo,&hi)) dde_observed_packet=(HGLOBAL)lo;
        dde_failed_window=hwnd;
        dde_failed_lparam=lparam;
        InterlockedIncrement(&dde_failed_posts);
        SetLastError(ERROR_ACCESS_DENIED);
        return FALSE;
    }
    return dde_real_post(hwnd,message,wparam,lparam);
}

static BOOL WINAPI dde_observe_free_lparam(UINT message,LPARAM lparam)
{
    BOOL result=dde_real_free_lparam(message,lparam);
    if(message==WM_DDE_DATA && lparam==dde_failed_lparam && result)
        InterlockedIncrement(&dde_freed_lparams);
    return result;
}

static BOOL dde_switch_failed_post(BOOL enable)
{
    union { ULONG_PTR value; BOOL (WINAPI *function)(HWND,UINT,WPARAM,LPARAM); } post;
    union { ULONG_PTR value; BOOL (WINAPI *function)(UINT,LPARAM); } lparam;
    if(enable) {
        if(dde_post_slot || dde_lparam_slot) return FALSE;
        dde_post_slot=dde_find_import("PostMessageA");
        dde_lparam_slot=dde_find_import("FreeDDElParam");
        if(!dde_post_slot || !dde_lparam_slot) goto fail;
        post.value=*dde_post_slot; dde_real_post=post.function;
        lparam.value=*dde_lparam_slot; dde_real_free_lparam=lparam.function;
        post.function=dde_fail_post;
        lparam.function=dde_observe_free_lparam;
        if(!dde_replace_import(dde_lparam_slot,lparam.value)) goto fail;
        if(!dde_replace_import(dde_post_slot,post.value)) {
            lparam.function=dde_real_free_lparam;
            dde_replace_import(dde_lparam_slot,lparam.value);
            goto fail;
        }
        return TRUE;
    }
    if(!dde_post_slot || !dde_lparam_slot) return FALSE;
    post.function=dde_real_post;
    lparam.function=dde_real_free_lparam;
    if(!dde_replace_import(dde_post_slot,post.value) ||
       !dde_replace_import(dde_lparam_slot,lparam.value)) return FALSE;
    dde_post_slot=NULL; dde_lparam_slot=NULL;
    return TRUE;
fail:
    dde_post_slot=NULL; dde_lparam_slot=NULL;
    return FALSE;
}

/* Test-only observation at WOW32's own import, not a product hook. The
 * original free executes normally; the freed handle is never queried. */
static HGLOBAL WINAPI dde_observe_free(HGLOBAL handle)
{
    HGLOBAL result=dde_real_free(handle);
    if(handle==dde_observed_packet) {
        dde_free_result=result;
        InterlockedIncrement(&dde_free_calls);
#ifdef CLASSIC_DDE_TASK_LOSS
        InterlockedIncrement(&dde_task_loss_free);
#endif
    }
    return result;
}

static BOOL dde_switch_free_import(BOOL enable)
{
    union { ULONG_PTR value; HGLOBAL (WINAPI *function)(HGLOBAL); } convert;
    DWORD old,ignored;
    if(enable) {
        BYTE *base=(BYTE *)GetModuleHandleA("WOW32.DLL");
        IMAGE_DOS_HEADER *dos;
        IMAGE_NT_HEADERS32 *nt;
        IMAGE_IMPORT_DESCRIPTOR *desc;
        if(!base || dde_free_slot) return FALSE;
        dos=(IMAGE_DOS_HEADER *)base;
        nt=(IMAGE_NT_HEADERS32 *)(base+dos->e_lfanew);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE || nt->Signature!=IMAGE_NT_SIGNATURE ||
           !nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress) return FALSE;
        desc=(IMAGE_IMPORT_DESCRIPTOR *)(base+nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
        for(;desc->Name;desc++) {
            IMAGE_THUNK_DATA32 *names,*slots;
            if(!desc->OriginalFirstThunk) continue;
            names=(IMAGE_THUNK_DATA32 *)(base+desc->OriginalFirstThunk);
            slots=(IMAGE_THUNK_DATA32 *)(base+desc->FirstThunk);
            for(;names->u1.AddressOfData;names++,slots++) {
                IMAGE_IMPORT_BY_NAME *name;
                if(IMAGE_SNAP_BY_ORDINAL32(names->u1.Ordinal)) continue;
                name=(IMAGE_IMPORT_BY_NAME *)(base+names->u1.AddressOfData);
                if(strcmp((const char *)name->Name,"GlobalFree")==0) {
                    dde_free_slot=(ULONG_PTR *)&slots->u1.Function;
                    convert.value=*dde_free_slot;dde_real_free=convert.function;
                    break;
                }
            }
            if(dde_free_slot) break;
        }
        if(!dde_free_slot || !dde_real_free) { dde_free_slot=NULL;return FALSE; }
        convert.function=dde_observe_free;
    } else {
        if(!dde_free_slot || !dde_real_free) return FALSE;
        convert.function=dde_real_free;
    }
    if(!VirtualProtect(dde_free_slot,sizeof(*dde_free_slot),PAGE_READWRITE,&old)) return FALSE;
    *dde_free_slot=convert.value;
    if(!VirtualProtect(dde_free_slot,sizeof(*dde_free_slot),old,&ignored)) return FALSE;
    if(!enable) dde_free_slot=NULL;
    return TRUE;
}
#endif
static HWND sender_window, receiver_window;
static DWORD sender_thread, receiver_thread, peer_thread_id;
#ifdef MESSAGE_CROSS_WORKER
static DWORD receiver_process_id;
#endif
static volatile LONG errors;
#ifdef MESSAGE_PENDING_SEND
static volatile LONG blocked_returned, blocked_nested;
static DWORD blocked_status, blocked_error, death_status, death_error;
static DWORD death_owner, death_result, stale_result;
static HWND death_window;
#endif
#ifndef MESSAGE_EXTERNAL_HOST
static volatile LONG received, nested, replied, text;
#endif
static HMODULE peer_module;
#ifdef MESSAGE_EXTERNAL_HOST
static DWORD peer_process_id;
#endif
#ifdef M4_STRUCTURES
#ifndef MESSAGE_EXTERNAL_HOST
static volatile LONG structure_received;
#ifdef M4_COPYDATA_PROC32
static volatile LONG proc32_fresh, proc32_borrowed;
#endif
static const LONG minmax_input[10] = {0x1234,2,3,4,5,6,-7,8,9,10};
C_ASSERT(sizeof(MINMAXINFO) == sizeof(minmax_input));

static void check_minmax(const MINMAXINFO *value, BOOL output)
{
    LONG words[10], expected[10];
    memcpy(words,value,sizeof(words));
    memcpy(expected,minmax_input,sizeof(expected));
    if (output) { expected[0]=0x7abc; expected[9]=-11; }
    if (memcmp(words,expected,sizeof(words))) InterlockedIncrement(&errors);
}

static void check_position(const WINDOWPOS *value, HWND target, BOOL output)
{
    if (LOWORD((DWORD_PTR)value->hwnd) != LOWORD((DWORD_PTR)target) ||
        value->hwndInsertAfter != HWND_TOP || value->x != (output ? -23 : -12) ||
        value->y != (output ? 45 : 34) || value->cx != (output ? 90 : 56) ||
        value->cy != 78 || value->flags != (output ? 0x14u : 4u))
        InterlockedIncrement(&errors);
}
#endif
#endif

#ifndef MESSAGE_EXTERNAL_PROGRAM
BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, PVOID reserved)
{
    (void)reserved;
    if (reason == DLL_PROCESS_ATTACH) peer_module = instance;
    return TRUE;
}
#endif

#ifdef M4_COPYDATA_PROC32
static void check_at(BOOL condition,int line)
{
    if(!condition){
        DWORD error=GetLastError(),written;char record[256];int length;HANDLE output;
        InterlockedIncrement(&errors);
        length=_snprintf_s(record,sizeof(record),_TRUNCATE,
            "line=%d tid=%lu sender=%lu peer=%lu sender_hwnd=%p peer_hwnd=%p structures=%ld fresh=%ld borrowed=%ld\r\n",
            line,GetCurrentThreadId(),sender_thread,peer_thread_id,
            (PVOID)sender_window,(PVOID)peer_window,structure_received,proc32_fresh,proc32_borrowed);
        output=CreateFileA("MSGPEER-FAIL.txt",FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
        if(output!=INVALID_HANDLE_VALUE){if(length>0)WriteFile(output,record,(DWORD)length,&written,NULL);CloseHandle(output);}
        SetLastError(error);
    }
}
#define check(condition) check_at((condition),__LINE__)
#else
static void check(BOOL condition)
{
    if (!condition) InterlockedIncrement(&errors);
}
#endif

#ifndef MESSAGE_EXTERNAL_HOST
static LRESULT CALLBACK peer_proc(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    DWORD_PTR value = 0;
    char buffer[32] = {0};
    if (message >= 0x470 && message <= 0x473) {
        check((InSendMessageEx(NULL) & ISMEX_SEND) != 0);
        check(GetCurrentThreadId() == peer_thread_id);
        InterlockedIncrement(&received);
    }
    switch (message) {
#ifdef CLASSIC_DDE
    case 0x487:
        return 0x447a0087;
    case WM_DDE_DATA:
    case WM_DDE_POKE:
    case WM_DDE_ADVISE:
        {
            UINT lo=0,hi=0;
            DDEDATA *data;
            check(UnpackDDElParam(message,lp,&lo,&hi));
            check(window==peer_window && (HWND)wp==sender_window && lo!=0 && hi==0);
            data=(DDEDATA *)GlobalLock((HGLOBAL)(ULONG_PTR)lo);
            check(data!=NULL);
            if(data) {
                check(data->cfFormat==CF_TEXT);
                if(message!=WM_DDE_ADVISE) check(strcmp((char *)data->Value,"T449-DDE-DATA")==0);
                GlobalUnlock((HGLOBAL)(ULONG_PTR)lo);
            }
            check(FreeDDElParam(message,lp));
            dde_observed_packet=(HGLOBAL)(ULONG_PTR)lo;
            InterlockedIncrement(&dde_reverse_received);
            check(dde_reverse_event && SetEvent(dde_reverse_event));
            return 0;
        }
    case WM_DDE_EXECUTE:
        {
            HGLOBAL payload=(HGLOBAL)lp;
            const char *reverse_text=(const char *)GlobalLock(payload);
            LPARAM ack;
            check(window==peer_window && (HWND)wp==sender_window && reverse_text!=NULL);
            if(reverse_text) {
                check(strcmp(reverse_text,"T449-DDE-REVERSE")==0);
                GlobalUnlock(payload);
            }
            if(InterlockedCompareExchange(&dde_native_control,0,0)) {
                InterlockedIncrement(&dde_reverse_received);
                check(dde_reverse_event && SetEvent(dde_reverse_event));
                return 0;
            }
            ack=PackDDElParam(WM_DDE_ACK,0x8000,(UINT_PTR)payload);
            check(ack!=0 && PostMessageA(sender_window,WM_DDE_ACK,(WPARAM)window,ack));
            InterlockedIncrement(&dde_reverse_received);
            check(dde_reverse_event && SetEvent(dde_reverse_event));
            return 0;
        }
    case WM_DDE_ACK:
        {
            UINT lo=0,hi=0;
            if (dde_initiate_event) {
                check(window==peer_window && (HWND)wp==sender_window);
                check(LOWORD((DWORD_PTR)lp)==dde_initiate_service &&
                    HIWORD((DWORD_PTR)lp)==dde_initiate_topic);
                InterlockedIncrement(&dde_initiate_ack);
                check(SetEvent(dde_initiate_event));
                return 0;
            }
            check(UnpackDDElParam(WM_DDE_ACK,lp,&lo,&hi));
            check(lo==0x8000 && (HGLOBAL)(ULONG_PTR)hi==dde_payload);
            check(FreeDDElParam(WM_DDE_ACK,lp));
            InterlockedIncrement(&dde_ack);
            check(SetEvent(dde_ack_event));
            return 0;
        }
#endif
#ifdef MESSAGE_CONSTRUCTION
    case 0x0038: {
        const DWORD *packet=(const DWORD *)lp;
        check(packet!=NULL);
        if(packet){check(LOWORD(packet[0])==512);check(packet[1]==0x11223344 && packet[127]==0x55667788);}
        return errors ? 0 : 0x447a007e;
    }
#endif
#ifdef MESSAGE_EXTERNAL_PROGRAM
    case 0x482:
        return structure_received; /* Test-only scalar completion query. */
#endif
#ifdef M4_REPLY_PAYLOAD
    case 0x481:
        {
            BOOL sent;
            check(GetCurrentThreadId() == peer_thread_id && reply_payload.page != NULL);
            sent=reply_payload_send(&reply_payload,sender_window,window);
            InterlockedExchange(&reply_driver_done,1);
            return sent ? 0x447a0055 : 0;
        }
#endif
#ifdef M4_STRUCTURES
    case WM_GETMINMAXINFO:
        if (wp != 0x4474) break;
        check_minmax((const MINMAXINFO *)lp,FALSE);
        ((MINMAXINFO *)lp)->ptReserved.x=0x7abc;
        ((MINMAXINFO *)lp)->ptMaxTrackSize.y=-11;
        InterlockedIncrement(&structure_received);
        return 0x447a0024;
    case WM_WINDOWPOSCHANGING:
        if (wp != 0x4474) break;
        check_position((const WINDOWPOS *)lp,window,FALSE);
        ((WINDOWPOS *)lp)->x=-23; ((WINDOWPOS *)lp)->y=45;
        ((WINDOWPOS *)lp)->cx=90; ((WINDOWPOS *)lp)->flags=0x14;
        InterlockedIncrement(&structure_received);
        return 0x447a0046;
    case WM_COPYDATA:
        if (((const COPYDATASTRUCT *)lp)->dwData != 0x456789ab) break;
        check(wp != 0 && ((const COPYDATASTRUCT *)lp)->cbData == 8);
        check(((const COPYDATASTRUCT *)lp)->lpData != NULL &&
            !memcmp(((const COPYDATASTRUCT *)lp)->lpData,"M4copy!!",8));
#ifdef M4_COPYDATA_PROC32
        if(GetCurrentThreadId()==sender_thread){
            /* Original HWND32 sign-extends USER16's value; original/native
             * CallWindowProc passes that argument to the procedure unchanged. */
            if(window==(HWND)(LONG_PTR)(SHORT)LOWORD(sender_window))InterlockedIncrement(&proc32_borrowed);
            else {check(window==(HWND)(LONG_PTR)(SHORT)LOWORD(peer_window));InterlockedIncrement(&proc32_fresh);}
            return 0x447a004a;
        }
#endif
        InterlockedIncrement(&structure_received);
        return 0x447a004a;
    case 0x480:
        {
            MINMAXINFO limits;
            WINDOWPOS position={sender_window,HWND_TOP,-12,34,56,78,4};
            COPYDATASTRUCT copy={0x456789ab,8,(PVOID)"M4copy!!"};
            memcpy(&limits,minmax_input,sizeof(limits));
            check((InSendMessageEx(NULL) & ISMEX_SEND) != 0);
            check(GetCurrentThreadId() == peer_thread_id);
            check(SendMessageTimeoutA(sender_window,WM_GETMINMAXINFO,0x4474,
                (LPARAM)&limits,SMTO_ABORTIFHUNG|SMTO_ERRORONEXIT,5000,&value) != 0);
            check(value == 0x447a0024); check_minmax(&limits,TRUE);
            InterlockedIncrement(&structure_received);
            check(SendMessageTimeoutA(sender_window,WM_WINDOWPOSCHANGING,0x4474,
                (LPARAM)&position,SMTO_ABORTIFHUNG|SMTO_ERRORONEXIT,5000,&value) != 0);
            check(value == 0x447a0046); check_position(&position,sender_window,TRUE);
            InterlockedIncrement(&structure_received);
            check(SendMessageTimeoutA(sender_window,WM_COPYDATA,(WPARAM)window,
                (LPARAM)&copy,SMTO_ABORTIFHUNG|SMTO_ERRORONEXIT,5000,&value) != 0);
            check(value == 0x447a004a);
            check(copy.dwData == 0x456789ab && copy.cbData == 8 &&
                !memcmp(copy.lpData,"M4copy!!",8));
            InterlockedIncrement(&structure_received);
            return 0x447a0020;
        }
#endif
    case 0x470:
        check(wp == 0x1234 && (DWORD)lp == 0x56789abc);
        return 0x447a0010;
#ifdef MESSAGE_PENDING_SEND
    case 0x483:
        SetLastError(ERROR_SUCCESS);
        blocked_status=(DWORD)SendMessageTimeoutA(sender_window,0x47b,0,0,
            SMTO_ERRORONEXIT |
#ifndef MESSAGE_NONBLOCK_CONTROL
            SMTO_BLOCK |
#endif
            SMTO_NORMAL,250,&value);
        blocked_error=GetLastError();
        check(blocked_status==0 && blocked_nested==0);
        InterlockedExchange(&blocked_returned,1);
        return 0x447a0083;
    case 0x484:
        check(GetCurrentThreadId()==peer_thread_id && InSendMessage());
        if(!InterlockedCompareExchange(&blocked_returned,0,0)){
            FILE *failure=NULL;
            if(!fopen_s(&failure,"MSGPEER-FAIL.txt","ab")){
                fputs("FAIL reverse callback before blocked send returned\n",failure);
                fclose(failure);
            }
        }
        check(InterlockedCompareExchange(&blocked_returned,0,0)==1);
        InterlockedIncrement(&blocked_nested);
        return 0x447a0084;
    case 0x485:
        check(SendMessageTimeoutA(receiver_window,0x47d,0,0,
            SMTO_ERRORONEXIT,5000,&value)!=0 && HIWORD(value)==0x447a);
        death_window=FindWindowA("T447_MSG_RECV","T447 death target");
        check(death_window!=NULL && LOWORD((DWORD_PTR)death_window)==LOWORD(value));
        check(GetWindowThreadProcessId(death_window,&death_owner)==receiver_thread &&
            death_owner==GetCurrentProcessId());
        SetLastError(ERROR_SUCCESS);
        death_status=(DWORD)SendMessageTimeoutA(death_window,0x47c,0,0,
            SMTO_ERRORONEXIT,5000,&value);
        death_error=GetLastError();
        /* Paired native controls prove that destroying the HWND inside its
         * callback does not force this completed send to fail, even with
         * ERRORONEXIT. Do not confuse dead HWND identity with API failure. */
        death_result=death_status ? (DWORD)value : 0;
        check(death_status!=0 && death_result==0x447a007c &&
            death_error==0 && !IsWindow(death_window));
        check(SendMessageTimeoutA(receiver_window,0x47e,0,0,
            SMTO_ERRORONEXIT,5000,&value)!=0);
        stale_result=(DWORD)value;check(stale_result==0x447a007e);
        return 0x447a0085;
#endif
    case 0x471:
        check(SendMessageTimeoutA(sender_window, 0x462, 0, 0x22334455,
            SMTO_ABORTIFHUNG | SMTO_ERRORONEXIT, 5000, &value) != 0);
        check(value == 0x447a0002);
        InterlockedIncrement(&nested);
        return 0x447a0011;
    case 0x472:
        check(SendMessageTimeoutA(sender_window, 0x464, 0, 0,
            SMTO_ABORTIFHUNG | SMTO_ERRORONEXIT, 5000, &value) != 0);
        check(value == 0x447a0004); /* Duplicate reply must not replace first. */
        InterlockedIncrement(&replied);
        return 0x447a0012;
    case 0x473:
        check(SendMessageTimeoutA(sender_window, WM_SETTEXT, 0,
            (LPARAM)"Native text", SMTO_ABORTIFHUNG, 5000, &value) != 0);
        check(value != 0);
        value = 0;
        check(SendMessageTimeoutA(sender_window, WM_GETTEXT, sizeof(buffer),
            (LPARAM)buffer, SMTO_ABORTIFHUNG, 5000, &value) != 0);
        check(value == 11 && strcmp(buffer, "Native text") == 0);
        InterlockedIncrement(&text);
        return 0x447a0013;
    case WM_CLOSE:
        check(DestroyWindow(window));
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(window, message, wp, lp);
}

static DWORD WINAPI peer_main(PVOID argument)
{
    WNDCLASSA cls = {0};
    MSG message;
    BOOL got;
    (void)argument;
    peer_thread_id = GetCurrentThreadId();
    cls.lpfnWndProc = peer_proc;
    cls.hInstance = peer_module;
    cls.lpszClassName = "T447_MSG_NATIVE";
    if (!RegisterClassA(&cls)) {
        InterlockedIncrement(&errors);
        SetEvent(peer_ready);
        return 1;
    }
    peer_window = CreateWindowA(cls.lpszClassName, "Native message peer", 0,
        0, 0, 1, 1, NULL, NULL, peer_module, NULL);
    check(peer_window != NULL);
    SetEvent(peer_ready);
    if (!peer_window) {
        UnregisterClassA(cls.lpszClassName, peer_module);
        return 1;
    }
    while ((got = GetMessageA(&message, NULL, 0, 0)) > 0)
        DispatchMessageA(&message);
    check(got == 0);
    check(!IsWindow(peer_window));
    check(UnregisterClassA(cls.lpszClassName, peer_module));
    return errors ? 1 : 0;
}
#endif

#ifdef MESSAGE_EXTERNAL_PROGRAM
/* The same actual endpoint, but now in an independent32/64-bit process.
 * Only scalar HWND/PID and a ready-event name cross the test launch edge;
 * native USER alone captures and marshals the standard pointer messages. */
int main(int argc, char **argv)
{
    DWORD owner = 0, code, report[15] = {0}, written;
    HANDLE ready, output;
    if (argc != 4) return 2;
    sender_window = (HWND)(ULONG_PTR)strtoul(argv[2], NULL, 16);
    sender_thread = GetWindowThreadProcessId(sender_window, &owner);
    if (!sender_thread || owner != strtoul(argv[3], NULL, 10) ||
            owner == GetCurrentProcessId()) return 3;
    receiver_window = FindWindowA("T447_MSG_RECV", NULL);
    receiver_thread = GetWindowThreadProcessId(receiver_window, &written);
    if (!receiver_thread || written != owner) return 4;
    ready = OpenEventA(EVENT_MODIFY_STATE, FALSE, argv[1]);
    if (!ready) return 5;
    peer_module = GetModuleHandleA(NULL);
    /* peer_main signals the shared event only after creating its real HWND. */
    peer_ready = ready;
    code = peer_main(NULL);
    check(CloseHandle(ready));
    check(received == 4 && nested == 1 && replied == 1 && text == 1 &&
        structure_received == 6);
    report[0]=0x47534d4e; report[1]=(DWORD)errors; report[2]=owner;
    report[3]=sender_thread; report[4]=receiver_thread; report[5]=peer_thread_id;
    report[6]=(DWORD)received; report[7]=(DWORD)nested; report[8]=(DWORD)replied;
    report[9]=(DWORD)text; report[10]=code; report[11]=IsWindow(peer_window);
    report[12]=(DWORD)structure_received; report[13]=GetCurrentProcessId();
    report[14]=sizeof(void *)*8;
    output=CreateFileA("MSGHOST.BIN",GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
    if(output==INVALID_HANDLE_VALUE) return 6;
    check(WriteFile(output,report,sizeof(report),&written,NULL) && written==sizeof(report));
    check(CloseHandle(output));
    return errors ? 1 : 0;
}
#else

DWORD WINAPI MessagePeer(DWORD operation)
{
    DWORD owner = 0, code = 1;
#ifdef CLASSIC_DDE
#ifdef CLASSIC_DDE_TASK_LOSS
    DWORD report[18] = {0};
#else
    DWORD report[17] = {0};
#endif
#else
    DWORD report[
#ifdef MESSAGE_CROSS_WORKER
        14
#else
#ifdef MESSAGE_EXTERNAL_HOST
        15
#else
#ifdef M4_COPYDATA_PROC32
        15
#else
#ifdef M4_STRUCTURES
        13
#else
        12
#endif
#endif
#endif
#endif
    ] = {0};
#endif
    DWORD written;
    HANDLE output;
#ifdef MESSAGE_TRANSACTION
    if(operation==12){
        char prefix[MAX_PATH], arm[MAX_PATH], buffer[64]={0};
        DWORD identity[2]={GetCurrentThreadId(),(DWORD)(ULONG_PTR)sender_window};
        WNDPROC procedure=(WNDPROC)GetWindowLongPtrA(sender_window,GWLP_WNDPROC);
        BOOL trapped=FALSE;
        LONG result;
        check(GetCurrentThreadId()==sender_thread && procedure!=NULL);
        if(!GetEnvironmentVariableA("MVDM_TEST_MESSAGE_TRANSACTION",prefix,sizeof(prefix)) ||
                strlen(prefix)+8>=sizeof(prefix))return 0;
        strcpy_s(arm,sizeof(arm),prefix);strcat_s(arm,sizeof(arm),".arm");
        output=CreateFileA(arm,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
        if(output==INVALID_HANDLE_VALUE)return 0;
        check(WriteFile(output,identity,sizeof(identity),&written,NULL) && written==sizeof(identity));
        check(CloseHandle(output));
        /* Use the real native entry on its owning WOW task thread. No guessed
         * provider address, private host layout, task bind or USER replacement. */
        __try{(void)CallWindowProcA(procedure,sender_window,WM_GETTEXT,sizeof(buffer),(LPARAM)buffer);}
        __except(GetExceptionCode()==0xe4470006 ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH){trapped=TRUE;}
        check(trapped);
        memset(buffer,0,sizeof(buffer));
        result=(LONG)CallWindowProcA(procedure,sender_window,WM_GETTEXT,sizeof(buffer),(LPARAM)buffer);
        check(result>0 && result<(LONG)sizeof(buffer) && result==(LONG)strlen(buffer));
        return errors ? 0 : 0x447a008c;
    }
#endif
#ifdef MESSAGE_CONSTRUCTION
    if(operation==11){
        DWORD packet[4]={16,0x11223344,0x55667788,0x12345678};
        check(GetCurrentThreadId()==sender_thread);
        check(SendMessageA(sender_window,0x0038,0,(LPARAM)packet)==0x447a007f);
        return errors ? 0 : 0x447a008b;
    }
#endif
#ifdef MESSAGE_PENDING_SEND
    if(operation==9 || operation==10){
        DWORD_PTR value=0;
        check(GetCurrentThreadId()==sender_thread);
        check(SendMessageTimeoutA(peer_window,operation==9 ? 0x483 : 0x485,0,0,
            SMTO_ERRORONEXIT,10000,&value)!=0);
        check(value==(operation==9 ? 0x447a0083u : 0x447a0085u));
        check(blocked_returned==1 && blocked_nested==1);
        if(operation==10){
            DWORD wait_report[]={0x54494157,(DWORD)errors,GetCurrentProcessId(),
                sender_thread,receiver_thread,peer_thread_id,blocked_status,
                blocked_error,(DWORD)blocked_nested,death_status,death_error,
                (DWORD)IsWindow(death_window),death_owner,stale_result,death_result};
            output=CreateFileA("MSGWAIT.BIN",GENERIC_WRITE,0,NULL,CREATE_NEW,
                FILE_ATTRIBUTE_NORMAL,NULL);
            if(output==INVALID_HANDLE_VALUE)return 0;
            check(WriteFile(output,wait_report,sizeof(wait_report),&written,NULL) &&
                written==sizeof(wait_report));
            check(CloseHandle(output));
        }
        return errors ? 0 : operation==9 ? 0x447a0089 : 0x447a008a;
    }
#endif
    if (operation == 0) {
        if (peer_thread || peer_ready) return 0;
        sender_window = FindWindowA("T447_MSG_SEND", NULL);
        if (!sender_window) return 0;
        sender_thread = GetWindowThreadProcessId(sender_window, &owner);
        if (owner != GetCurrentProcessId() || sender_thread != GetCurrentThreadId())
            return 0;
        if (!peer_module) return 0;
#ifdef MESSAGE_EXTERNAL_HOST
        {
            char executable[MAX_PATH], directory[MAX_PATH], command[1024], name[96], *slash;
            STARTUPINFOA startup = {sizeof(startup)};
            PROCESS_INFORMATION process = {0};
            DWORD length = GetModuleFileNameA(peer_module, executable, MAX_PATH);
            if (!length || length >= MAX_PATH || !(slash=strrchr(executable,'\\'))) return 0;
            if (strcpy_s(slash+1, sizeof(executable)-(size_t)(slash+1-executable), "MSGPEER.EXE")) return 0;
            if (strcpy_s(directory,sizeof(directory),executable)) return 0;
            *strrchr(directory,'\\')=0;
            if (_snprintf_s(name,sizeof(name),_TRUNCATE,"Local\\T447-MSG-%lu-%lu-%lu",
                    GetCurrentProcessId(),GetCurrentThreadId(),GetTickCount())<0) return 0;
            peer_ready=CreateEventA(NULL,TRUE,FALSE,name);
            if (!peer_ready || GetLastError()==ERROR_ALREADY_EXISTS) return 0;
            if (_snprintf_s(command,sizeof(command),_TRUNCATE,"\"%s\" %s %lx %lu",
                    executable,name,(unsigned long)(ULONG_PTR)sender_window,owner)<0) return 0;
            if (!CreateProcessA(executable,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,
                    NULL,directory,&startup,&process)) return 0;
            peer_thread=process.hProcess; peer_process_id=process.dwProcessId;
            check(CloseHandle(process.hThread));
            if (WaitForSingleObject(peer_ready,5000)!=WAIT_OBJECT_0) return 0;
            peer_window=FindWindowA("T447_MSG_NATIVE",NULL);
            peer_thread_id=GetWindowThreadProcessId(peer_window,&owner);
            check(owner==peer_process_id && owner!=GetCurrentProcessId());
        }
#else
        peer_ready = CreateEventA(NULL, TRUE, FALSE, NULL);
        if (!peer_ready) return 0;
        peer_thread = CreateThread(NULL, 0, peer_main, NULL, 0, NULL);
        if (!peer_thread || WaitForSingleObject(peer_ready, 5000) != WAIT_OBJECT_0)
            return 0;
#endif
        check(peer_thread_id != sender_thread && peer_window != NULL);
        return errors ? 0 : 0x447a0001;
    }
    if (operation == 1) {
        receiver_window = FindWindowA("T447_MSG_RECV", NULL);
        if (!peer_thread || !receiver_window) return 0;
        receiver_thread = GetWindowThreadProcessId(receiver_window, &owner);
#ifdef MESSAGE_CROSS_WORKER
        receiver_process_id=owner;
        check(owner && owner != GetCurrentProcessId());
#else
        check(owner == GetCurrentProcessId());
#endif
        check(receiver_thread != sender_thread && receiver_thread != peer_thread_id);
        return errors ? 0 : 0x447a0002;
    }
#ifdef M4_STRUCTURES
    if (operation == 3) {
        DWORD_PTR value=0;
        check(peer_thread && GetCurrentThreadId() == sender_thread);
        check(SendMessageTimeoutA(peer_window,0x480,0,0,
            SMTO_ABORTIFHUNG|SMTO_ERRORONEXIT,15000,&value) != 0);
        check(value == 0x447a0020);
#ifdef MESSAGE_EXTERNAL_HOST
        check(SendMessageA(peer_window,0x482,0,0)==6);
#else
        check(structure_received == 6);
#endif
        return errors ? 0 : 0x447a0020;
    }
#endif
#ifdef M4_REPLY_PAYLOAD
    if (operation == 5
#ifdef M4_TIMEOUT_PAYLOAD
        || operation == 8
#endif
    ) {
        DWORD_PTR value=0;
        LRESULT sent;
        if (GetCurrentThreadId() != sender_thread || reply_payload.page ||
            !reply_payload_create(&reply_payload)) return 0;
#ifdef M4_TIMEOUT_PAYLOAD
        if (operation == 8) {
            reply_payload.timeout=TRUE;
            ((COPYDATASTRUCT *)reply_payload.page)->dwData=TIMEOUT_PAYLOAD_DATA;
        }
#endif
        InterlockedExchange(&reply_driver_done,0);
        sent=SendMessageTimeoutA(peer_window,0x481,0,0,
            SMTO_ABORTIFHUNG|SMTO_ERRORONEXIT,15000,&value);
        /* An incomplete/late peer may still own input. Do not free it on a
         * failed path; fail this test process instead of manufacturing cleanup. */
        if (!sent || value != 0x447a0055 || !reply_driver_done || !reply_payload.continued)
            return 0;
        check(reply_payload_finish(&reply_payload,reply_payload.timeout ? "MSGTIME.BIN" : "MSGREPLY.BIN"));
        check(!reply_payload.page && !reply_payload.acknowledgement);
        return errors ? 0 : operation == 8 ? 0x447a0058 : 0x447a0055;
    }
    if (operation == 7) {
        if (GetCurrentThreadId() != sender_thread || !reply_payload.page ||
            !(InSendMessageEx(NULL) & ISMEX_SEND) ||
            InterlockedCompareExchange(&reply_payload.accepted,1,0) != 0) return 0;
        return 0x447a0057;
    }
    if (operation == 6) {
        if (GetCurrentThreadId() != sender_thread || !reply_payload.page ||
            !reply_payload.accepted) return 0;
        return reply_payload_wait(&reply_payload) ? 0x447a0056 : 0;
    }
#endif
#ifdef CLASSIC_DDE
    if (operation == 20) {
        FILE *out;
        if(GetCurrentThreadId()!=sender_thread || dde_free_slot || dde_post_slot) return 0;
        dde_observed_packet=NULL; dde_failed_lparam=0; dde_failed_window=NULL;
        dde_failed_posts=0; dde_freed_lparams=0; dde_free_calls=0;
        dde_free_result=(HGLOBAL)(ULONG_PTR)1;
        if(!dde_switch_free_import(TRUE)) return 0;
        if(!dde_switch_failed_post(TRUE)) {
            out=fopen("MSGPEER-FAIL.txt","a");
            if(out) { fprintf(out,"failed-post import setup failed post=%p lparam=%p error=%lu\n",
                (void *)dde_find_import("PostMessageA"),(void *)dde_find_import("FreeDDElParam"),GetLastError()); fclose(out); }
            dde_switch_free_import(FALSE); return 0;
        }
        return 0x447a0094;
    }
    if (operation == 21) {
        BOOL restored_post,restored_free;
        FILE *out;
        if(GetCurrentThreadId()!=sender_thread || !dde_free_slot || !dde_post_slot) return 0;
        restored_post=dde_switch_failed_post(FALSE);
        restored_free=dde_switch_free_import(FALSE);
        out=fopen("MSGPEER-FAIL.txt","a");
        if(out) { fprintf(out,"failed-post restore=%d/%d posts=%ld window=%p peer=%p packet=%p lparams=%ld frees=%ld result=%p\n",
            restored_post,restored_free,dde_failed_posts,dde_failed_window,peer_window,dde_observed_packet,dde_freed_lparams,dde_free_calls,dde_free_result); fclose(out); }
        check(restored_post && restored_free && dde_failed_posts==1 &&
              LOWORD((ULONG_PTR)dde_failed_window)==LOWORD((ULONG_PTR)peer_window) &&
              dde_observed_packet && dde_freed_lparams==1 && dde_free_calls==1 &&
              dde_free_result==NULL);
        dde_observed_packet=NULL;
        return errors ? 0 : 0x447a0095;
    }
    if (operation == 22) {
        if(GetCurrentThreadId()!=sender_thread || dde_initiate_event || !peer_window) return 0;
        dde_initiate_service=GlobalAddAtomA("T465-DDE-SERVICE");
        dde_initiate_topic=GlobalAddAtomA("T465-DDE-TOPIC");
        if(!dde_initiate_service || !dde_initiate_topic) goto initiate_fail;
        dde_initiate_event=CreateEventA(NULL,TRUE,FALSE,NULL);
        if(!dde_initiate_event) goto initiate_fail;
        InterlockedExchange(&dde_initiate_ack,0);
        if(!PostMessageA(sender_window,WM_DDE_INITIATE,(WPARAM)peer_window,
                         MAKELPARAM(dde_initiate_service,dde_initiate_topic))) goto initiate_fail;
        return 0x447a0096;
initiate_fail:
        if(dde_initiate_event) { CloseHandle(dde_initiate_event); dde_initiate_event=NULL; }
        if(dde_initiate_service) { GlobalDeleteAtom(dde_initiate_service); dde_initiate_service=0; }
        if(dde_initiate_topic) { GlobalDeleteAtom(dde_initiate_topic); dde_initiate_topic=0; }
        return 0;
    }
    if (operation == 23) {
        DWORD wait;
        if(GetCurrentThreadId()!=sender_thread || !dde_initiate_event) return 0;
        wait=WaitForSingleObject(dde_initiate_event,5000);
        check(wait==WAIT_OBJECT_0 && dde_initiate_ack==1);
        check(CloseHandle(dde_initiate_event)); dde_initiate_event=NULL;
        check(GlobalDeleteAtom(dde_initiate_service)==0);
        check(GlobalDeleteAtom(dde_initiate_topic)==0);
        dde_initiate_service=0; dde_initiate_topic=0;
        return errors ? 0 : 0x447a0097;
    }
    if (operation == 18) {
        if(GetCurrentThreadId()!=sender_thread || !dde_observed_packet || dde_free_slot) return 0;
        dde_free_calls=0;dde_free_result=(HGLOBAL)(ULONG_PTR)1;
#ifdef CLASSIC_DDE_TASK_LOSS
        dde_task_loss_free=0;
#endif
        return dde_switch_free_import(TRUE) ? 0x447a0092 : 0;
    }
    if (operation == 19) {
        BOOL restored;
        if(GetCurrentThreadId()!=sender_thread || !dde_free_slot) return 0;
        restored=dde_switch_free_import(FALSE);
        check(restored && dde_free_calls==1 && dde_free_result==NULL);
        dde_observed_packet=NULL;
        return errors ? 0 : 0x447a0093;
    }
    if (operation == 17) {
        HGLOBAL payload;
        char *bytes;
        LRESULT result, generic;
        DWORD error;
        if(GetCurrentThreadId()!=sender_thread || !peer_window || !dde_reverse_event) return 0;
        payload=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,32);
        if(!payload) return 0;
        bytes=(char *)GlobalLock(payload);
        if(!bytes) { GlobalFree(payload);return 0; }
        memcpy(bytes,"T449-DDE-REVERSE",17);
        GlobalUnlock(payload);
        generic=SendMessageA(peer_window,0x487,0,0);
        check(generic==0x447a0087);
        InterlockedExchange(&dde_native_control,1);
        SetLastError(0);
        result=SendMessageA(peer_window,WM_DDE_EXECUTE,(WPARAM)sender_window,(LPARAM)payload);
        error=GetLastError();
        {
            FILE *out=fopen("MSGPEER-FAIL.txt","a");
            if(out) {
                fprintf(out,"native-generic=%ld native-send-result=%ld error=%lu received=%ld\n",
                        (long)generic,(long)result,error,dde_reverse_received);
                fclose(out);
            }
        }
        InterlockedExchange(&dde_native_control,0);
        GlobalFree(payload);
        return errors ? 0 : 0x447a0091;
    }
    if (operation == 15) {
        if(GetCurrentThreadId()!=sender_thread || dde_reverse_event || !peer_window) return 0;
        InterlockedExchange(&dde_reverse_received,0);
        dde_reverse_event=CreateEventA(NULL,TRUE,FALSE,NULL);
        return dde_reverse_event ? 0x447a008f : 0;
    }
    if (operation == 16) {
        DWORD wait;
        if(GetCurrentThreadId()!=sender_thread || !dde_reverse_event) return 0;
        wait=WaitForSingleObject(dde_reverse_event,5000);
        {
            FILE *out=fopen("MSGPEER-FAIL.txt","a");
            if(out) {
                fprintf(out,"reverse-wait=%lu received=%ld errors=%ld last-error=%lu\n",
                        wait,dde_reverse_received,errors,GetLastError());
                fclose(out);
            }
        }
        check(wait==WAIT_OBJECT_0 && dde_reverse_received==1);
        check(CloseHandle(dde_reverse_event)); dde_reverse_event=NULL;
        return errors ? 0 : 0x447a0090;
    }
    if (operation == 13) {
        char *payload;
        if(GetCurrentThreadId()!=sender_thread || dde_payload || !peer_window) return 0;
        dde_ack_event=CreateEventA(NULL,TRUE,FALSE,NULL);
        if(!dde_ack_event) return 0;
        dde_payload=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,16);
        if(!dde_payload) {
            CloseHandle(dde_ack_event);
            dde_ack_event=NULL;
            return 0;
        }
        payload=(char *)GlobalLock(dde_payload);
        if(!payload) {
            GlobalFree(dde_payload);
            dde_payload=NULL;
            CloseHandle(dde_ack_event);
            dde_ack_event=NULL;
            return 0;
        }
        memcpy(payload,"T447-DDE-EXEC",14);payload[14]=0;
        check(GlobalUnlock(dde_payload)==FALSE);
        /* The original USER message contract declares EXECUTE posted.  It
         * must not be synchronously delivered merely to simplify the probe. */
        check(PostMessageA(sender_window,WM_DDE_EXECUTE,(WPARAM)peer_window,
            (LPARAM)dde_payload));
        return errors ? 0 : 0x447a008d;
    }
    if (operation == 14) {
        DWORD_PTR value=0;
        if(GetCurrentThreadId()!=sender_thread || !dde_payload || !dde_ack_event) return 0;
        check(WaitForSingleObject(dde_ack_event,5000)==WAIT_OBJECT_0 && dde_ack==1);
        check(GlobalFree(dde_payload)==NULL);dde_payload=NULL;InterlockedIncrement(&dde_released);
        check(SendMessageTimeoutA(sender_window,0x486,0,0,
            SMTO_ABORTIFHUNG|SMTO_ERRORONEXIT,5000,&value)!=0 && value==0x447a0086);
        InterlockedIncrement(&dde_retired);
        check(CloseHandle(dde_ack_event));dde_ack_event=NULL;
        return errors ? 0 : 0x447a008e;
    }
#endif
    if (operation != 2 || !peer_thread) return 0;
    check(PostMessageA(peer_window, WM_CLOSE, 0, 0));
    if (WaitForSingleObject(peer_thread, 5000) != WAIT_OBJECT_0) return 0;
#ifdef MESSAGE_EXTERNAL_HOST
    check(GetExitCodeProcess(peer_thread,&code) && code==0);
    output=CreateFileA("MSGHOST.BIN",GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if(output==INVALID_HANDLE_VALUE) return 0;
    check(ReadFile(output,report,sizeof(report),&written,NULL) && written==sizeof(report));
    check(CloseHandle(output));
    check(report[0]==0x47534d4e && report[1]==0 && report[2]==GetCurrentProcessId() &&
        report[3]==sender_thread && report[4]==receiver_thread && report[5]==peer_thread_id &&
        report[13]==peer_process_id && (report[14]==32 || report[14]==64));
#else
    check(GetExitCodeThread(peer_thread, &code) && code == 0);
    check(received == 4 && nested == 1 && replied == 1 && text == 1);
    report[0] = 0x47534d4e; /* NMSG */
    report[1] = (DWORD)errors;
    report[2] = GetCurrentProcessId();
    report[3] = sender_thread;
    report[4] = receiver_thread;
    report[5] = peer_thread_id;
    report[6] = (DWORD)received;
    report[7] = (DWORD)nested;
    report[8] = (DWORD)replied;
    report[9] = (DWORD)text;
    report[10] = code;
    report[11] = IsWindow(peer_window);
#ifdef M4_STRUCTURES
    check(structure_received == 6);
    report[12]=(DWORD)structure_received;
    report[1]=(DWORD)errors;
#endif
#ifdef CLASSIC_DDE
    report[13]=(DWORD)dde_ack;
    report[14]=(DWORD)dde_released;
    report[15]=(DWORD)dde_retired;
    report[16]=(DWORD)dde_initiate_ack;
#ifdef CLASSIC_DDE_TASK_LOSS
    report[17]=(DWORD)dde_task_loss_free;
#endif
    report[1]=(DWORD)errors;
#endif
#ifdef M4_COPYDATA_PROC32
    check(proc32_fresh==1 && proc32_borrowed==1);
    report[13]=(DWORD)proc32_fresh;report[14]=(DWORD)proc32_borrowed;
    report[1]=(DWORD)errors;
#endif
#ifdef MESSAGE_CROSS_WORKER
    report[13]=receiver_process_id;
#endif
    output = CreateFileA("MSGHOST.BIN", GENERIC_WRITE, 0, NULL, CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (output == INVALID_HANDLE_VALUE) return 0;
    check(WriteFile(output, report, sizeof(report), &written, NULL) && written == sizeof(report));
    check(CloseHandle(output));
#endif
    check(CloseHandle(peer_thread)); peer_thread = NULL;
    check(CloseHandle(peer_ready)); peer_ready = NULL;
    return errors ? 0 : 0x447a0003;
}

/* Actual generic-thunk caller on the sender's bound task thread. Function
 * addresses come from the production DLL's public exports, never a fixture
 * scope owner or fabricated PTD. No User32 calls from DllMain are added. */
DWORD WINAPI MessageExtension(DWORD operation, DWORD procedure, DWORD receiver_task)
{
    typedef BOOL (WINAPI *callback_ex)(DWORD,DWORD,DWORD,PVOID,PDWORD);
    typedef DWORD (WINAPI *callback_one)(DWORD,DWORD);
    typedef VOID (WINAPI *yield_proc)(VOID);
    typedef VOID (WINAPI *directed_proc)(WORD);
    HMODULE wow = GetModuleHandleA("WOW32.DLL");
    callback_ex extended;
    callback_one simple;
    yield_proc yield;
    directed_proc directed_yield;
    FARPROC raw;
    DWORD arguments[8] = {0x12345678}; /* Original PARM16 capture extent. */
    DWORD value = 0x447affff, first, second, report[10], written;
    HANDLE output;
    BOOL invalid;
    if (!wow || operation != 0x79 || !procedure || !receiver_task ||
            receiver_task > 0xffff || GetCurrentThreadId() != sender_thread) return 0;
    raw = GetProcAddress(wow,"WOWCallback16Ex"); if (!raw) return 0;
    memcpy(&extended,&raw,sizeof(extended));
    raw = GetProcAddress(wow,"WOWCallback16"); if (!raw) return 0;
    memcpy(&simple,&raw,sizeof(simple));
    raw = GetProcAddress(wow,"WOWYield16"); if (!raw) return 0;
    memcpy(&yield,&raw,sizeof(yield));
    raw = GetProcAddress(wow,"WOWDirectedYield16"); if (!raw) return 0;
    memcpy(&directed_yield,&raw,sizeof(directed_yield));
    SetLastError(ERROR_SUCCESS);
    invalid = !extended(procedure,0,MAXDWORD,NULL,&value) &&
        GetLastError() == ERROR_INVALID_PARAMETER && value == 0x447affff;
    check(invalid);
    check(extended(procedure,0,sizeof(DWORD),arguments,&value));
    first = value;
    check(first == 0x447a0079);
    second = simple(procedure,0x12345678);
    check(second == 0x447a0079);
    yield();
    check(PostMessageA(receiver_window,0x475,0,0));
    directed_yield((WORD)receiver_task);
    report[0] = 0x54584547; /* GEXT */
    report[1] = (DWORD)errors;
    report[2] = GetCurrentProcessId();report[3] = GetCurrentThreadId();
    report[4] = receiver_task;report[5] = first;report[6] = second;
    report[7] = 1;report[8] = 1;report[9] = invalid;
    output = CreateFileA("MSGEXT.BIN",GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
    if(output == INVALID_HANDLE_VALUE) return 0;
    check(WriteFile(output,report,sizeof(report),&written,NULL) && written == sizeof(report));
    check(CloseHandle(output));
    return errors ? 0 : 0x447a0079;
}
#endif
