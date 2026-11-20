#ifndef WOW_DEFAULT_MESSAGE_BINDINGS_H
#define WOW_DEFAULT_MESSAGE_BINDINGS_H
#include <windows.h>
#include "compat/include/opennt_user_multipass.h"
#include "compat/include/wow_control_message_abi.h"

/* OPENNT-HOST-090: selected original winuserp.h constants absent from the
 * current public SDK. These are message values, not native syscall IDs. */
#ifndef WM_CTLCOLOR
#define WM_CTLCOLOR                     0x0019
#endif
#ifndef WM_ISACTIVEICON
#define WM_ISACTIVEICON                 0x0035
#endif
#ifndef WM_CLIENTSHUTDOWN
#define WM_CLIENTSHUTDOWN               0x003B
#endif
#ifndef WM_KEYF1
#define WM_KEYF1                        0x004D
#endif
#ifndef WM_KLUDGEMINRECT
#define WM_KLUDGEMINRECT                0x008B
#endif
#ifndef WM_DROPOBJECT
#define WM_DROPOBJECT                   0x022A
#endif
#ifndef WM_QUERYDROPOBJECT
#define WM_QUERYDROPOBJECT              0x022B
#endif
#ifndef WM_IME_SYSTEM
#define WM_IME_SYSTEM                   0x0287
#endif
#ifndef WM_SYSMENU
#define WM_SYSMENU                      0x0313
#endif
#ifndef WM_FINALDESTROY
#define WM_FINALDESTROY                 0x0070
#endif
#ifndef WM_SYSTIMER
#define WM_SYSTIMER                     0x0118
#endif

/* A process-owned facade, never a layout overlay of native SHAREDINFO. */
typedef struct wow_default_message_state_type {
    WNDMSG DefWindowMsgs;
    WNDMSG DefWindowSpecMsgs;
    /* Original control message tables retain their FNID indexing. */
    WNDMSG awmControl[FNID_STATIC - FNID_START + 1];
    UINT uiShellMsg;
} wow_default_message_state_type;
extern wow_default_message_state_type wow_default_message_state;
extern const WORD gawDefWindowMsgs[], gawDefWindowSpecMsgs[];
extern const WORD gawEditWndProc[];
extern const WORD gawButtonWndProc[];
extern const WORD gawListboxWndProc[], gawComboWndProc[];
VOID WINAPI InitMessageTables(VOID);
VOID WINAPI InitWindowMsgTable(PBYTE *, PUINT, const WORD *);
WORD WINAPI WowGetDefWindowProcBits(PBYTE, WORD);
BOOL WINAPI wow_default_messages_ready(void);

#ifdef WOW_ORIGINAL_MESSAGE_TABLES
/* Original winuserp.h menu message values, not native syscall numbers. */
#define MN_FIRST                        0x01E0
#define MN_SETHMENU                     (MN_FIRST + 0)
#define MN_SIZEWINDOW                   (MN_FIRST + 2)
#define MN_OPENHIERARCHY                (MN_FIRST + 3)
#define MN_CLOSEHIERARCHY               (MN_FIRST + 4)
#define MN_SELECTITEM                   (MN_FIRST + 5)
#define MN_CANCELMENUS                  (MN_FIRST + 6)
#define MN_SELECTFIRSTVALIDITEM         (MN_FIRST + 7)
#define MN_FINDMENUWINDOWFROMPOINT      (MN_FIRST + 11)
#define MN_SHOWPOPUPWINDOW              (MN_FIRST + 12)
#define MN_BUTTONDOWN                   (MN_FIRST + 13)
#define MN_MOUSEMOVE                    (MN_FIRST + 14)
#define MN_BUTTONUP                     (MN_FIRST + 15)
#define MN_SETTIMERTOOPENHIERARCHY      (MN_FIRST + 16)
#define MN_DBLCLK                       (MN_FIRST + 17)
#define gSharedInfo wow_default_message_state
#define SharedAlloc(bytes) HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, (bytes))
#define TRACE_INIT(args) ((void)0)
#endif
#endif
