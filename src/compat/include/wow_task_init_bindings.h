#ifndef WOW_TASK_INIT_BINDINGS_H
#define WOW_TASK_INIT_BINDINGS_H
#include "wow_task_order_bindings.h"

/* ADAPTER-WOW-041: explicit original initialization views. Required native
 * queue/profile/presentation operations have no default implementations.
 * These callbacks must not invent task selection or compatibility policy. */
typedef struct wow_task_client_info {
    DWORD dwTIFlags, dwExpWinVer, dwCompatFlags, dwCompatFlags2;
    /* Read-only projection for retained client consumers, refreshed from
     * the bound task on access. Not a native TEB CLIENTINFO layout. */
    DWORD CI_flags;
    /* Inactive guest CLIENTINFO callback projection only. While executing,
     * the guest TEB is authoritative; the existing callback borrow, never
     * this scalar snapshot, owns the pointed-to generation. */
    ULONG guest_callback[2];
} wow_task_client_info;
#define WOW_TASK_CI_16BIT 0x00000004u
/* XP windows/published/winuser.w:407,435,440,450; original WOW mask. */
#define GACF2_GIVEUPFOREGROUND 0x00000040
#define GACF2_DDENOSYNC 0x00040000
#define GACF2_FORCEFUSION 0x00800000
#define COMPATFLAGS2_FORWOW (GACF2_DDENOSYNC | GACF2_GIVEUPFOREGROUND | GACF2_FORCEFUSION)
typedef struct wow_task_init_ops {
    BOOL (WINAPI *journal_hooked)(wow_task_order_process *, BOOL playback);
    BOOL (WINAPI *reattach)(wow_task_order_process *, BOOL journal);
    void (WINAPI *start_cursor)(wow_task_order_process *, DWORD timeout);
    UINT (WINAPI *profile)(wow_task_order_process *, UINT section,
        LPCWSTR key, LPCWSTR fallback, LPWSTR output, DWORD capacity);
} wow_task_init_ops;
NTSTATUS WINAPI zzzInitTask(UINT, DWORD, DWORD, PUNICODE_STRING, PUNICODE_STRING, DWORD, DWORD, DWORD,
    DWORD, DWORD, DWORD, DWORD);
void WINAPI SetAppStarting(wow_task_order_process *);
void WINAPI zzzShowStartGlass(DWORD);

#ifdef WOW_ORIGINAL_TASK_ORDER
typedef wow_task_order_entry TDB;
/* Uninstantiated XP kernel lookaside declarations; no allocator is imported. */
typedef struct _PAGED_LOOKASIDE_LIST *PPAGED_LOOKASIDE_LIST;
#define W32GetCurrentThread() (wow_user_runtime_current()->thread)
#define TIF_MEOW 0x00800000u
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif
#define RIPMSG1(level, text, arg) ((void)0)
#undef gpsi
#define gpwtiFirst (ppi->shared->pwtiFirst)
#define gppiStarting (ppi->shared->ppiStarting)
#define gfAllowForegroundActivate (ppi->shared->allowForegroundActivate)
/* XP usergdi.h moved the quota pool entry out of userk.h. Keep the
 * existing non-raising, process-heap binding for task initialization. */
#define UserAllocPoolWithQuota(bytes, tag) HeapAlloc(GetProcessHeap(),0,bytes)
#define W32PF_APPSTARTING 0x00000040
#define W32PF_HAVECOMPATFLAGS 0x00000080
#define W32PF_ALLOWFOREGROUNDACTIVATE 0x00000100
#define W32PF_SHOWSTARTGLASSCALLED 0x00000400
#define TIF_FIRSTIDLE WOW_TASK_TIF_FIRSTIDLE
#define CW2_USEDEFAULT 0x8000
#define VER31 0x030a
#define HTW_IS16BIT 0x80000000u
#define HTW_ISMEOW 0x40000000u
#define TDBF_SETUP 1
#define W32PF_SETUPAPP 0x01000000u
#define IMECOMPAT_NOFINALIZECOMPSTR 0x00400000u
#define IMECOMPAT_HYDRACLIENT 0x00800000u
#define ZwQueryDefaultLocale NtQueryDefaultLocale
#define TAGMSG0(tag,text) ((void)0)
#define TAGMSG1(tag,text,arg) ((void)0)
#define SET_PUDF(flag) (ppi->shared->allowForegroundActivate = TRUE)
/* This original USER kernel slice is Unicode even in the ANSI WOW client
 * build. Retain its TEXT literals without changing the original bodies. */
#undef TEXT
#define TEXT(value) L##value
#ifndef STATUS_OBJECT_NAME_INVALID
#define STATUS_OBJECT_NAME_INVALID ((NTSTATUS)0xC0000033)
#endif
#define FJOURNALRECORD() ppi->init->journal_hooked(ppi,FALSE)
#define FJOURNALPLAYBACK() ppi->init->journal_hooked(ppi,TRUE)
#define zzzReattachThreads(journal) ppi->init->reattach(ppi,journal)
#define zzzCalcStartCursorHide(process, timeout) (process)->init->start_cursor(process,timeout)
#define UT_FastGetProfileStringW(section,key,fallback,out,capacity) \
    pti->ppi->init->profile(pti->ppi,section,key,fallback,out,capacity)
#ifndef try
#define try __try
#define except __except
#endif
#endif
#endif
