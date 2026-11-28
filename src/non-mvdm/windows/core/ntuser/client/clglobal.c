/****************************** Module Header ******************************\
* Module Name: clglobal.c
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
*
* This module contains all of USER.DLL's global variables. These are all
* instance-specific, i.e. each client has his own copy of these. In general,
* there shouldn't be much reason to create instance globals.
*
* NOTE: In this case what we mean by global is that this data is shared by
* all threads of a given process, but not shared between processes
* or between the client and the server. None of this data is useful
* (or even accessable) to the server.
*
* History:
* 10-18-90 DarrinM Created.
\***************************************************************************/

#include <windows.h>
#include "compat/include/wow_shared_user_abi.h"
#include "compat/include/wow_user_client_view_layout.h"
#include "compat/include/wow_user_globals_binding.h"
#line 20
#pragma hdrstop


// Debug globals
#if DBG
#error
#endif

/*
 * We get this warning if we don't explicitly initalize gZero:
 *
 * C4132: 'gZero' : const object should be initialized
 *
 * But we can't explicitly initialize it since it is a union. So
 * we turn the warning off.
 */
#pragma warning(disable:4132)
CONST ALWAYSZERO gZero;
#pragma warning(default:4132)

/*
 * Amount wheel has been scrolled in a control less than WHEEL_DELTA. Each
 * control resets this variable to 0 in WM_KILLFOCUS, and verifies it is
 * 0 in WM_SETFOCUS.
 * CONSIDER: Should be per-queue rather than per client?
 */
int gcWheelDelta;

WCHAR awchSlashStar[] = L"\\*";
CHAR achSlashStar[] = "\\*";

#line 52
SHAREDINFO gSharedInfo;
HMODULE hmodUser;               // USER.DLL's hmodule
ULONG_PTR gHighestUserAddress;

BOOL gfServerProcess;           // USER is linked on the CSR server side
BOOL gfSystemInitialized;       // System has been initialized

ACCESS_MASK gamWinSta;          // ACCESS_MASK for the current WindowStation

PVOID pUserHeap;

WCHAR szUSER32[] = TEXT("USER32");
CONST WCHAR szNull[2] = { TEXT('\0'), TEXT('\015') };
CONST WCHAR szOneChar[] = TEXT("0");
WCHAR szSLASHSTARDOTSTAR[] = TEXT("\\*");  /* This is a single "\"  */

LPWSTR pwcHighContrastScheme;
LPSTR  pcHighContrastScheme;

/* Maps MessageBox type to number of buttons in the MessageBox */
CONST BYTE mpTypeCcmd[] = { 1, 2, 3, 3, 2, 2, 3 };

/* Maps MessageBox type to index into SEBbuttons array */
CONST BYTE mpTypeIich[] = { 0, 2, 5, 12, 9, 16, 19 };

CONST UINT SEBbuttons[] = {
    SEB_OK, SEB_HELP,
    SEB_OK, SEB_CANCEL, SEB_HELP,
    SEB_ABORT, SEB_RETRY, SEB_IGNORE, SEB_HELP,
    SEB_YES, SEB_NO, SEB_HELP,
    SEB_YES, SEB_NO, SEB_CANCEL, SEB_HELP,
    SEB_RETRY, SEB_CANCEL, SEB_HELP,
    SEB_CANCEL, SEB_TRYAGAIN, SEB_CONTINUE, SEB_HELP,
};

ATOM atomMDIActivateProp;

CRITICAL_SECTION gcsLookaside;
CRITICAL_SECTION gcsHdc;
CRITICAL_SECTION gcsClipboard;
CRITICAL_SECTION gcsAccelCache;

#ifdef _JANUS_
#error
#endif
#line 100

HDC    ghdcBits2;
HDC    ghdcGray;
HFONT  ghFontSys;
HBRUSH ghbrWindowText;
int    gcxGray;
int    gcyGray;

FPLPKTABBEDTEXTOUT fpLpkTabbedTextOut = UserLpkTabbedTextOut;
FPLPKPSMTEXTOUT fpLpkPSMTextOut       = UserLpkPSMTextOut;
FPLPKDRAWTEXTEX fpLpkDrawTextEx       = (FPLPKDRAWTEXTEX)NULL;
PLPKEDITCALLOUT fpLpkEditControl      = (PLPKEDITCALLOUT)NULL;
#line 112

/*
 * These are the resource call procedure addresses. If WOW is running,
 * it makes a call to set all these up to point to it. If it isn't
 * running, it defaults to the values you see below.
 */
PFNFINDA pfnFindResourceExA; // Assigned dynamically - _declspec (PFNFINDA)FindResourceExA,
PFNFINDW pfnFindResourceExW; // Assigned dynamically - _declspec (PFNFINDW)FindResourceExW,
PFNLOAD pfnLoadResource; // Assigned dynamically - _declspec (PFNLOAD)LoadResource,
PFNLOCK pfnLockResource             = (PFNLOCK)_LockResource;
PFNUNLOCK pfnUnlockResource         = (PFNUNLOCK)_UnlockResource;
PFNFREE pfnFreeResource             = (PFNFREE)_FreeResource;
PFNSIZEOF pfnSizeofResource; // Assigned dynamically - _declspec (PFNSIZEOF)SizeofResource
#line 131
PFNGETEXPWINVER pfnGetExpWinVer     = RtlGetExpWinVer;
PFN16GALLOC pfn16GlobalAlloc;
PFN16GFREE pfn16GlobalFree;
PFNEMPTYCB pfnWowEmptyClipBoard;
PFNWOWWNDPROCEX  pfnWowWndProcEx;
PFNWOWDLGPROCEX  pfnWowDlgProcEx;
PFNWOWEDITNEXTWORD   pfnWowEditNextWord;
PFNWOWCBSTOREHANDLE pfnWowCBStoreHandle;
PFNGETPROCMODULE16  pfnWowGetProcModule;
PFNWOWTELLWOWTHEHDLG pfnWOWTellWOWThehDlg;
PFNWOWMSGBOXINDIRECTCALLBACK pfnWowMsgBoxIndirectCallback;
PFNWOWILSTRCMP  pfnWowIlstrcmp;
PFNWOWTASK16SCHEDNOTIFY pfnWowTask16SchedNotify;

/*
 * UserApiHook
 *
 * It is important to keep guah initialization in sync with ResetUserApiHook()
 * and the definition of USERAPIHOOK. This initialization must be done here
 * so that the function pointers are valid immediately on startup.
 */
BYTE grgbDwpLiteHookMsg[(WM_USER + 7) / 8];
BYTE grgbWndLiteHookMsg[(WM_USER + 7) / 8];
BYTE grgbDlgLiteHookMsg[(WM_USER + 7) / 8];

HMODULE ghmodUserApiHook;
#line 191
BOOL    gfUserApiHook;
LONG    gcCallUserApiHook;
LONG    gcLoadUserApiHook;
RTL_CRITICAL_SECTION gcsUserApiHook;
INITUSERAPIHOOK      gpfnInitUserApi;

#ifdef MESSAGE_PUMP_HOOK
#error
#endif // MESSAGE_PUMP_HOOK
#line 220


/*
 * Menu Drag and Drop
 */
HINSTANCE ghinstOLE;
FARPROC gpfnOLEOleUninitialize;
FARPROC gpfnOLERegisterDD;
FARPROC gpfnOLERevokeDD;
FARPROC gpfnOLEDoDD;

/*
 * Accelerator table resources list.
 */
PACCELCACHE gpac;

/*
 * IME Window Handling.
 */
DWORD gfConIme = UNKNOWN_CONIME;

/*
 * Used for TS Services Message Box handling
 */
FARPROC     gfnWinStationSendMessageW;
HINSTANCE   ghinstWinStaDll;

/*
 * Are we winlogon?
 */
BOOL gfLogonProcess;

#include "wow_registration_provider.h"
#include "wow_user_registration_state.h"
#include "wow_dialog_creation_binding.h"
#include "wow_private_user_compat.h"
#include "wow_bitmap_bindings.h"
#include "wow_user_private_access.h"

/* Original registration ABI storage; B owns task/lifecycle orchestration. */
static PFNWOWHANDLERSIN wow_input_handlers;
const struct tagAPFNWOWHANDLERSIN *WINAPI wow_original_registered_callbacks(void)
{
    return &wow_input_handlers;
}
void WINAPI wow_original_set_registered_callbacks(struct tagAPFNWOWHANDLERSIN *input)
{
    wow_input_handlers = *input;
    pfnWowIlstrcmp = input->pfnWowIlstrsmp;
}
/* XP inc/ntcb.h:7814 ClientWOWGetProcModule, same-thread native boundary. */
static WORD WINAPI wow_original_procedure_module(DWORD procedure)
{
    ULONG real;
    if (!wow_input_handlers.pfnGetProcModule16 || !IsWOWProc(procedure)) return 0;
    UnMarkWOWProc(procedure, real);
    return wow_input_handlers.pfnGetProcModule16(real);
}
void WINAPI wow_original_registration_lifecycle(wow_user_task_lifecycle_callbacks *callbacks)
{
    callbacks->get_version = wow_input_handlers.pfnGetExpWinVer;
    callbacks->callback = wow_input_handlers.pfnWowWndProcEx;
    callbacks->procedure_module = wow_original_procedure_module;
}
wow_window_callback WINAPI wow_original_registration_window_callback(void)
{
    return wow_input_handlers.pfnWowWndProcEx;
}
BOOL WINAPI wow_original_edit_callbacks_ready(void)
{
    const PFNWOWHANDLERSIN *callbacks = wow_original_registered_callbacks();
    if (!callbacks->pfnLocalAlloc || !callbacks->pfnLocalReAlloc ||
            !callbacks->pfnLocalLock || !callbacks->pfnLocalUnlock ||
            !callbacks->pfnLocalSize || !callbacks->pfnLocalFree ||
            !callbacks->pfnWowEditNextWord || !callbacks->pfnEmptyCB ||
            !callbacks->pfnGetExpWinVer) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    return TRUE;
}
/* Indexed by this USER cohort's FNID values, not the WOW producer's version. */
static int aiClassWow[] = {
    WOWCLASS_SCROLLBAR,
    WOWCLASS_ICONTITLE,
    WOWCLASS_MENU,
    WOWCLASS_DESKTOP,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_SWITCHWND,
    WOWCLASS_BUTTON,
    WOWCLASS_COMBOBOX,
    WOWCLASS_COMBOLBOX,
    WOWCLASS_DIALOG,
    WOWCLASS_EDIT,
    WOWCLASS_LISTBOX,
    WOWCLASS_MDICLIENT,
    WOWCLASS_STATIC,
    WOWCLASS_WIN16,    // 2A9
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,     // 2B1
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16,
    WOWCLASS_WIN16
    };

void WINAPI wow_original_registration_exchange(struct tagAPFNWOWHANDLERSIN *input,
    struct tagAPFNWOWHANDLERSOUT *output)
{
    const wow_registration_provider *provider = wow_registration_provider_current();
    wow_original_set_registered_callbacks(input);
    ZeroMemory(output, sizeof(*output));
    /* Pinned USER.EXE publishes WOW 0400 with WOWDBG layout (init.c).
     * The shared view uses that checked layout, including 16-byte handles. */
    output->dwBldInfo = 0x84000000u;
    output->aiWowClass = aiClassWow;
    output->pfnCsCreateWindowEx = registered_create_window_ex;
    output->pfnDirectedYield = registered_directed_yield;
    output->pfnFreeDDEData = provider->free_dde_data;
    output->pfnGetClassWOWWords = provider->class_words;
    output->pfnInitTask = registered_init_task;
    output->pfnRegisterClassWOWA = registered_register_class;
    output->pfnRegisterUserHungAppHandlers = registered_hung_handlers;
    output->pfnServerCreateDialog = wow_user_server_create_dialog;
    output->pfnServerLoadCreateCursorIcon =
        wow_private_user_load_create_cursor_icon;
    output->pfnServerLoadCreateMenu = wow_private_user_load_create_menu;
    output->pfnWOWCleanup = registered_cleanup;
    output->pfnWOWModuleUnload = registered_module_unload;
    output->pfnWOWFindWindow = wow_native_FindWindowA;

    output->pfnWOWLoadBitmapA = wow_bitmap_load;
    output->pfnWowWaitForMsgAndEvent = registered_wait_for_message;
    output->pfnYieldTask = registered_yield_task;
    output->pfnGetFullUserHandle = registered_full_handle;
    output->pfnGetMenuIndex = wow_private_user_get_menu_index;
    output->pfnWowGetDefWindowProcBits =
        wow_private_user_get_def_window_proc_bits;
    output->pfnFillWindow = registered_fill_window;
}
