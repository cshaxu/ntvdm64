#include "bochs.h"
#include "bx-core/memory/memory.h"
#include "bx-mantle/bx_ntvdm_extended_memory_v1.h"
#include "bx-mantle/bx_ntvdm_minimal_machine.h"

static int call(struct bx_ntvdm_extended_memory_request_v1 *request,
  struct bx_ntvdm_extended_memory_result_v1 *result)
{
  bx_ntvdm_extended_memory_v1_dispatch(request, result);
  return result->status == BX_NTVDM_EXTMEM_OK;
}

int main()
{
  struct bx_ntvdm_extended_memory_request_v1 request = {};
  struct bx_ntvdm_extended_memory_result_v1 first;
  struct bx_ntvdm_extended_memory_result_v1 second;
  struct bx_ntvdm_extended_memory_result_v1 third;
  struct bx_ntvdm_extended_memory_result_v1 rejected;
  bx_ntvdm_minimal_machine_c machine;
  Bit8u pattern[16];
  Bit8u observed[16];
  unsigned i;

  request.version = BX_NTVDM_EXTENDED_MEMORY_V1_VERSION;
  request.operation = BX_NTVDM_EXTMEM_QUERY;
  bx_ntvdm_extended_memory_v1_dispatch(&request, &first);
  if (first.status != BX_NTVDM_EXTMEM_REJECTED_LIFECYCLE) return 1;
  if (machine.initialize(0x400000, 0x400000) != BX_NTVDM_MINIMAL_MACHINE_OK)
    return 2;
  if (!call(&request, &first) || first.kib != 3072u) return 3;

  request.operation = BX_NTVDM_EXTMEM_ALLOCATE;
  request.kib = 64u;
  if (!call(&request, &first) || first.handle == 0 ||
      first.physical_address < BX_CONST64(0x100000)) return 4;
  if (!call(&request, &second) || second.handle == first.handle ||
      second.physical_address < first.physical_address + BX_CONST64(0x10000))
    return 5;

  request.operation = BX_NTVDM_EXTMEM_QUERY_FREE;
  if (!call(&request, &rejected) || rejected.free_kib != 2944u ||
      rejected.largest_free_kib != 2944u) return 20;

  for (i = 0; i < sizeof(pattern); ++i) pattern[i] = (Bit8u)i;
  if (!bx_mem.copy_to_ordinary_ram(0x2000u, sizeof(pattern), pattern)) return 21;
  request.operation = BX_NTVDM_EXTMEM_MOVE_PHYSICAL;
  request.source_address = 0x2000u;
  request.destination_address = 0x3000u;
  request.byte_count = sizeof(pattern);
  if (!call(&request, &rejected) ||
      !bx_mem.copy_from_ordinary_ram(0x3000u, sizeof(observed), observed) ||
      memcmp(pattern, observed, sizeof(pattern)) != 0) return 22;
  if (!bx_mem.copy_to_ordinary_ram(first.physical_address, sizeof(pattern),
      pattern)) return 6;
  request.operation = BX_NTVDM_EXTMEM_MOVE;
  request.source_handle = first.handle;
  request.destination_handle = second.handle;
  request.source_offset = 0;
  request.destination_offset = 0;
  request.byte_count = sizeof(pattern);
  if (!call(&request, &rejected) ||
      !bx_mem.copy_from_ordinary_ram(second.physical_address,
        sizeof(observed), observed) || memcmp(pattern, observed,
        sizeof(pattern)) != 0) return 7;

  request.operation = BX_NTVDM_EXTMEM_REALLOCATE;
  request.handle = first.handle;
  request.kib = 128u;
  if (!call(&request, &third) || third.handle != first.handle ||
      third.physical_address == first.physical_address ||
      !bx_mem.copy_from_ordinary_ram(third.physical_address,
        sizeof(observed), observed) || memcmp(pattern, observed,
        sizeof(pattern)) != 0) return 8;
  first = third;
  request.kib = 4096u;
  bx_ntvdm_extended_memory_v1_dispatch(&request, &rejected);
  if (rejected.status != BX_NTVDM_EXTMEM_UNAVAILABLE) return 9;
  request.operation = BX_NTVDM_EXTMEM_MOVE;
  request.source_handle = first.handle;
  request.destination_handle = second.handle;
  request.source_offset = 0;
  request.destination_offset = 16;
  request.byte_count = sizeof(pattern);
  if (!call(&request, &rejected) ||
      !bx_mem.copy_from_ordinary_ram(second.physical_address + 16,
        sizeof(observed), observed) || memcmp(pattern, observed,
        sizeof(pattern)) != 0) return 10;

  request.operation = BX_NTVDM_EXTMEM_ALLOCATE;
  request.kib = 64u;
  if (!call(&request, &third)) return 11;
  if (!bx_mem.copy_to_ordinary_ram(third.physical_address, sizeof(pattern),
      pattern)) return 12;
  request.operation = BX_NTVDM_EXTMEM_MOVE;
  request.source_handle = third.handle;
  request.destination_handle = third.handle;
  request.source_offset = 0;
  request.destination_offset = 4;
  request.byte_count = 12;
  if (!call(&request, &rejected) ||
      !bx_mem.copy_from_ordinary_ram(third.physical_address + 4, 12,
        observed)) return 13;
  for (i = 0; i < 12; ++i) if (observed[i] != (Bit8u)i) return 14;

  request.destination_offset = 65530;
  request.byte_count = 16;
  bx_ntvdm_extended_memory_v1_dispatch(&request, &rejected);
  if (rejected.status != BX_NTVDM_EXTMEM_REJECTED_RANGE) return 15;
  request.operation = BX_NTVDM_EXTMEM_FREE;
  request.handle = second.handle;
  bx_ntvdm_extended_memory_v1_dispatch(&request, &rejected);
  if (rejected.status != BX_NTVDM_EXTMEM_OK) return 16;
  bx_ntvdm_extended_memory_v1_dispatch(&request, &rejected);
  if (rejected.status != BX_NTVDM_EXTMEM_REJECTED_HANDLE) return 17;
  if (machine.cleanup() != BX_NTVDM_MINIMAL_MACHINE_OK) return 18;
  request.operation = BX_NTVDM_EXTMEM_QUERY;
  bx_ntvdm_extended_memory_v1_dispatch(&request, &rejected);
  return rejected.status == BX_NTVDM_EXTMEM_REJECTED_LIFECYCLE ? 0 : 19;
}
