/*
 * Native proof for the generic pre-entry preserve/publication ordering.
 * The opaque entry image overwrites physical 0x714, while the guest reads
 * that physical word after entry.  The finite runner must have restored the
 * reset-time bytes (zero in this minimal machine) before CS:IP is applied.
 */

#include "bochs.h"
#include "adapter-mvdm-host-out/softpc/finite_run.h"

#include <string.h>

int main()
{
  static Bit8u image[0x20];
  static const Bit8u code[] = {
    0x31, 0xc0,                   /* xor ax,ax */
    0x8e, 0xd8,                   /* mov ds,ax */
    0xa1, 0x14, 0x07,             /* mov ax,[0714] */
    0x0b, 0xc0,                   /* or ax,ax */
    0x75, 0x01,                   /* jnz incorrect */
    0xf4,                         /* hlt: preserved bytes were restored */
    0x0f, 0x0b                    /* ud2: preservation failed */
  };
  static runtime_finite_run_request request;
  int status;

  memset(image, 0, sizeof(image));
  memcpy(image, code, sizeof(code));
  image[0x14] = 0xa5;
  image[0x15] = 0x5a;
  image[0x16] = 0x3c;
  image[0x17] = 0xc3;

  request.request_version = RUNTIME_FINITE_RUN_REQUEST_VERSION;
  memcpy(request.entry_bytes, image, sizeof(image));
  request.entry_byte_count = sizeof(image);
  request.entry_physical_address = 0x700;
  request.entry_cs = 0x70;
  request.entry_eip = 0;
  request.instruction_tick_budget = 64;
  request.ips = 1000000;
  request.preserve_physical_address = 0x714;
  request.preserve_byte_count = 4;
  status = (int) runtime_run_finite_bare_bytes(&request);
  if (status != RUNTIME_FINITE_RUN_COMPLETED_BUDGET) return status + 1;

  /* Validation occurs before the machine is initialized or any RAM is
   * copied.  This range cannot fit in the fixed ordinary-RAM aperture. */
  request.entry_physical_address = 0xfffff;
  request.entry_byte_count = 2;
  status = (int) runtime_run_finite_bare_bytes(&request);
  return status == RUNTIME_FINITE_RUN_REJECTED_INPUT ? 0 : status + 16;
}
