#ifndef NTDOS64_REDACTED_TRACE_H
#define NTDOS64_REDACTED_TRACE_H

#include <stddef.h>
#include <stdint.h>

/*
 * This is a copied-value diagnostic envelope.  Its public shape deliberately
 * has no text, guest address, host handle, pointer, or guest-byte field.
 */

#define NTDOS64_REDACTED_TRACE_IDENTITY_PREFIX_SIZE 8u
#define NTDOS64_REDACTED_TRACE_MAX_EVENTS 64u

typedef enum ntdos64_redacted_trace_event_kind {
    NTDOS64_REDACTED_TRACE_EVENT_ADMISSION = 1,
    NTDOS64_REDACTED_TRACE_EVENT_PUBLICATION = 2,
    NTDOS64_REDACTED_TRACE_EVENT_DECODE = 3,
    NTDOS64_REDACTED_TRACE_EVENT_PROTOCOL = 4,
    NTDOS64_REDACTED_TRACE_EVENT_TERMINAL = 5
} ntdos64_redacted_trace_event_kind;

typedef enum ntdos64_redacted_trace_subject_kind {
    NTDOS64_REDACTED_TRACE_SUBJECT_NONE = 0,
    NTDOS64_REDACTED_TRACE_SUBJECT_COMPONENT = 1,
    NTDOS64_REDACTED_TRACE_SUBJECT_RANGE = 2,
    NTDOS64_REDACTED_TRACE_SUBJECT_COMMAND = 3,
    NTDOS64_REDACTED_TRACE_SUBJECT_SESSION = 4
} ntdos64_redacted_trace_subject_kind;

typedef enum ntdos64_redacted_trace_outcome {
    NTDOS64_REDACTED_TRACE_OUTCOME_ACCEPTED = 1,
    NTDOS64_REDACTED_TRACE_OUTCOME_REJECTED = 2,
    NTDOS64_REDACTED_TRACE_OUTCOME_STOPPED = 3,
    NTDOS64_REDACTED_TRACE_OUTCOME_FAULTED = 4
} ntdos64_redacted_trace_outcome;

typedef enum ntdos64_redacted_trace_status {
    NTDOS64_REDACTED_TRACE_OK = 0,
    NTDOS64_REDACTED_TRACE_INVALID_ARGUMENT = 1,
    NTDOS64_REDACTED_TRACE_FULL = 2
} ntdos64_redacted_trace_status;

typedef struct ntdos64_redacted_trace_event {
    uint32_t sequence;
    uint32_t event_kind;
    uint32_t subject_kind;
    uint32_t outcome;
    uint32_t detail_code;
    uint32_t route_class;
    uint64_t byte_count;
    uint8_t identity_prefix[NTDOS64_REDACTED_TRACE_IDENTITY_PREFIX_SIZE];
} ntdos64_redacted_trace_event;

typedef struct ntdos64_redacted_trace {
    uint32_t count;
    ntdos64_redacted_trace_event events[NTDOS64_REDACTED_TRACE_MAX_EVENTS];
} ntdos64_redacted_trace;

void ntdos64_redacted_trace_reset(ntdos64_redacted_trace *trace);

ntdos64_redacted_trace_status ntdos64_redacted_trace_record(
    ntdos64_redacted_trace *trace,
    const ntdos64_redacted_trace_event *event);

const ntdos64_redacted_trace_event *ntdos64_redacted_trace_at(
    const ntdos64_redacted_trace *trace,
    uint32_t index);

#endif
