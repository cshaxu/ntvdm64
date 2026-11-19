#ifndef MVDM_KEYBOARD_HISTORY_H
#define MVDM_KEYBOARD_HISTORY_H

#include <stdint.h>

/* Metadata only: the original nt_event.c ring still owns KEY_EVENT_RECORDs.
 * Original keyboard locking must cover both the device and these tags. */
#define MVDM_KEY_HISTORY_CAPACITY 100u
#define MVDM_KEY_DEVICE_CAPACITY 512u
#define MVDM_KEY_HELD_CAPACITY 16u
#define MVDM_KEY_RECEIPT_BATCH_CAPACITY 40u
typedef uint64_t mvdm_key_origin;
typedef struct mvdm_keyboard_history {
    mvdm_key_origin issued,dispatched,active;
    mvdm_key_origin device[MVDM_KEY_DEVICE_CAPACITY];
    mvdm_key_origin held[MVDM_KEY_HELD_CAPACITY];
    mvdm_key_origin translated,prefix,output;
    uint64_t receipts[MVDM_KEY_HISTORY_CAPACITY];
    uint64_t receipt_batch[MVDM_KEY_RECEIPT_BATCH_CAPACITY];
    unsigned receipt_batch_count,receipt_batch_next;
    unsigned char selected[MVDM_KEY_HISTORY_CAPACITY];
} mvdm_keyboard_history;

/* Zero is reserved for data not originating in a Console key record.
 * Overflow/refused dispatch leaves state unchanged. */
int mvdm_keyboard_history_record(mvdm_keyboard_history *state);
int mvdm_keyboard_history_record_receipt(mvdm_keyboard_history *state,uint64_t receipt);
/* Stage only the KEY_EVENT receipts from the just-read normalized batch.
 * The original update_key_history consumes them in the same order. */
int mvdm_keyboard_history_stage_receipts(mvdm_keyboard_history *state,const uint64_t *receipts,unsigned count);
int mvdm_keyboard_history_begin(mvdm_keyboard_history *state);
void mvdm_keyboard_history_end(mvdm_keyboard_history *state);
void mvdm_keyboard_history_select_begin(mvdm_keyboard_history *state);
void mvdm_keyboard_history_select(mvdm_keyboard_history *state,mvdm_key_origin origin);
unsigned mvdm_keyboard_history_selected(const mvdm_keyboard_history *state);
/* Newest-first ordinal -> original GetHistoryKeyEvent's newest-first age.
 * Zero means unavailable; an overwritten history slot is never aliased. */
unsigned mvdm_keyboard_history_age(const mvdm_keyboard_history *state,unsigned ordinal);
uint64_t mvdm_keyboard_history_receipt(const mvdm_keyboard_history *state,unsigned ordinal);
/* All receipts that the original bounded history can still return. */
unsigned mvdm_keyboard_history_live(const mvdm_keyboard_history *state,uint64_t *receipts);
/* Snapshot before original hardware reset; selected ages survive that reset. */
unsigned mvdm_keyboard_history_pending(mvdm_keyboard_history *state,
    unsigned first,unsigned next,unsigned capacity,unsigned held,
    int output,int prefix);
void mvdm_keyboard_history_clear_ring(mvdm_keyboard_history *state);

#endif
