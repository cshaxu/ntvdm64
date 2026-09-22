#ifndef WOW_CLEANUP_BINDINGS_H
#define WOW_CLEANUP_BINDINGS_H
#include "wow_class_lookup_bindings.h"
#include "wow_window_dispatch_bindings.h"
#include "wow_input_group_bindings.h"
#include "wow_user_probe_bindings.h"
#include "wow_user_object_bindings.h"
extern const BYTE gabObjectCreateFlags[TYPE_CTYPES];
typedef VOID (APIENTRY *PFNW32ET)(VOID);
#pragma warning(push)
#pragma warning(disable:4201)
#include "opennt-abi/source/public/internal/windows/inc/wowuserp.h"
#pragma warning(pop)

/* ADAPTER-WOW-047: borrowed, serialized original cleanup fields. These are
 * live owner views, not native NT4 layouts or enumeration snapshots. Native
 * identity is retained, not allocated again. Complete membership is required
 * before production publication. Mutation must affect dispatch's actual target.
 * An object release operation owns native failure reporting and table updates.
 * The table always has at least one slot (which may be TYPE_FREE). */
typedef struct wow_cleanup_desktop_info {
    wow_cleanup_window *spwnd;
} wow_cleanup_desktop_info;
typedef struct wow_cleanup_process_object {
    DWORD hTaskWow;
} wow_cleanup_process_object;
typedef struct wow_cleanup_client_procs {
    PROC pfnDefWindowProc, pfnButtonWndProc, pfnStaticWndProc;
    PROC pfnListBoxWndProc, pfnComboBoxWndProc, pfnDialogWndProc;
    PROC pfnTitleWndProc, pfnMDIClientWndProc, pfnComboListBoxProc;
} wow_cleanup_client_procs;
typedef struct wow_cleanup_shared {
    wow_cleanup_client_procs apfnClientA, apfnClientW;
    ATOM atomSysClass[20];
} wow_cleanup_shared;
typedef struct wow_cleanup_context {
    wow_task_order_thread *thread;
    wow_cleanup_shared *shared;
    struct { wow_cleanup_handle *aheList; } handles;
    ULONG last_handle;
    PROC scroll_proc, desktop_proc, menu_proc, switch_proc;
    BOOL (WINAPI *destroy_class)(struct wow_cleanup_context *, wow_class_lookup_entry **);
    void (WINAPI *destroy_object)(struct wow_cleanup_context *, wow_cleanup_handle *);
    PVOID (WINAPI *allocate)(SIZE_T);
    void (WINAPI *release)(PVOID);
} wow_cleanup_context;
BOOL WINAPI _WOWCleanup(HANDLE, DWORD, PNEMODULESEG, DWORD, wow_cleanup_context *);
/* Native composition of the original non-xxx cleanup operation. The USER
 * owner holds its domain serialization and native release operations must not
 * call out or pump messages. Tree links are derived only for this invocation;
 * no native parenting, z-order or ordinary USER tree policy is implemented. */
BOOL WINAPI wow_user_cleanup_bound(HANDLE, DWORD, PNEMODULESEG, DWORD, wow_cleanup_context *);

#ifdef WOW_ORIGINAL_CLEANUP
#define WOW_USER_OBJECT_FLAGS_ONLY
#include "opennt-host/windows/core/ntuser/kernel/userk.h"
#undef WOW_USER_OBJECT_FLAGS_ONLY
typedef wow_cleanup_window *PWND;
typedef wow_class_lookup_entry *PCLS, **PPCLS;
typedef wow_task_order_process *PPROCESSINFO;
typedef wow_cleanup_handle *PHE;
typedef wow_cleanup_process_object *PPROCOBJHEAD;
typedef WNDPROC WNDPROC_PWND;
#undef PtiCurrent
#define PtiCurrent() (context->thread)
#define PpiCurrent() (context->thread->ppi)
#define pclsPrivateList classes->pclsPrivateList
#define pclsPublicList classes->pclsPublicList
#define pclsNext next
#define GETPTI(window) ((window)->thread)
#define gpsi (context->shared)
#define gSharedInfo (context->handles)
#define giheLast (context->last_handle)
#define xxxSBWndProc (context->scroll_proc)
#define xxxDesktopWndProc (context->desktop_proc)
#define xxxMenuWindowProc (context->menu_proc)
#define xxxSwitchWndProc (context->switch_proc)
#define TestWF(window, flag) ((window)->serverSideProc)
#define HANDLEF_DESTROY 0x01
#define DestroyClass(link) context->destroy_class(context,link)
#define HMDestroyUnlockedObject(handle) context->destroy_object(context,handle)
#define UserAllocPoolWithQuota(size, tag) context->allocate(size)
#define UserFreePool(memory) context->release(memory)
#define RIPMSG0(level, message) ((void)0)
#define RIPMSG1(level, message, arg) ((void)0)
#define try __try
#define except __except
#endif
#endif
