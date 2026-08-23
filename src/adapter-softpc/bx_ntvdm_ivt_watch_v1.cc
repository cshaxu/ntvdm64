#include "bochs.h"
#include "bx-core/memory/memory.h"
#include "bx_ntvdm_ivt_watch_v1.h"
#include "bx_ntvdm_machine_stage_v1.h"

#include <string.h>

struct bx_ntvdm_ivt_watch_v1_state {
  uint32_t active;
  uint32_t vector;
  uint16_t offset;
  uint16_t segment;
};

static bx_ntvdm_ivt_watch_v1_state bx_ntvdm_ivt_watch_v1_state;

static int bx_ntvdm_ivt_watch_v1_vector_valid(uint32_t vector)
{
  return vector <= 0xffu;
}

extern "C" int bx_ntvdm_ivt_watch_v1_read_current(uint32_t vector,
  uint16_t *offset, uint16_t *segment)
{
  uint8_t bytes[4];
  uint64_t address;
  if (!bx_ntvdm_machine_stage_v1_active() || !bx_ntvdm_ivt_watch_v1_vector_valid(vector) ||
      offset == 0 || segment == 0) return 0;
  address = (uint64_t)vector * 4u;
  if (!bx_mem.ordinary_ram_readable(address, sizeof(bytes)) ||
      !bx_mem.copy_from_ordinary_ram(address, sizeof(bytes), bytes)) return 0;
  *offset = (uint16_t)((uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8));
  *segment = (uint16_t)((uint16_t)bytes[2] | ((uint16_t)bytes[3] << 8));
  return 1;
}

extern "C" int bx_ntvdm_ivt_watch_v1_initialize(uint32_t vector)
{
  uint16_t offset, segment;
  if (bx_ntvdm_ivt_watch_v1_state.active ||
      !bx_ntvdm_ivt_watch_v1_read_current(vector, &offset, &segment)) return 0;
  bx_ntvdm_ivt_watch_v1_state.active = 1u;
  bx_ntvdm_ivt_watch_v1_state.vector = vector;
  bx_ntvdm_ivt_watch_v1_state.offset = offset;
  bx_ntvdm_ivt_watch_v1_state.segment = segment;
  return 1;
}

extern "C" int bx_ntvdm_ivt_watch_v1_copy_expected(uint32_t vector,
  uint16_t *offset, uint16_t *segment)
{
  if (!bx_ntvdm_machine_stage_v1_active() || offset == 0 || segment == 0 ||
      !bx_ntvdm_ivt_watch_v1_state.active ||
      bx_ntvdm_ivt_watch_v1_state.vector != vector) return 0;
  *offset = bx_ntvdm_ivt_watch_v1_state.offset;
  *segment = bx_ntvdm_ivt_watch_v1_state.segment;
  return 1;
}

extern "C" int bx_ntvdm_ivt_watch_v1_store_expected(uint32_t vector,
  uint16_t offset, uint16_t segment)
{
  if (!bx_ntvdm_machine_stage_v1_active() || !bx_ntvdm_ivt_watch_v1_state.active ||
      bx_ntvdm_ivt_watch_v1_state.vector != vector) return 0;
  bx_ntvdm_ivt_watch_v1_state.offset = offset;
  bx_ntvdm_ivt_watch_v1_state.segment = segment;
  return 1;
}

extern "C" void bx_ntvdm_ivt_watch_v1_reset(void)
{
  memset(&bx_ntvdm_ivt_watch_v1_state, 0, sizeof(bx_ntvdm_ivt_watch_v1_state));
}
