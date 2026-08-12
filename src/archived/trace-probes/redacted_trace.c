#include "redacted_trace.h"

#include <string.h>

static int ntdos64_redacted_trace_event_kind_is_valid(uint32_t value)
{
    return value >= NTDOS64_REDACTED_TRACE_EVENT_ADMISSION &&
        value <= NTDOS64_REDACTED_TRACE_EVENT_TERMINAL;
}

static int ntdos64_redacted_trace_subject_kind_is_valid(uint32_t value)
{
    return value >= NTDOS64_REDACTED_TRACE_SUBJECT_NONE &&
        value <= NTDOS64_REDACTED_TRACE_SUBJECT_SESSION;
}

static int ntdos64_redacted_trace_outcome_is_valid(uint32_t value)
{
    return value >= NTDOS64_REDACTED_TRACE_OUTCOME_ACCEPTED &&
        value <= NTDOS64_REDACTED_TRACE_OUTCOME_FAULTED;
}

void ntdos64_redacted_trace_reset(ntdos64_redacted_trace *trace)
{
    if (trace != NULL) {
        memset(trace, 0, sizeof(*trace));
    }
}

ntdos64_redacted_trace_status ntdos64_redacted_trace_record(
    ntdos64_redacted_trace *trace,
    const ntdos64_redacted_trace_event *event)
{
    ntdos64_redacted_trace_event copy;

    if (trace == NULL || event == NULL ||
        !ntdos64_redacted_trace_event_kind_is_valid(event->event_kind) ||
        !ntdos64_redacted_trace_subject_kind_is_valid(event->subject_kind) ||
        !ntdos64_redacted_trace_outcome_is_valid(event->outcome)) {
        return NTDOS64_REDACTED_TRACE_INVALID_ARGUMENT;
    }
    if (trace->count >= NTDOS64_REDACTED_TRACE_MAX_EVENTS) {
        return NTDOS64_REDACTED_TRACE_FULL;
    }

    copy = *event;
    copy.sequence = trace->count + 1u;
    trace->events[trace->count] = copy;
    ++trace->count;
    return NTDOS64_REDACTED_TRACE_OK;
}

const ntdos64_redacted_trace_event *ntdos64_redacted_trace_at(
    const ntdos64_redacted_trace *trace,
    uint32_t index)
{
    if (trace == NULL || index >= trace->count) {
        return NULL;
    }
    return &trace->events[index];
}
