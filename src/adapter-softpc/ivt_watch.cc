#include "ivt_watch.h"
#include "machine_stage.h"
#include "machine_binding.h"

#include <string.h>

struct runtime_ivt_watch_v1_state {
  uint32_t active;
  uint32_t vector;
  uint16_t offset;
  uint16_t segment;
};

static runtime_ivt_watch_v1_state runtime_ivt_watch_v1_state;

static int runtime_ivt_watch_v1_vector_valid(uint32_t vector)
{
  return vector <= 0xffu;
}

extern "C" int runtime_ivt_watch_v1_read_current(uint32_t vector,
  uint16_t *offset, uint16_t *segment)
{
  uint8_t bytes[4];
  uint64_t address;
  if (!runtime_machine_stage_v1_active() || !runtime_ivt_watch_v1_vector_valid(vector) ||
      offset == 0 || segment == 0) return 0;
  address = (uint64_t)vector * 4u;
  if (!runtime_machine_binding_v1_memory_readable(address, sizeof(bytes)) ||
      !runtime_machine_binding_v1_memory_read(address, sizeof(bytes), bytes)) return 0;
  *offset = (uint16_t)((uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8));
  *segment = (uint16_t)((uint16_t)bytes[2] | ((uint16_t)bytes[3] << 8));
  return 1;
}

extern "C" int runtime_ivt_watch_v1_initialize(uint32_t vector)
{
  uint16_t offset, segment;
  if (runtime_ivt_watch_v1_state.active ||
      !runtime_ivt_watch_v1_read_current(vector, &offset, &segment)) return 0;
  runtime_ivt_watch_v1_state.active = 1u;
  runtime_ivt_watch_v1_state.vector = vector;
  runtime_ivt_watch_v1_state.offset = offset;
  runtime_ivt_watch_v1_state.segment = segment;
  return 1;
}

extern "C" int runtime_ivt_watch_v1_copy_expected(uint32_t vector,
  uint16_t *offset, uint16_t *segment)
{
  if (!runtime_machine_stage_v1_active() || offset == 0 || segment == 0 ||
      !runtime_ivt_watch_v1_state.active ||
      runtime_ivt_watch_v1_state.vector != vector) return 0;
  *offset = runtime_ivt_watch_v1_state.offset;
  *segment = runtime_ivt_watch_v1_state.segment;
  return 1;
}

extern "C" int runtime_ivt_watch_v1_store_expected(uint32_t vector,
  uint16_t offset, uint16_t segment)
{
  if (!runtime_machine_stage_v1_active() || !runtime_ivt_watch_v1_state.active ||
      runtime_ivt_watch_v1_state.vector != vector) return 0;
  runtime_ivt_watch_v1_state.offset = offset;
  runtime_ivt_watch_v1_state.segment = segment;
  return 1;
}

extern "C" void runtime_ivt_watch_v1_reset(void)
{
  memset(&runtime_ivt_watch_v1_state, 0, sizeof(runtime_ivt_watch_v1_state));
}
