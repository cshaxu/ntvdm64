/////////////////////////////////////////////////////////////////////////
// DIVERGENCE(BX-MEM-024): private SIM-free allocation body. The adopted
// memory header retains only the original-shaped member declaration.
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "memory/memory.h"
#include "iodev/iodev.h"
#define LOG_THIS BX_MEM(0)->

#define BX_MEM_VECTOR_ALIGN 4096
#define BX_MEM_HANDLERS ((BX_CONST64(1) << BX_PHY_ADDRESS_WIDTH) >> 20)

bx_bool BX_MEM_C::init_memory_without_sim(Bit64u guest, Bit64u host)
{
  unsigned idx;
  if ((host & 0xfffff) != 0 || (guest & 0xfffff) != 0) return 0;
  if (BX_MEM_THIS actual_vector != NULL || BX_MEM_THIS vector != NULL ||
      BX_MEM_THIS blocks != NULL || BX_MEM_THIS memory_handlers != NULL) return 0;
  BX_MEM_THIS vector = alloc_vector_aligned(host + BIOSROMSZ + EXROMSIZE + 4096,
                                             BX_MEM_VECTOR_ALIGN);
  BX_MEM_THIS len = guest;
  BX_MEM_THIS allocated = host;
  BX_MEM_THIS rom = &BX_MEM_THIS vector[host];
  BX_MEM_THIS bogus = &BX_MEM_THIS vector[host + BIOSROMSZ + EXROMSIZE];
  memset(BX_MEM_THIS rom, 0xff, BIOSROMSZ + EXROMSIZE + 4096);
  BX_ASSERT((BX_MEM_THIS len / BX_MEM_BLOCK_LEN) <= 0xffffffff);
  Bit32u num_blocks = (Bit32u)(BX_MEM_THIS len / BX_MEM_BLOCK_LEN);
  BX_MEM_THIS blocks = new Bit8u* [num_blocks];
  for (idx = 0; idx < num_blocks; idx++) BX_MEM_THIS blocks[idx] = NULL;
  BX_MEM_THIS used_blocks = 0;
  BX_MEM_THIS memory_handlers = new struct memory_handler_struct *[BX_MEM_HANDLERS];
  for (idx = 0; idx < BX_MEM_HANDLERS; idx++) BX_MEM_THIS memory_handlers[idx] = NULL;
  BX_MEM_THIS pci_enabled = 0;
  BX_MEM_THIS smram_available = 0;
  BX_MEM_THIS smram_enable = 0;
  BX_MEM_THIS smram_restricted = 0;
  return 1;
}
