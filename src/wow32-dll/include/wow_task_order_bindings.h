#ifndef WOW_TASK_ORDER_BINDINGS_H
#define WOW_TASK_ORDER_BINDINGS_H
#include <windows.h>
#include <winternl.h>

/* ADAPTER-WOW-039: finite view of original TDB/WOWPROCESSINFO/PROCESSINFO
 * fields used by the recovered scheduler/registration owners, not private
 * NT4 structure layouts or guest data.
 * A future complete scheduler owner must serialize every list mutation and
 * retain every task until unlinked. Registration allocates a process view,
 * not a native process, task or thread. */
typedef struct wow_task_order_entry {
    struct wow_task_order_entry *ptdbNext;
    int nPriority;
    int nEvents;
    struct wow_task_order_thread *pti;
    struct wow_task_order_waiter *pwti;
    DWORD hTaskWow;
} wow_task_order_entry;
typedef struct wow_task_order_state {
    wow_task_order_entry *ptdbHead;
    struct wow_task_order_thread *ptiScheduled;
    DWORD nTaskLock;
    DWORD nSendLock;
    DWORD nRecvLock;
    struct wow_task_order_thread *CSOwningThread;
    LONG CSLockCount;
    HANDLE pEventWowExec;
    HANDLE hEventWowExecClient;
    DWORD lpfnWowExitTask;
    struct wow_task_order_state *pwpiNext;
} wow_task_order_state;
typedef struct wow_task_order_waiter {
    struct wow_task_order_waiter *pwtiNext;
    HANDLE pIdleEvent;
    DWORD idTask;
} wow_task_order_waiter;
typedef struct wow_task_order_shared {
    int nEvents;
    wow_task_order_waiter *pwtiFirst;
    wow_task_order_state *pwpiFirstWow;
    struct wow_task_order_process *ppiStarting;
    BOOL allowForegroundActivate;
} wow_task_order_shared;
/* Original USERSTARTUPINFO field layout, not native STARTUPINFO. */
typedef struct wow_task_startup {
    DWORD cb, dwX, dwY, dwXSize, dwYSize, dwFlags;
    WORD wShowWindow, cbReserved2;
} wow_task_startup;
struct wow_task_init_ops;
typedef struct wow_task_order_process {
    wow_task_order_state *pwpi;
    wow_task_order_shared *shared; /* Same domain for all participating views. */
    DWORD W32PF_Flags;
    struct wow_task_order_process *ppiNext;
    wow_task_startup usi;
    DWORD dwHotkey, dwCompatFlags;
    UNICODE_STRING imagePathName;
    const struct wow_task_init_ops *init;
    PVOID init_context;
    struct wow_class_lookup_context *classes; /* Same original USER process. */
    struct wow_user_object_table *objects;
} wow_task_order_process;
typedef struct wow_task_order_message {
    DWORD flags;
    struct wow_task_order_thread *ptiReceiver;
    struct wow_task_order_thread *ptiSender;
} wow_task_order_message;
typedef struct wow_task_order_queue {
    WORD fsWakeMask;
    WORD fsChangeBits;
    /* Original Q::fsWakeBits is sampled by userk.h::xxxReceiveMessages.
     * The standalone bridge mirrors only the observable native queue bit;
     * it never manufactures an SMS list or a second message queue. */
    WORD fsWakeBits;
    UINT cThreads;
} wow_task_order_queue;
/* Required host operations at the original USER boundary. No missing
 * operation is silently treated as success. The production owner must
 * implement message/death/idle semantics before registering these entries. */
typedef struct wow_task_host_ops {
    void (WINAPI *release_lock)(struct wow_task_order_thread *);
    void (WINAPI *acquire_lock)(struct wow_task_order_thread *);
    void (WINAPI *check_death)(struct wow_task_order_thread *);
    void (WINAPI *deliver_apc)(struct wow_task_order_thread *);
    BOOL (WINAPI *idle_hooked)(struct wow_task_order_thread *);
    void (WINAPI *idle_hook)(struct wow_task_order_thread *);
    void (WINAPI *wake_input_idle)(struct wow_task_order_thread *);
    void (WINAPI *receive)(struct wow_task_order_thread *);
    /* The standalone owner waits one original task/WOWEXEC event set plus
     * the current thread's one public USER32 queue.  This replaces only the
     * unavailable kernel wait primitive; task selection remains taskman.c. */
    NTSTATUS (WINAPI *wait)(struct wow_task_order_thread *, DWORD,
        const HANDLE *, BOOL);
    /* Lookup uses the existing host thread identity; borrowed result remains
     * live in the same USER domain while the caller holds its lock. */
    struct wow_task_order_thread *(WINAPI *find_thread)(struct wow_task_order_thread *, DWORD);
} wow_task_host_ops;
typedef struct wow_task_order_thread {
    wow_task_order_process *ppi;
    wow_task_order_entry *ptdb;
    UINT TIF_flags;
    wow_task_order_message *psmsSent;
    HANDLE pEventQueueServer;
    wow_task_order_message *psmsCurrent;
    wow_task_order_queue *pcti;
    HANDLE *apEvent;
    const wow_task_host_ops *host;
    PVOID host_context;
    PUNICODE_STRING pstrAppName;
    struct wow_task_client_info *pClientInfo;
    DWORD dwExpWinVer, dwCompatFlags;
    /* Original USER membership precedes scheduler TDB insertion. */
    LIST_ENTRY PtiLink;
    struct wow_input_desktop *rpdesk;
    wow_task_order_queue *pqAttach;
    wow_task_order_queue *pq;
} wow_task_order_thread;
void WINAPI InsertTask(wow_task_order_process *, wow_task_order_entry *);
void WINAPI WakeWowTask(wow_task_order_thread *);
void WINAPI DirectedScheduleTask(wow_task_order_thread *, wow_task_order_thread *,
    BOOL, wow_task_order_message *);
DWORD WINAPI CurrentTaskLock(DWORD, wow_task_order_process *);
/* DestroyTask consumes heap-owned task/waiter records and their independently
 * retained idle-event handle. Thread and process records survive the call. */
void WINAPI DestroyTask(wow_task_order_process *, wow_task_order_thread *);
BOOL WINAPI xxxSleepTask(BOOL, HANDLE, wow_task_order_thread *);
BOOL WINAPI xxxUserYield(wow_task_order_thread *);
void WINAPI xxxDirectedYield(DWORD, wow_task_order_thread *);
typedef VOID (WINAPI *wow_task_end_callback)(VOID);
BOOL WINAPI xxxRegisterUserHungAppHandlers(wow_task_end_callback, HANDLE, wow_task_order_process *);
BOOL WINAPI wow_task_reference_event(HANDLE, HANDLE *);
#define WOW_TASK_TIF_16BIT 0x00000002u
#define WOW_TASK_TIF_FIRSTIDLE 0x00002000u
#define WOW_TASK_SMF_RECEIVE 0x1000u
#define WOW_TASK_SMF_SEND 0x2000u

#ifdef WOW_ORIGINAL_TASK_ORDER
typedef wow_task_order_entry *PTDB;
typedef wow_task_order_state *PWOWPROCESSINFO;
typedef wow_task_order_state WOWPROCESSINFO;
typedef wow_task_end_callback PFNW32ET;
typedef wow_task_order_process *PPROCESSINFO;
typedef wow_task_order_thread *PTHREADINFO;
typedef wow_task_order_message *PSMS;
typedef wow_task_order_waiter *PWOWTHREADINFO;
#define TIF_16BIT WOW_TASK_TIF_16BIT
#define SMF_WOWRECEIVE WOW_TASK_SMF_RECEIVE
#define SMF_WOWSEND WOW_TASK_SMF_SEND
#define SMF_REPLY 0x0001
#define SMF_RECEIVEDMESSAGE 0x0010
#define SMF_RECEIVERBUSY 0x4000
#define TIF_SHAREDWOW 0x00001000
#define W32PF_WAKEWOWEXEC 0x00010000
#define HEVENT_REMOVEME ((HANDLE)0xFFFFFFFF)
#define QS_EVENT 0x2000
/* Original winuser.h mask excludes modern QS_RAWINPUT. This is the
 * historical queue view, not the native host's GetQueueStatus mask. */
#undef QS_INPUT
#define QS_INPUT (QS_MOUSE | QS_KEY)
#undef QS_ALLINPUT
#define QS_ALLINPUT (QS_INPUT | QS_POSTMESSAGE | QS_TIMER | QS_PAINT | QS_HOTKEY | QS_SENDMESSAGE)
#define POLL_EVENT_CNT 5
#define IEV_TASK 3
#define IEV_WOWEXEC 4
#define STATUS_WAIT_1 ((NTSTATUS)1)
typedef HANDLE PKEVENT;
#define KeClearEvent(event) ResetEvent(event)
#define ExAllocatePoolWithTag(pool, bytes, tag) HeapAlloc(GetProcessHeap(),0,bytes)
#define KeWaitForMultipleObjects(count, events, kind, reason, mode, alert, timeout, blocks) \
    pti->host->wait(pti,(count),(events),(alert))
#define LeaveCrit() pti->host->release_lock(pti)
#define EnterCrit() pti->host->acquire_lock(pti)
#define CheckForClientDeath() pti->host->check_death(pti)
#define ClientDeliverUserApc() pti->host->deliver_apc(pti)
#define IsHooked(thread, hook) ((thread)->host->idle_hooked(thread))
#define xxxCallHook(code, wp, lp, hook) pti->host->idle_hook(pti)
#define WakeInputIdle(thread) (thread)->host->wake_input_idle(thread)
#define xxxReceiveMessages(thread) (thread)->host->receive(thread)
#define RIPMSG2(level, text, first, second) ((void)0)
#define RIPMSG0(level, text) ((void)0)
#define DY_OLDYIELD ((DWORD)-1)
#define PtiFromThreadId(id) ptiOld->host->find_thread(ptiOld, id)
/* Original single gpsi domain, supplied by the sender's process view.
 * This does not create separate per-process counters or message queues. */
#define gpsi (ptiOld->ppi->shared)
/* The reached KeSetEvent return value is unused; native event ownership
 * and lifetime remain the complete scheduler binding's responsibility. */
#define KeSetEvent(event, increment, wait) SetEvent(event)
#define ObDereferenceObject(event) CloseHandle(event)
#define UserFreePool(pointer) HeapFree(GetProcessHeap(), 0, pointer)
/* These original checks are non-evaluating in the selected retail profile;
 * this is not a substitute for the scheduler owner's critical section. */
#define CheckCritIn() ((void)0)
#define UserAssert(expression) ((void)0)
#endif
#endif
