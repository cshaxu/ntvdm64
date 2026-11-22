#ifndef WOW_MENU_PARSER_BINDINGS_H
#define WOW_MENU_PARSER_BINDINGS_H
#include <windows.h>
#include <winternl.h>

/* Original menu parsing uses the bound task CLIENTINFO, not the native TEB. */
HMENU WINAPI CreateMenuFromResource(LPBYTE);
typedef HMENU (WINAPI *wow_menu_parser_provider)(LPBYTE);
BOOL WINAPI wow_menu_parser_bind(wow_menu_parser_provider);
HMENU WINAPI wow_user_load_menu_indirect(const MENUTEMPLATEW *);

#ifdef WOW_ORIGINAL_MENU_CLIENT
#include "wow_task_init_bindings.h"
#include "wow_edit_call_bindings.h"
#define GetClientInfo() wow_edit_client_info()
#define TIF_16BIT WOW_TASK_TIF_16BIT
/* The retained parser's allocation, insertion and failure destruction must
 * use the same task-associated native boundary as the WOW menu thunks. */
#include "wow_menu_native_bindings.h"
/* Exact private declarations from OpenNT public/internal/windows/inc/winuserp.h
 * and ntuser/client/usercli.h. Native menu objects remain USER32-owned. */
#define MF_CHANGE_VALID (MF_INSERT | MF_CHANGE | MF_APPEND | MF_DELETE | \
    MF_REMOVE | MF_BYCOMMAND | MF_BYPOSITION | MF_SEPARATOR | MF_ENABLED | \
    MF_GRAYED | MF_DISABLED | MF_UNCHECKED | MF_CHECKED | MF_USECHECKBITMAPS | \
    MF_STRING | MF_BITMAP | MF_OWNERDRAW | MF_POPUP | MF_MENUBARBREAK | \
    MF_MENUBREAK | MF_UNHILITE | MF_HILITE | MF_SYSMENU)
#define MF_VALID (MF_CHANGE_VALID | MF_HELP | MF_MOUSESELECT)
#define MFT_MASK 0x00034B64L
#define MFS_MASK 0x0000108BL
#define MFR_POPUP 0x01
#define MFR_END 0x80
#define MFT_OLDAPI_MASK 0x00004B64L
#define MFS_OLDAPI_MASK 0x0000008BL
typedef struct {
    DWORD dwHelpID;
    DWORD fType;
    DWORD fState;
    DWORD menuId;
    WORD wResInfo;
    WCHAR mtString[1];
} MENUITEMTEMPLATE2, *PMENUITEMTEMPLATE2;
#define NextWordBoundary(p) ((PBYTE)(p) + ((DWORD)(p) & 1))
#define MFMWFP_NOITEM ((UINT)-1)
#define MENUITEMINFO MENUITEMINFOW
#define NtUserCreateMenu CreateMenu
#define NtUserDestroyMenu DestroyMenu
#define NtUserSetMenuContextHelpId SetMenuContextHelpId
/* Original callers always insert by position; no private MENU is emulated. */
#define NtUserThunkedMenuItemInfo(menu, item, position, insert, info, str, ansi) \
    InsertMenuItemW(menu, item, position, info)
#define RIPERR1(error, level, format, value) SetLastError(error)
#define RIPMSG0(level, message) ((void)0)
#endif
#endif
