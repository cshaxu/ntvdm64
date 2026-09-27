#ifndef MVDM_KEYBOARD_HISTORY_H
#define MVDM_KEYBOARD_HISTORY_H

#include <stdint.h>

/* Metadata only: the original nt_event.c ring still owns KEY_EVENT_RECORDs.
 * Original keyboard locking must cover both the device and these tags. */
#define MVDM_KEY_HISTORY_CAPACITY 100u
#define MVDM_KEY_DEVICE_CAPACITY 512u
#define MVDM_KEY_HELD_CAPACITY 16u
typedef uint64_t mvdm_key_origin;
typedef struct mvdm_keyboard_history {
    mvdm_key_origin issued,dispatched,active;
    mvdm_key_origin device[MVDM_KEY_DEVICE_CAPACITY];
    mvdm_key_origin held[MVDM_KEY_HELD_CAPACITY];
    mvdm_key_origin translated,prefix,pending,output;
    unsigned char selected[MVDM_KEY_HISTORY_CAPACITY];
} mvdm_keyboard_history;

/* Zero is reserved for data not originating in a Console key record.
 * Overflow/refused dispatch leaves state unchanged. */
int mvdm_keyboard_history_record(mvdm_keyboard_history *state);
int mvdm_keyboard_history_begin(mvdm_keyboard_history *state);
void mvdm_keyboard_history_end(mvdm_keyboard_history *state);
void mvdm_keyboard_history_select_begin(mvdm_keyboard_history *state);
void mvdm_keyboard_history_select(mvdm_keyboard_history *state,mvdm_key_origin origin);
unsigned mvdm_keyboard_history_selected(const mvdm_keyboard_history *state);
/* Newest-first ordinal -> original GetHistoryKeyEvent's newest-first age.
 * Zero means unavailable; an overwritten history slot is never aliased. */
unsigned mvdm_keyboard_history_age(const mvdm_keyboard_history *state,unsigned ordinal);
/* Snapshot before original hardware reset; selected ages survive that reset. */
unsigned mvdm_keyboard_history_pending(mvdm_keyboard_history *state,
    unsigned first,unsigned next,unsigned capacity,unsigned held,
    int output,int pending,int prefix);
void mvdm_keyboard_history_clear_device(mvdm_keyboard_history *state);
void mvdm_keyboard_history_clear_ring(mvdm_keyboard_history *state);

#endif
