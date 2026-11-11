/* Test-only dependency spies for the pinned original _ReplyMessage body.
 * Not a USER provider, scheduler, or guest-acceptance fixture. */
#include <windows.h>
#include <stdio.h>
#include <string.h>

#define SMF_REPLY 0x0001
#define SMF_SENDERDIED 0x0004
#define SMF_CB_REQUEST 0x0100
#define SMF_CB_CLIENT 0x0400
#define ISM_CALLBACK 0x0001
#define ISM_REPLY 0x0020
#define ISM_CB_CLIENT 0x0100
#define TIF_16BIT 0x00000002
#define QS_SMSREPLY 0x0200

typedef struct test_thread THREADINFO, *PTHREADINFO;
typedef struct test_sms {
    UINT flags, message;
    LONG lRet;
    PTHREADINFO ptiSender, ptiCallBackSender;
    HWND spwnd;
    void *lpResultCallBack;
    DWORD dwData;
} SMS, *PSMS;
struct test_thread { UINT TIF_flags; PSMS psmsCurrent, psmsSent; };
typedef struct { UINT fuCall; void *lpResultCallBack; DWORD dwData; LONG lRet; } INTRSENDMSGEX;
typedef unsigned TL;
static THREADINFO sender, receiver;
static SMS message, other;
static unsigned errors, callbacks, locks, cases;
static char sequence[16];
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr,"FAIL line=%u\n",(unsigned)__LINE__); } } while (0)
static void step(char c) { size_t n = strlen(sequence); CHECK(n + 1 < sizeof(sequence)); if (n + 1 < sizeof(sequence)) { sequence[n] = c; sequence[n+1] = 0; } }
#define CheckCritIn() ((void)0)
static PTHREADINFO PtiCurrent(void) { return &receiver; }
static void SetWakeBit(PTHREADINFO thread, UINT bits) {
    CHECK(thread == &sender && bits == QS_SMSREPLY);
    CHECK((message.flags & SMF_REPLY) && message.lRet == 114);
    step('W');
}
static void DirectedScheduleTask(PTHREADINFO from, PTHREADINFO to, BOOL send, PSMS sms) {
    CHECK(from == &receiver && to == &sender && !send && sms == &message);
    step('D');
}
static BOOL xxxSleepTask(BOOL idle, HANDLE event) {
    CHECK(idle && !event); step('Y'); return FALSE;
}
static void ThreadLockWithPti(PTHREADINFO thread, HWND window, TL *lock) {
    CHECK(thread == &receiver && window == message.spwnd); *lock = 1; ++locks;
}
static void ThreadUnlock(TL *lock) { CHECK(*lock == 1); --locks; }
static LONG xxxInterSendMsgEx(HWND window, UINT msg, DWORD wp, DWORD lp,
    PTHREADINFO from, PTHREADINFO to, INTRSENDMSGEX *ism) {
    CHECK(window == message.spwnd && msg == message.message && !wp && !lp && !from);
    CHECK(to == message.ptiCallBackSender);
    CHECK(ism->fuCall == (ISM_CALLBACK | ISM_REPLY | ISM_CB_CLIENT));
    CHECK(ism->lRet == 114 && ism->dwData == message.dwData);
    CHECK(ism->lpResultCallBack == message.lpResultCallBack);
    ++callbacks; return 0;
}

/* Generated verbatim from pinned OpenNT sendmsg.c; no rewritten algorithm. */
#include "original_reply_body.inc"

static void reset(UINT from, UINT to, BOOL exact) {
    ZeroMemory(&sender, sizeof(sender)); ZeroMemory(&receiver, sizeof(receiver));
    ZeroMemory(&message, sizeof(message)); ZeroMemory(&other, sizeof(other));
    ZeroMemory(sequence, sizeof(sequence)); callbacks = locks = 0;
    sender.TIF_flags = from; receiver.TIF_flags = to;
    sender.psmsSent = exact ? &message : &other;
    receiver.psmsCurrent = &message; message.ptiSender = &sender;
}
int main(void) {
    unsigned s, r, exact;
    for (s = 0; s < 2; ++s) for (r = 0; r < 2; ++r) for (exact = 0; exact < 2; ++exact) {
        const char *expected = !(s || r) ? "W" : r && exact ? "WDY" : "WD";
        reset(s ? TIF_16BIT : 0, r ? TIF_16BIT : 0, exact);
        CHECK(_ReplyMessage(114)); CHECK(strcmp(sequence, expected) == 0);
        CHECK(!_ReplyMessage(999) && message.lRet == 114);
        CHECK(strcmp(sequence, expected) == 0);
        printf("REPLY sender16=%u receiver16=%u exact=%u order=%s\n", s, r, exact, sequence);
        ++cases;
    }
    reset(TIF_16BIT, TIF_16BIT, TRUE); receiver.psmsCurrent = NULL;
    CHECK(!_ReplyMessage(114) && !sequence[0]); ++cases;
    reset(0, TIF_16BIT, TRUE); message.ptiSender = NULL;
    CHECK(_ReplyMessage(114) && !sequence[0] && !callbacks); ++cases;
    reset(0, TIF_16BIT, TRUE); message.ptiSender = NULL;
    message.flags = SMF_CB_REQUEST | SMF_CB_CLIENT;
    message.ptiCallBackSender = &sender;
    CHECK(_ReplyMessage(114) && callbacks == 1 && !locks && !sequence[0]); ++cases;
    reset(0, TIF_16BIT, TRUE); message.ptiSender = NULL;
    message.flags = SMF_CB_REQUEST | SMF_CB_CLIENT | SMF_SENDERDIED;
    CHECK(_ReplyMessage(114) && !callbacks && !locks && !sequence[0]); ++cases;
    printf("WOW_ORIGINAL_REPLY_CONTRACT cases=%u errors=%u\n", cases, errors);
    return errors ? 1 : 0;
}
