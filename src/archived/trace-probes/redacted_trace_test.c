#include "redacted_trace.h"

#include <stdio.h>
#include <string.h>

static int require(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "redacted-trace-test: %s\n", message);
        return 0;
    }
    return 1;
}

int main(void)
{
    ntdos64_redacted_trace trace;
    ntdos64_redacted_trace_event event;
    const ntdos64_redacted_trace_event *recorded;
    uint32_t index;

    ntdos64_redacted_trace_reset(&trace);
    memset(&event, 0, sizeof(event));
    event.event_kind = NTDOS64_REDACTED_TRACE_EVENT_ADMISSION;
    event.subject_kind = NTDOS64_REDACTED_TRACE_SUBJECT_COMPONENT;
    event.outcome = NTDOS64_REDACTED_TRACE_OUTCOME_ACCEPTED;
    event.detail_code = 17u;
    event.route_class = 3u;
    event.byte_count = 27858u;
    memset(event.identity_prefix, 0xa5, sizeof(event.identity_prefix));

    if (!require(ntdos64_redacted_trace_record(&trace, &event) ==
            NTDOS64_REDACTED_TRACE_OK, "record accepted event")) {
        return 1;
    }
    memset(&event, 0, sizeof(event));
    recorded = ntdos64_redacted_trace_at(&trace, 0u);
    if (!require(recorded != NULL && recorded->sequence == 1u &&
            recorded->detail_code == 17u && recorded->byte_count == 27858u &&
            recorded->identity_prefix[0] == 0xa5,
            "event is copied and sequenced")) {
        return 1;
    }

    event.event_kind = 0u;
    event.subject_kind = NTDOS64_REDACTED_TRACE_SUBJECT_NONE;
    event.outcome = NTDOS64_REDACTED_TRACE_OUTCOME_REJECTED;
    if (!require(ntdos64_redacted_trace_record(&trace, &event) ==
            NTDOS64_REDACTED_TRACE_INVALID_ARGUMENT && trace.count == 1u,
            "invalid event changes nothing")) {
        return 1;
    }

    event.event_kind = NTDOS64_REDACTED_TRACE_EVENT_PROTOCOL;
    event.subject_kind = NTDOS64_REDACTED_TRACE_SUBJECT_RANGE;
    event.outcome = NTDOS64_REDACTED_TRACE_OUTCOME_STOPPED;
    for (index = trace.count; index < NTDOS64_REDACTED_TRACE_MAX_EVENTS; ++index) {
        if (!require(ntdos64_redacted_trace_record(&trace, &event) ==
                NTDOS64_REDACTED_TRACE_OK, "fill bounded trace")) {
            return 1;
        }
    }
    if (!require(ntdos64_redacted_trace_record(&trace, &event) ==
            NTDOS64_REDACTED_TRACE_FULL &&
            trace.count == NTDOS64_REDACTED_TRACE_MAX_EVENTS,
            "full trace rejects atomically")) {
        return 1;
    }
    if (!require(ntdos64_redacted_trace_at(&trace,
            NTDOS64_REDACTED_TRACE_MAX_EVENTS) == NULL,
            "out-of-range snapshot rejected")) {
        return 1;
    }

    ntdos64_redacted_trace_reset(&trace);
    if (!require(trace.count == 0u &&
            ntdos64_redacted_trace_at(&trace, 0u) == NULL,
            "reset removes diagnostic state")) {
        return 1;
    }

    puts("redacted-trace-test: copied-value redaction envelope verified");
    return 0;
}
