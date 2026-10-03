#ifndef FRONTEND_WINDOW_KEYBOARD_H
#define FRONTEND_WINDOW_KEYBOARD_H

#include <windows.h>
#include "lib/kvm-base/event_interface.h"
#include "window_input_queue.h"

/* Format conversion only. Call on the frontend input owner, passing its
 * reconciled native modifier/toggle state (including left/right Alt/Ctrl).
 * Do not query GetKeyState from the controller thread's unrelated input queue.
 * Physical scans remain physical; independent text uses the existing NT
 * scan-less/paste normalization, never a second keyboard translator. */
BOOL frontend_window_keyboard_record(const kvm_input_event *event,
    DWORD control_state, HKL layout, INPUT_RECORD *record);

/* Only records actually delivered to the selected backend enter this ledger. Host
 * shortcuts must be consumed before accept. Native control_state is captured
 * in the Window UI input callback, not sampled later on a different event thread. */
typedef struct frontend_window_keyboard_state {
    KEY_EVENT_RECORD held[512];
    lib_u64 source_identity;
    DWORD locks;
    unsigned held_count;
    BOOL releasing, retiring;
} frontend_window_keyboard_state;

BOOL frontend_window_keyboard_accept(frontend_window_keyboard_state *state,
    const kvm_input_event *event, DWORD control_state, HKL layout, INPUT_RECORD *record);
/* Temporary focus loss releases keys but retains source identity. Permanent
 * retirement also clears identity after every recorded break is delivered.
 * The input owner drains releases before accepting the next source/event. */
void frontend_window_keyboard_release_begin(frontend_window_keyboard_state *state, BOOL retiring);
BOOL frontend_window_keyboard_release_next(frontend_window_keyboard_state *state, INPUT_RECORD *record);

/* Console records need Windows' layout translation even while DOS owns I/O:
 * unread records can be returned to a native cooked reader. Use
 * exclusively on the frontend input-owner thread (not the Window message
 * thread). Reset on backend/source retirement before reusing that thread. */
typedef struct frontend_native_keyboard {
    HKL layout;
    DWORD thread;
} frontend_native_keyboard;
#define FRONTEND_NATIVE_KEY_RECORDS 16
DWORD frontend_native_keyboard_reset(frontend_native_keyboard *);
DWORD frontend_native_keyboard_records(frontend_native_keyboard *,
    const kvm_input_event *,const INPUT_RECORD *,HKL,
    INPUT_RECORD [FRONTEND_NATIVE_KEY_RECORDS],DWORD *);

typedef DWORD (*frontend_keyboard_sink)(void *,const INPUT_RECORD *,DWORD);
typedef struct frontend_keyboard_delivery {
    frontend_window_keyboard_state physical;
    frontend_native_keyboard native;
} frontend_keyboard_delivery;
/* Caller serializes dispatch and does not change backend until retirement.
 * A failed sink is terminal: translation state must not be replayed. */
DWORD frontend_keyboard_dispatch(frontend_keyboard_delivery *,
    const frontend_window_input *,frontend_keyboard_sink,void *);

#endif
