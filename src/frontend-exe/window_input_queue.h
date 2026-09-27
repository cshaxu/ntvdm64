#ifndef FRONTEND_WINDOW_INPUT_QUEUE_H
#define FRONTEND_WINDOW_INPUT_QUEUE_H

#include <windows.h>
#include "lib/kvm-base/event_interface.h"

#define FRONTEND_WINDOW_INPUT_CAPACITY 256u

typedef struct frontend_window_input {
    kvm_input_event event;
    DWORD control_state;
    HKL keyboard_layout;
    DWORD ui_thread_id;
} frontend_window_input;

/* This FIFO is the sole copied Window-input channel to the frontend I/O owner
 * thread. It is not the original COMMAND prepend queue. No guest execution,
 * coordinate conversion or keyboard policy belongs in this transport. */
typedef struct frontend_window_input_queue {
    SRWLOCK lock;
    HANDLE ready;
    frontend_window_input events[FRONTEND_WINDOW_INPUT_CAPACITY];
    unsigned head, count;
    BOOL closed;
} frontend_window_input_queue;

BOOL frontend_window_input_queue_create(frontend_window_input_queue *queue);
BOOL frontend_window_input_queue_push(frontend_window_input_queue *queue,
    const kvm_input_event *event);
BOOL frontend_window_input_queue_pop(frontend_window_input_queue *queue,
    frontend_window_input *event);
/* Close wakes the consumer, rejects producers, and retains queued releases.
 * Destroy is valid only after UI/controller producers and input consumer join. */
void frontend_window_input_queue_close(frontend_window_input_queue *queue);
void frontend_window_input_queue_destroy(frontend_window_input_queue *queue);

#endif
