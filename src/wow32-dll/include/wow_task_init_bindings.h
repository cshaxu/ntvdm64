#ifndef WOW_TASK_INIT_BINDINGS_H
#define WOW_TASK_INIT_BINDINGS_H
#include "wow_task_order_bindings.h"

/* ADAPTER-WOW-041: explicit original initialization views. Required native
 * queue/profile/presentation operations have no default implementations.
 * These callbacks must not invent task selection or compatibility policy. */
typedef struct wow_task_client_info {
    DWORD dwTIFlags, dwExpWinVer, dwCompatFlags;
} wow_task_client_info;
typedef struct wow_task_init_ops {
    BOOL (WINAPI *journal_hooked)(wow_task_order_process *, BOOL playback);
    BOOL (WINAPI *reattach)(wow_task_order_process *, BOOL journal);
    void (WINAPI *start_cursor)(wow_task_order_process *, DWORD timeout);
    UINT (WINAPI *profile)(wow_task_order_process *, UINT section,
        LPCWSTR key, LPCWSTR fallback, LPWSTR output, DWORD capacity);
} wow_task_init_ops;
NTSTATUS WINAPI xxxInitTask(UINT, PUNICODE_STRING, DWORD, DWORD, DWORD,
    DWORD, DWORD, DWORD, DWORD, WORD, wow_task_order_thread *);
void WINAPI SetAppCompatFlags(wow_task_order_thread *);
void WINAPI _ShowStartGlass(DWORD, wow_task_order_process *);

#ifdef WOW_ORIGINAL_TASK_ORDER
typedef wow_task_order_entry TDB;
typedef wow_task_order_process *PW32PROCESS;
#undef gpsi
#define gpwtiFirst (ppi->shared->pwtiFirst)
#define gppiStarting (ppi->shared->ppiStarting)
#define gfAllowForegroundActivate (ppi->shared->allowForegroundActivate)
#define UserAllocPoolWithQuota(bytes, tag) HeapAlloc(GetProcessHeap(),0,bytes)
#define W32PF_APPSTARTING 0x00000040
#define W32PF_HAVECOMPATFLAGS 0x00000080
#define W32PF_ALLOWFOREGROUNDACTIVATE 0x00000100
#define W32PF_SHOWSTARTGLASSCALLED 0x00000400
#define TIF_FIRSTIDLE 0x00002000
#define CW2_USEDEFAULT 0x8000
#define VER31 0x030a
#define PMAP_COMPAT 14
#define QUERY_VALUE_BUFFER 80
/* This original USER kernel slice is Unicode even in the ANSI WOW client
 * build. Retain its TEXT literals without changing the original bodies. */
#undef TEXT
#define TEXT(value) L##value
#ifndef STATUS_OBJECT_NAME_INVALID
#define STATUS_OBJECT_NAME_INVALID ((NTSTATUS)0xC0000033)
#endif
#define FJOURNALRECORD() ppi->init->journal_hooked(ppi,FALSE)
#define FJOURNALPLAYBACK() ppi->init->journal_hooked(ppi,TRUE)
#define ReattachThreads(journal) ppi->init->reattach(ppi,journal)
#define CalcStartCursorHide(process, timeout) (process)->init->start_cursor(process,timeout)
#define UT_FastGetProfileStringW(section,key,fallback,out,capacity) \
    pti->ppi->init->profile(pti->ppi,section,key,fallback,out,capacity)
#ifndef try
#define try __try
#define except __except
#endif
NTSYSAPI NTSTATUS NTAPI RtlUnicodeStringToInteger(PCUNICODE_STRING, ULONG, PULONG);
#endif
#endif
