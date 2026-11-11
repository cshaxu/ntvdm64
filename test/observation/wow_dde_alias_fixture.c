/* Exact original DDE list/free bodies are supplied by the observation driver.
 * Service doubles exercise ownership and reentry, not the guest ABI, actual
 * heap implementation, DDE transport, native BASE or complete conversations.
 * Keep the original algorithm under test: no copied list implementation. */
#include <windows.h>
#include <dde.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef WORD HAND16;
typedef DWORD VPVOID;
typedef struct { struct { WORD hwnd, wMsg, wParam; DWORD lParam; } WndProc; } PARM16;
#define FASTCALL __stdcall
#define GETHWND16(value) ((WORD)(ULONG_PTR)(value))
#define LOW(value) (*(WORD *)&(value))
#define HIW(value) (*((WORD *)&(value) + 1))
#define WOW32ASSERT(test) ((void)0)
typedef struct _WM32MSGPARAMEX {
    HWND hwnd; UINT uMsg, uParam; LONG lParam; PARM16 Parm16;
    BOOL fThunk, fFree; LONG lReturn;
} WM32MSGPARAMEX, *LPWM32MSGPARAMEX;
#define LOGDEBUG(level, args) ((void)0)
#define WOW32ASSERTMSG(test, text) ((void)0)
#define RET_WOWDDEFREEHANDLE 51
#define malloc_w fixture_malloc
#define free_w fixture_free
#define CallBack16 fixture_callback
#define GlobalLock16 fixture_lock16
#define GlobalUnlockFree16 fixture_free16
#define GlobalFree fixture_native_free
#define DeleteMetaFile fixture_delete_meta
#define UnpackDDElParam fixture_unpack
#define FreeDDElParam fixture_free_packet
#define WI32DDEInitiate fixture_initiating

#include "dde_alias_types.h"

#ifdef FIXTURE_CONVERSION
typedef struct _MSGPARAMEX {
    PARM16 Parm16;
    LONG lParam;
    BOOL ddePostPacked;
} MSGPARAMEX, *LPMSGPARAMEX;
#endif

static PHDDE phDDEFirst;
static unsigned failures, cases, live_nodes, marks, unmarks;
static unsigned native_frees, guest_frees, meta_frees;
static unsigned allocation_fail, last_guest, last_native;
static BOOL reject_mark;
static HAND16 reenter_guest;
static BYTE marked[65536];
static PHDDE retire_on_unmark, quarantined_node;
static UINT packet_low, packet_high;
static unsigned packet_frees;
static BOOL initiating, fThunkDDEmsg = TRUE;

static BOOL DDEDeletehandle(HAND16, HANDLE);
#ifdef FIXTURE_CONVERSION
static BOOL DDEAddhandle(HAND16, HAND16, HAND16, HANDLE, PDDEINFO);
#endif
static BOOL W32DdeFreeHandle16(HAND16);
static BOOL W32DDEFreeGlobalMem32(HANDLE);
#include "dde_alias_mark_prototype.h"
static VOID W32UnMarkDDEHandle(HAND16);
static BOOL FASTCALL WM32DDEAck(LPWM32MSGPARAMEX);

static void *fixture_malloc(size_t bytes)
{
    void *p;
    if (allocation_fail) { --allocation_fail; return NULL; }
    p = malloc(bytes);
    if (p) ++live_nodes;
    return p;
}

static void fixture_free(void *p)
{
    if (p) {
        --live_nodes;
        if (p == retire_on_unmark) {
            /* Poison retired storage deterministically; release quarantine
             * after the tested call. This is a service-level reentry witness,
             * not a claim about real guest allocator/reuse timing. */
            memset(p, 0xdd, sizeof(HDDE));
            quarantined_node = p;
        } else free(p);
    }
}

static BOOL fixture_callback(unsigned type, PARM16 *parm,
    DWORD unused, VPVOID *result)
{
    (void)unused;
    if (type != RET_WOWDDEFREEHANDLE) ++failures;
    if (parm->WndProc.wMsg && reject_mark) return FALSE;
    marked[parm->WndProc.wParam] = (BYTE)(parm->WndProc.wMsg != 0);
    if (parm->WndProc.wMsg) ++marks; else ++unmarks;
    /* Both unchanged guest SetDdeHandleFlag implementations return AX0 on
     * normal completion. CallBack16's BOOL, not this payload, is success. */
    *result = 0;
    if (!parm->WndProc.wMsg && retire_on_unmark && !quarantined_node) {
        HAND16 key16 = retire_on_unmark->hMem16;
        HANDLE key32 = retire_on_unmark->hMem32;
        DDEDeletehandle(key16, key32);
    }
    if (!parm->WndProc.wMsg && reenter_guest) {
        HAND16 key = reenter_guest;
        reenter_guest = 0;
        W32DdeFreeHandle16(key);
    }
    return TRUE;
}

static VPVOID fixture_lock16(HAND16 handle, void *size)
{
    (void)size;
    return handle;
}

static HAND16 fixture_free16(VPVOID pointer)
{
    HAND16 key = (HAND16)pointer;
    if (marked[key]) ++failures; /* Original free must clear its arena hook. */
    ++guest_frees;
    last_guest = key;
    return 0;
}

static HGLOBAL fixture_native_free(HGLOBAL handle)
{
    /* Observed modern BASE contract: notify before free, FALSE vetoes it. */
    if (W32DDEFreeGlobalMem32(handle)) {
        ++native_frees;
        last_native = (unsigned)(ULONG_PTR)handle;
    }
    return NULL;
}

static BOOL fixture_delete_meta(HMETAFILE handle)
{
    ++meta_frees;
    last_native = (unsigned)(ULONG_PTR)handle;
    return TRUE;
}

static BOOL fixture_unpack(UINT message, LPARAM packed, PUINT low, PUINT high)
{
    (void)message; (void)packed;
    *low = packet_low; *high = packet_high;
    return TRUE;
}

static BOOL fixture_free_packet(UINT message, LPARAM packed)
{
    (void)message; (void)packed; ++packet_frees;
    return TRUE;
}

static BOOL fixture_initiating(HAND16 window)
{
    (void)window;
    return initiating;
}

/* Bodies remain externally linked exactly as extracted; previous prototypes
 * carry internal linkage in this single translation-unit fixture. */
#include "dde_alias_exact_bodies.h"

#define H(value) ((HANDLE)(ULONG_PTR)(value))
#define CHECK(name, condition) do { ++cases; if (!(condition)) { \
    ++failures; printf("FAIL %s\n", name); } } while (0)

static BOOL add(HAND16 to, HAND16 from, HAND16 guest, unsigned native,
    WORD message, WORD flags)
{
    DDEINFO info;
    memset(&info, 0, sizeof(info));
    info.Msg = message;
    info.Flags = flags;
    return DDEAddhandle(to, from, guest, H(native), &info);
}

static BOOL delete_empty(void)
{
    BOOL result = TRUE;
    __try { result = DDEDeletehandle(1, H(101)); }
    __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        return FALSE;
    }
    return !result;
}

int __cdecl main(void)
{
    unsigned before;
    CHECK("empty-delete-false", delete_empty());
    CHECK("empty-find16", !DDEFindPair16(1, 2, H(101)));
    CHECK("empty-find32", !DDEFindPair32(1, 2, 1));
    CHECK("empty-node16", !DDEFindNode16(1));
    CHECK("empty-node32", !DDEFindNode32(H(101)));
    CHECK("empty-ack", !DDEFindAckNode(1, 2, H(101)));
    CHECK("null-guest-rejected", !add(1, 2, 0, 101, WM_DDE_DATA, DDE_PACKET));
    CHECK("null-native-rejected", !add(1, 2, 1, 0, WM_DDE_DATA, DDE_PACKET));
    allocation_fail = 1;
    CHECK("node-oom-rejected", !add(1, 2, 1, 101, WM_DDE_DATA, DDE_PACKET));
    CHECK("node-oom-no-publication", !phDDEFirst && !live_nodes && !marks);
    reject_mark = TRUE;
    CHECK("mark-reject-unpublished", !add(1, 2, 7, 107, WM_DDE_DATA, DDE_PACKET));
    reject_mark = FALSE;
    CHECK("mark-reject-baseline-cleanup", !phDDEFirst || DDEDeletehandle(7, H(107)));
    CHECK("mark-reject-no-mark", !marked[7]);

    CHECK("add-tail", add(1, 2, 1, 101, WM_DDE_DATA, DDE_PACKET));
    CHECK("add-middle", add(3, 4, 2, 102, WM_DDE_POKE, DDE_PACKET));
    CHECK("add-head", add(5, 6, 3, 103, WM_DDE_ADVISE, DDE_PACKET));
    CHECK("mark-zero-is-success", marks == 3 && live_nodes == 3);
    CHECK("pair16-correct", DDEFindPair16(3, 4, H(102)) == 2);
    CHECK("pair16-peer-isolation", !DDEFindPair16(4, 3, H(102)));
    CHECK("pair32-correct", DDEFindPair32(3, 4, 2) == H(102));
    CHECK("pair32-peer-isolation", !DDEFindPair32(4, 3, 2));
    CHECK("find-node16", DDEFindNode16(2) == phDDEFirst->pDDENext);
    CHECK("find-node32", DDEFindNode32(H(102)) == phDDEFirst->pDDENext);
    CHECK("find-ack", DDEFindAckNode(3, 4, H(102)) == phDDEFirst->pDDENext);
    CHECK("ack-peer-isolation", !DDEFindAckNode(4, 3, H(102)));
    CHECK("missing-pair-false", !DDEDeletehandle(2, H(999)) && live_nodes == 3);
    CHECK("delete-middle", DDEDeletehandle(2, H(102)) && live_nodes == 2);
    CHECK("delete-tail", DDEDeletehandle(1, H(101)) && live_nodes == 1);
    CHECK("delete-head", DDEDeletehandle(3, H(103)) && !phDDEFirst && !live_nodes);
    /* Removing an alias node alone does not free payloads or clear marks. */
    CHECK("node-only-removal", native_frees == 0 && guest_frees == 0 && unmarks == 0);
    memset(marked, 0, sizeof(marked));

    before = native_frees;
    CHECK("guest-free-add", add(1, 2, 11, 111, WM_DDE_DATA, DDE_PACKET));
    CHECK("guest-free-notification", W32DdeFreeHandle16(11));
    CHECK("guest-free-native-once", native_frees == before + 1 && last_native == 111);
    CHECK("guest-free-node-removed", !phDDEFirst && !live_nodes);
    CHECK("guest-free-does-not-free-guest", guest_frees == 0);
    marked[11] = 0; /* Actual guest allocator completes its own free. */

    before = guest_frees;
    CHECK("native-free-add1", add(1, 2, 12, 112, WM_DDE_DATA, DDE_PACKET));
    CHECK("native-free-add2", add(3, 4, 13, 112, WM_DDE_DATA, DDE_PACKET));
    CHECK("native-free-allows-free", W32DDEFreeGlobalMem32(H(112)));
    CHECK("native-free-both-aliases", guest_frees == before + 2 && last_guest == 12);
    CHECK("native-free-unmarked", !marked[12] && !marked[13]);
    CHECK("native-free-list-empty", !phDDEFirst && !live_nodes);

    before = native_frees;
    CHECK("execute-add", add(1, 2, 14, 114, WM_DDE_EXECUTE, DDE_PACKET));
    CHECK("execute-guest-veto", !W32DdeFreeHandle16(14));
    CHECK("execute-retains-alias", DDEFindNode16(14) != NULL && live_nodes == 1);
    CHECK("execute-records-late-free", (DDEFindNode16(14)->DdeFlags & DDE_EXECUTE_FREE_MEM) != 0);
    CHECK("execute-not-freed-early", native_frees == before);
    /* ACK behavior is a separate exact converter obligation, not simulated
     * here. Exercise native eventual release, not a claimed ACK roundtrip. */
    CHECK("execute-native-final-release", W32DDEFreeGlobalMem32(H(114)));
    CHECK("execute-final-empty", !live_nodes && !phDDEFirst && !marked[14]);

    before = meta_frees;
    CHECK("meta-add", add(1, 2, 15, 115, WM_DDE_DATA, DDE_METAFILE));
    CHECK("meta-native-veto", !W32DDEFreeGlobalMem32(H(115)));
    CHECK("meta-delete-not-global", meta_frees == before + 1 && last_native == 115);
    CHECK("meta-final-empty", !live_nodes && !phDDEFirst && !marked[15]);
    before = meta_frees;
    CHECK("meta-guest-add", add(1, 2, 16, 116, WM_DDE_DATA, DDE_METAFILE));
    CHECK("meta-guest-allows-free", W32DdeFreeHandle16(16));
    CHECK("meta-guest-delete", meta_frees == before + 1 && !phDDEFirst && !live_nodes);
    marked[16] = 0;

    /* Real original callbacks may reenter. Remove an unrelated alias while
     * native retirement is in progress; it must not be skipped or freed twice. */
    before = native_frees;
    CHECK("reentry-add-other", add(1, 2, 17, 117, WM_DDE_DATA, DDE_PACKET));
    CHECK("reentry-add-target", add(3, 4, 18, 118, WM_DDE_DATA, DDE_PACKET));
    reenter_guest = 17;
    CHECK("reentry-native-free", W32DDEFreeGlobalMem32(H(118)));
    CHECK("reentry-other-once", native_frees == before + 1 && last_native == 117);
    CHECK("reentry-target-once", last_guest == 18 && !marked[18]);
    CHECK("reentry-list-empty", !phDDEFirst && !live_nodes);
    marked[17] = 0;
    CHECK("unknown-guest-free", W32DdeFreeHandle16(99));
    CHECK("unknown-native-free", W32DDEFreeGlobalMem32(H(199)));
    CHECK("null-native-veto", !W32DDEFreeGlobalMem32(NULL));
    /* Complete original ACK body, with only packing/window/guest services
     * doubled. HDDE remains the same original list used above. */
    {
        WM32MSGPARAMEX ack;
        unsigned mode;
        for (mode = 0; mode < 3; ++mode) {
            unsigned old_native = native_frees, old_guest = guest_frees;
            memset(&ack, 0, sizeof(ack));
            ack.hwnd = (HWND)1; ack.uParam = 2; ack.uMsg = WM_DDE_ACK;
            ack.fThunk = TRUE; ack.fFree = TRUE;
            packet_low = 0x8000; packet_high = 0x10072;
            CHECK("ack-add", add(2, 1, 20, packet_high, WM_DDE_EXECUTE,
                DDE_PACKET | (mode == 1 ? DDE_EXECUTE_FREE_MEM :
                    mode == 2 ? DDE_EXECUTE_FREE_H16 : 0)));
            phDDEFirst->h16 = 77;
            CHECK("ack-convert", WM32DDEAck(&ack));
            CHECK("ack-result", LOWORD(ack.Parm16.WndProc.lParam) == 0x8000 &&
                HIWORD(ack.Parm16.WndProc.lParam) == (mode == 2 ? 77 : 20));
            CHECK("ack-release-policy", native_frees == old_native + 1 &&
                guest_frees == old_guest + (mode ? 1 : 0) &&
                !live_nodes && !phDDEFirst);
            marked[20] = 0;
        }
        CHECK("ack-reentry-other", add(3, 4, 21, 121, WM_DDE_DATA, DDE_PACKET));
        CHECK("ack-reentry-target", add(2, 1, 22, packet_high, WM_DDE_EXECUTE,
            DDE_PACKET | DDE_EXECUTE_FREE_H16));
        phDDEFirst->h16 = 78;
        retire_on_unmark = phDDEFirst;
        CHECK("ack-reentry-convert", WM32DDEAck(&ack));
        CHECK("ack-retired-node-captured", last_guest == 22 &&
            HIWORD(ack.Parm16.WndProc.lParam) == 78 && live_nodes == 1);
        retire_on_unmark = NULL;
        free(quarantined_node); quarantined_node = NULL;
        CHECK("ack-reentry-tail-retained", DDEFindNode16(21) &&
            DDEDeletehandle(21, H(121)) && !phDDEFirst && !live_nodes);
        marked[21] = 0;
        CHECK("ack-wrapper-release", packet_frees == 4);
    }
#ifdef FIXTURE_CONVERSION
    /* Candidate-only exact original conversion bodies. A failed conversion
     * must not clear a mark still owned by an independent association, and
     * an EXECUTE compatibility alias is owned by the conversion. These are
     * deterministic service-double checks, not actual guest/post evidence. */
    {
        DDECONVERSION conversion;
        DDEINFO info;
        unsigned old_native = native_frees, old_guest = guest_frees;
        memset(&info, 0, sizeof(info));
        info.Msg = WM_DDE_DATA; info.Flags = DDE_PACKET;
        CHECK("rollback-peer-add", add(3, 4, 30, 130, WM_DDE_DATA, DDE_PACKET));
        DDEBeginConversion(&conversion);
        CHECK("rollback-track", DDETrackhandle(&conversion, 1, 2, 30, H(131), &info));
        CHECK("rollback-mark-before", marked[30] && live_nodes == 2);
        DDERollback32(&conversion, 1, 2);
        CHECK("rollback-peer-retained", DDEFindPair32(3, 4, 30) == H(130));
        CHECK("rollback-peer-mark-retained", marked[30]);
        CHECK("rollback-new-native-once", native_frees == old_native + 1 && last_native == 131);
        CHECK("rollback-peer-cleanup", DDEDeletehandle(30, H(130)) && !live_nodes);
        marked[30] = 0;
        DDEBeginConversion(&conversion);
        info.Msg = WM_DDE_EXECUTE; info.Flags = DDE_PACKET | DDE_EXECUTE_FREE_H16;
        info.h16 = 77;
        CHECK("rollback-execute-track", DDETrackhandle(&conversion, 1, 2, 31, H(132), &info));
        DDERollback32(&conversion, 1, 2);
        CHECK("rollback-execute-alias-freed", guest_frees == old_guest + 1 && last_guest == 31);
        CHECK("rollback-execute-native-freed", native_frees == old_native + 2 && last_native == 132);
        CHECK("rollback-execute-unmarked", !marked[31]);
        CHECK("rollback-execute-empty", !phDDEFirst && !live_nodes && !conversion.Count);
        {
            MSGPARAMEX message;
            unsigned old_packets = packet_frees;
            memset(&message, 0, sizeof(message));
            message.Parm16.WndProc.hwnd = 1;
            message.Parm16.WndProc.wParam = 2;
            message.Parm16.WndProc.wMsg = WM_DDE_POKE;
            message.lParam = 0x1234;
            message.ddePostPacked = TRUE;
            DDEBeginConversion(&conversion);
            info.Msg = WM_DDE_POKE; info.Flags = DDE_PACKET;
            CHECK("post-cancel-track", DDETrackhandle(&conversion, 1, 2, 33, H(134), &info));
            SetLastError(ERROR_ACCESS_DENIED);
            W32CancelDDEPost(&message, &conversion);
            CHECK("post-cancel-error-preserved", GetLastError() == ERROR_ACCESS_DENIED);
            CHECK("post-cancel-packet-once", packet_frees == old_packets + 1 && !message.ddePostPacked);
            CHECK("post-cancel-native-once", native_frees == old_native + 3 && last_native == 134);
            CHECK("post-cancel-guest-retained", guest_frees == old_guest + 1 && !marked[33]);
            W32CancelDDEPost(&message, &conversion);
            CHECK("post-cancel-idempotent", packet_frees == old_packets + 1 && native_frees == old_native + 3);
            CHECK("post-cancel-empty", !phDDEFirst && !live_nodes && !conversion.Count);
        }
    }
#endif
    printf("DDE_ALIAS cases=%u errors=%u nodes=%u guest_proof=0 transport_proof=0\n",
        cases, failures, live_nodes);
    return failures ? 1 : 0;
}
