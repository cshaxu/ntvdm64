#include "bochs.h"
#include "bx-core/memory/memory.h"
#include "bx_ntvdm_extended_memory_v1.h"

#define BX_NTVDM_EXTMEM_BASE BX_CONST64(0x100000)
#define BX_NTVDM_EXTMEM_SLOTS 32u
#define BX_NTVDM_EXTMEM_COPY_CHUNK 4096u

struct bx_ntvdm_extmem_slot {
  bx_bool used;
  Bit64u address;
  Bit64u bytes;
};

static bx_bool bx_ntvdm_extmem_lifecycle_active = 0;
static bx_ntvdm_extmem_slot bx_ntvdm_extmem_slots[BX_NTVDM_EXTMEM_SLOTS];

static Bit64u bx_ntvdm_extmem_align_kib(Bit64u value)
{
  return (value + 1023u) & ~BX_CONST64(1023);
}

static bx_bool bx_ntvdm_extmem_handle(unsigned handle, unsigned *index)
{
  if (handle == 0 || handle > BX_NTVDM_EXTMEM_SLOTS ||
      !bx_ntvdm_extmem_slots[handle - 1].used) return 0;
  *index = handle - 1;
  return 1;
}

static bx_bool bx_ntvdm_extmem_range(Bit64u address, Bit64u bytes,
  Bit64u end)
{
  return bytes != 0 && address >= BX_NTVDM_EXTMEM_BASE &&
    address <= end && bytes <= end - address;
}

static bx_bool bx_ntvdm_extmem_find_free(Bit64u bytes, Bit64u end,
  Bit64u *address)
{
  Bit64u candidate = BX_NTVDM_EXTMEM_BASE;
  unsigned i;
  bx_bool moved;

  if (!bx_ntvdm_extmem_range(candidate, bytes, end)) return 0;
  do {
    moved = 0;
    for (i = 0; i < BX_NTVDM_EXTMEM_SLOTS; ++i) {
      const bx_ntvdm_extmem_slot *slot = &bx_ntvdm_extmem_slots[i];
      if (slot->used && candidate < slot->address + slot->bytes &&
          candidate + bytes > slot->address) {
        candidate = bx_ntvdm_extmem_align_kib(slot->address + slot->bytes);
        moved = 1;
      }
    }
  } while (moved && bx_ntvdm_extmem_range(candidate, bytes, end));

  if (!bx_ntvdm_extmem_range(candidate, bytes, end)) return 0;
  *address = candidate;
  return 1;
}

static void bx_ntvdm_extmem_query_free(Bit64u end, Bit64u *total,
  Bit64u *largest)
{
  Bit64u candidate = BX_NTVDM_EXTMEM_BASE;
  Bit64u next;
  unsigned i;

  *total = 0;
  *largest = 0;
  while (candidate < end) {
    next = end;
    for (i = 0; i < BX_NTVDM_EXTMEM_SLOTS; ++i) {
      const bx_ntvdm_extmem_slot *slot = &bx_ntvdm_extmem_slots[i];
      if (slot->used && slot->address >= candidate && slot->address < next)
        next = slot->address;
    }
    if (next > candidate) {
      Bit64u gap = next - candidate;
      *total += gap;
      if (gap > *largest) *largest = gap;
    }
    if (next == end) break;
    for (i = 0; i < BX_NTVDM_EXTMEM_SLOTS; ++i) {
      const bx_ntvdm_extmem_slot *slot = &bx_ntvdm_extmem_slots[i];
      if (slot->used && slot->address == next) {
        candidate = bx_ntvdm_extmem_align_kib(slot->address + slot->bytes);
        break;
      }
    }
  }
}

static bx_bool bx_ntvdm_extmem_copy(Bit64u source, Bit64u destination,
  Bit64u bytes)
{
  Bit8u copied[BX_NTVDM_EXTMEM_COPY_CHUNK];
  Bit64u offset;
  Bit64u chunk;

  if (!bx_mem.backing_ram_readable(source, bytes) ||
      !bx_mem.backing_ram_writable(destination, bytes)) return 0;
  if (destination > source && destination < source + bytes) {
    offset = bytes;
    while (offset != 0) {
      chunk = offset > BX_NTVDM_EXTMEM_COPY_CHUNK ?
        BX_NTVDM_EXTMEM_COPY_CHUNK : offset;
      offset -= chunk;
      if (!bx_mem.copy_from_backing_ram(source + offset, chunk, copied) ||
          !bx_mem.copy_to_backing_ram(destination + offset, chunk, copied))
        return 0;
    }
  } else {
    offset = 0;
    while (offset != bytes) {
      chunk = bytes - offset > BX_NTVDM_EXTMEM_COPY_CHUNK ?
        BX_NTVDM_EXTMEM_COPY_CHUNK : bytes - offset;
      if (!bx_mem.copy_from_backing_ram(source + offset, chunk, copied) ||
          !bx_mem.copy_to_backing_ram(destination + offset, chunk, copied))
        return 0;
      offset += chunk;
    }
  }
  return 1;
}

void bx_ntvdm_extended_memory_v1_set_lifecycle_active(uint32_t active)
{
  bx_ntvdm_extmem_lifecycle_active = active == 1u;
  if (!bx_ntvdm_extmem_lifecycle_active)
    memset(bx_ntvdm_extmem_slots, 0, sizeof(bx_ntvdm_extmem_slots));
}

void bx_ntvdm_extended_memory_v1_dispatch(
  const struct bx_ntvdm_extended_memory_request_v1 *request,
  struct bx_ntvdm_extended_memory_result_v1 *result)
{
  Bit64u end;
  Bit64u bytes;
  Bit64u address;
  Bit64u source;
  Bit64u destination;
  unsigned index;
  unsigned source_index;
  unsigned destination_index;
  unsigned i;

  if (result == 0) return;
  memset(result, 0, sizeof(*result));
  result->status = BX_NTVDM_EXTMEM_REJECTED_LIFECYCLE;
  if (!bx_ntvdm_extmem_lifecycle_active) return;

  end = bx_mem.get_memory_len();
  result->kib = end > BX_NTVDM_EXTMEM_BASE ?
    (uint32_t)((end - BX_NTVDM_EXTMEM_BASE) / 1024u) : 0u;
  if (request == 0) {
    result->status = BX_NTVDM_EXTMEM_REJECTED_REQUEST;
    return;
  }
  if (request->version != BX_NTVDM_EXTENDED_MEMORY_V1_VERSION) {
    result->status = BX_NTVDM_EXTMEM_REJECTED_VERSION;
    return;
  }
  if (request->operation == BX_NTVDM_EXTMEM_QUERY) {
    result->status = BX_NTVDM_EXTMEM_OK;
    return;
  }
  if (request->operation == BX_NTVDM_EXTMEM_QUERY_FREE) {
    Bit64u total;
    Bit64u largest;
    bx_ntvdm_extmem_query_free(end, &total, &largest);
    result->free_kib = (uint32_t)(total / 1024u);
    result->largest_free_kib = (uint32_t)(largest / 1024u);
    result->status = BX_NTVDM_EXTMEM_OK;
    return;
  }
  if (request->operation == BX_NTVDM_EXTMEM_MOVE_PHYSICAL) {
    if (request->byte_count == 0 ||
        !bx_ntvdm_extmem_copy(request->source_address,
          request->destination_address, request->byte_count)) {
      result->status = BX_NTVDM_EXTMEM_REJECTED_RANGE;
      return;
    }
    result->status = BX_NTVDM_EXTMEM_OK;
    return;
  }
  if (request->operation == BX_NTVDM_EXTMEM_FREE) {
    if (!bx_ntvdm_extmem_handle(request->handle, &index)) {
      result->status = BX_NTVDM_EXTMEM_REJECTED_HANDLE;
      return;
    }
    memset(&bx_ntvdm_extmem_slots[index], 0,
      sizeof(bx_ntvdm_extmem_slots[index]));
    result->status = BX_NTVDM_EXTMEM_OK;
    return;
  }
  if (request->operation == BX_NTVDM_EXTMEM_ALLOCATE) {
    if (request->kib == 0) {
      result->status = BX_NTVDM_EXTMEM_REJECTED_REQUEST;
      return;
    }
    bytes = (Bit64u)request->kib * 1024u;
    if (!bx_ntvdm_extmem_find_free(bytes, end, &address) ||
        !bx_mem.backing_ram_writable(address, bytes)) {
      result->status = BX_NTVDM_EXTMEM_UNAVAILABLE;
      return;
    }
    for (i = 0; i < BX_NTVDM_EXTMEM_SLOTS; ++i) {
      if (!bx_ntvdm_extmem_slots[i].used) {
        bx_ntvdm_extmem_slots[i].used = 1;
        bx_ntvdm_extmem_slots[i].address = address;
        bx_ntvdm_extmem_slots[i].bytes = bytes;
        result->status = BX_NTVDM_EXTMEM_OK;
        result->handle = i + 1;
        result->kib = request->kib;
        result->physical_address = address;
        return;
      }
    }
    result->status = BX_NTVDM_EXTMEM_UNAVAILABLE;
    return;
  }
  if (request->operation == BX_NTVDM_EXTMEM_REALLOCATE) {
    if (!bx_ntvdm_extmem_handle(request->handle, &index)) {
      result->status = BX_NTVDM_EXTMEM_REJECTED_HANDLE;
      return;
    }
    if (request->kib == 0) {
      result->status = BX_NTVDM_EXTMEM_REJECTED_REQUEST;
      return;
    }
    bytes = (Bit64u)request->kib * 1024u;
    if (bytes <= bx_ntvdm_extmem_slots[index].bytes) {
      bx_ntvdm_extmem_slots[index].bytes = bytes;
      result->status = BX_NTVDM_EXTMEM_OK;
      result->handle = request->handle;
      result->kib = request->kib;
      result->physical_address = bx_ntvdm_extmem_slots[index].address;
      return;
    }
    if (!bx_ntvdm_extmem_find_free(bytes, end, &address) ||
        !bx_mem.backing_ram_writable(address, bytes) ||
        !bx_ntvdm_extmem_copy(bx_ntvdm_extmem_slots[index].address, address,
          bx_ntvdm_extmem_slots[index].bytes)) {
      result->status = BX_NTVDM_EXTMEM_UNAVAILABLE;
      return;
    }
    bx_ntvdm_extmem_slots[index].address = address;
    bx_ntvdm_extmem_slots[index].bytes = bytes;
    result->status = BX_NTVDM_EXTMEM_OK;
    result->handle = request->handle;
    result->kib = request->kib;
    result->physical_address = address;
    return;
  }
  if (request->operation != BX_NTVDM_EXTMEM_MOVE) {
    result->status = BX_NTVDM_EXTMEM_REJECTED_OPERATION;
    return;
  }
  if (!bx_ntvdm_extmem_handle(request->source_handle, &source_index) ||
      !bx_ntvdm_extmem_handle(request->destination_handle,
        &destination_index)) {
    result->status = BX_NTVDM_EXTMEM_REJECTED_HANDLE;
    return;
  }
  if (request->byte_count == 0 ||
      request->source_offset > bx_ntvdm_extmem_slots[source_index].bytes ||
      request->byte_count > bx_ntvdm_extmem_slots[source_index].bytes -
        request->source_offset ||
      request->destination_offset >
        bx_ntvdm_extmem_slots[destination_index].bytes ||
      request->byte_count > bx_ntvdm_extmem_slots[destination_index].bytes -
        request->destination_offset) {
    result->status = BX_NTVDM_EXTMEM_REJECTED_RANGE;
    return;
  }
  source = bx_ntvdm_extmem_slots[source_index].address +
    request->source_offset;
  destination = bx_ntvdm_extmem_slots[destination_index].address +
    request->destination_offset;
  if (!bx_ntvdm_extmem_copy(source, destination, request->byte_count)) {
    result->status = BX_NTVDM_EXTMEM_UNAVAILABLE;
    return;
  }
  result->status = BX_NTVDM_EXTMEM_OK;
}
