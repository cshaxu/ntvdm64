#include "adapter-mvdm-host-in/bop_ingress.h"
#include "adapter-mvdm-host-in/generic_ud_bridge.h"

#include <string.h>

static unsigned requested_disposition;

static int route(const struct runtime_generic_ud_event *event,
  struct runtime_generic_ud_outcome *outcome, void *context)
{
  (void)context;
  if (event == 0 || outcome == 0 || event->vector != 6u) return 0;
  memset(outcome, 0, sizeof(*outcome));
  outcome->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
  outcome->disposition = requested_disposition;
  return 1;
}

static void event_initialize(struct runtime_generic_ud_event *event)
{
  memset(event, 0, sizeof(*event));
  event->magic = RUNTIME_GENERIC_UD_EVENT_MAGIC;
  event->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
  event->struct_bytes = sizeof(*event);
  event->vector = 6u;
  event->window_bytes = 2u;
  event->window[0] = 0x0fu;
  event->window[1] = 0x0bu;
}

int main(void)
{
  struct runtime_generic_ud_event event;
  struct runtime_generic_ud_outcome outcome;
  event_initialize(&event);
  memset(&outcome, 0, sizeof(outcome));

  if (runtime_bop_ingress_opaque_callback(0, &event, sizeof(event) - 1u,
      &outcome, sizeof(outcome)) != 0 ||
      runtime_bop_ingress_opaque_callback(0, &event, sizeof(event),
      &outcome, sizeof(outcome) - 1u) != 0 ||
      runtime_bop_ingress_opaque_callback(0, &event, sizeof(event),
      &outcome, sizeof(outcome)) != 0) return 1;

  if (!runtime_bop_ingress_bind(route, 0)) return 2;
  requested_disposition = RUNTIME_GENERIC_UD_RESUME;
  if (!runtime_bop_ingress_opaque_callback(0, &event, sizeof(event),
      &outcome, sizeof(outcome)) ||
      outcome.disposition != RUNTIME_GENERIC_UD_RESUME) return 3;
  runtime_machine_generic_ud_pending_observation_reset();
  requested_disposition = RUNTIME_GENERIC_UD_PENDING;
  if (!runtime_bop_ingress_opaque_callback(0, &event, sizeof(event),
      &outcome, sizeof(outcome)) ||
      !runtime_machine_generic_ud_pending_observed()) return 4;
  runtime_machine_generic_ud_stop_observation_reset();
  requested_disposition = RUNTIME_GENERIC_UD_STOP;
  if (!runtime_bop_ingress_opaque_callback(0, &event, sizeof(event),
      &outcome, sizeof(outcome)) ||
      !runtime_machine_generic_ud_stop_observed()) return 5;
  runtime_bop_ingress_unbind();
  return 0;
}
