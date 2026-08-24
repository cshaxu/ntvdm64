#include "ntio_preentry.h"

#include <string.h>

#define RUNTIME_NTIO_PREENTRY_V1_BYTES 0x8400u
#define RUNTIME_NTIO_PREENTRY_V1_PHYSICAL 0x700u
#define RUNTIME_NTIO_PREENTRY_V1_PRESERVE 0x714u
#define RUNTIME_NTIO_PREENTRY_V1_PRESERVE_BYTES 4u
#define RUNTIME_NTIO_PREENTRY_V1_CS 0x0070u

int runtime_ntio_preentry_v1_prepare(const byob_image *ntio,
    const byob_component_descriptor *identity,
    const struct runtime_preentry_input_v1 *preentry_input,
    runtime_finite_run_request *request, Bit64u tick_budget, Bit32u ips)
{
  if (ntio == 0 || identity == 0 || request == 0 || ntio->bytes == 0 ||
      ntio->byte_count != RUNTIME_NTIO_PREENTRY_V1_BYTES ||
      identity->bytes != RUNTIME_NTIO_PREENTRY_V1_BYTES ||
      ntio->byte_count > RUNTIME_FINITE_RUN_MAX_ENTRY_BYTES ||
      tick_budget == 0 || ips == 0) return 0;

  memset(request, 0, sizeof(*request));
  request->request_version = RUNTIME_FINITE_RUN_REQUEST_VERSION;
  memcpy(request->entry_bytes, ntio->bytes, ntio->byte_count);
  request->entry_byte_count = (Bit32u) ntio->byte_count;
  request->entry_physical_address = RUNTIME_NTIO_PREENTRY_V1_PHYSICAL;
  request->entry_cs = RUNTIME_NTIO_PREENTRY_V1_CS;
  request->entry_eip = 0;
  request->instruction_tick_budget = tick_budget;
  request->ips = ips;
  request->preserve_physical_address = RUNTIME_NTIO_PREENTRY_V1_PRESERVE;
  request->preserve_byte_count = RUNTIME_NTIO_PREENTRY_V1_PRESERVE_BYTES;
  if (!runtime_preentry_input_v1_make_write_action(preentry_input,
      &request->preentry_action)) return 0;
  request->has_preentry_action = 1;
  return 1;
}
