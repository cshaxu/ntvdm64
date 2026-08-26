/////////////////////////////////////////////////////////////////////////
// DIVERGENCE(BX-CORE-DIV-006): private checked guest-RAM mechanics.
// The adopted memory header retains only the original-shaped member
// declarations.  This body is called only through bochs-core.
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "memory/memory.h"
#include "bochs-core/cpu/cpu.h"
#define LOG_THIS BX_MEM(0)->

bx_bool BX_MEM_C::ordinary_ram_writable(bx_phy_address addr, Bit64u len)
{
  bx_phy_address current, last, page_last;
  struct memory_handler_struct *memory_handler;
  if (len == 0 || len > (Bit64u)((bx_phy_address)-1) ||
      addr > ((bx_phy_address)-1) - (bx_phy_address)len) return 0;
  last = addr + (bx_phy_address)len - 1;
  current = addr;
  while (1) {
    page_last = current | 0xfff;
    if (page_last > last) page_last = last;
    if (A20ADDR(current) != current || A20ADDR(page_last) != page_last ||
        current >= BX_MEM_THIS len) return 0;
    memory_handler = BX_MEM_THIS memory_handlers[current >> 20];
    while (memory_handler) {
      if (memory_handler->begin <= page_last && memory_handler->end >= current) return 0;
      memory_handler = memory_handler->next;
    }
#if BX_SUPPORT_MONITOR_MWAIT
    if (BX_MEM_THIS is_monitor(current & ~((bx_phy_address)0xfff), 0xfff)) return 0;
#endif
    if (BX_MEM_THIS getHostMemAddr(NULL, current, BX_WRITE) == NULL) return 0;
    if (page_last == last) break;
    current = page_last + 1;
  }
  return 1;
}

bx_bool BX_MEM_C::backing_ram_accessible(bx_phy_address addr, Bit64u len)
{
  bx_phy_address current, last, page_last;
  struct memory_handler_struct *memory_handler;
  if (len == 0 || len > (Bit64u)((bx_phy_address)-1) ||
      addr > ((bx_phy_address)-1) - (bx_phy_address)len) return 0;
  last = addr + (bx_phy_address)len - 1;
  current = addr;
  while (1) {
    page_last = current | 0xfff;
    if (page_last > last) page_last = last;
    if (current >= BX_MEM_THIS len) return 0;
    memory_handler = BX_MEM_THIS memory_handlers[current >> 20];
    while (memory_handler) {
      if (memory_handler->begin <= page_last && memory_handler->end >= current) return 0;
      memory_handler = memory_handler->next;
    }
#if BX_SUPPORT_MONITOR_MWAIT
    if (BX_MEM_THIS is_monitor(current & ~((bx_phy_address)0xfff), 0xfff)) return 0;
#endif
    if (BX_MEM_THIS get_vector(current) == NULL) return 0;
    if (page_last == last) break;
    current = page_last + 1;
  }
  return 1;
}

bx_bool BX_MEM_C::backing_ram_writable(bx_phy_address addr, Bit64u len)
{ return BX_MEM_THIS backing_ram_accessible(addr, len); }

bx_bool BX_MEM_C::backing_ram_readable(bx_phy_address addr, Bit64u len)
{ return BX_MEM_THIS backing_ram_accessible(addr, len); }

bx_bool BX_MEM_C::copy_to_backing_ram(bx_phy_address addr, Bit64u len, const Bit8u *data)
{
  if (data == NULL || !BX_MEM_THIS backing_ram_writable(addr, len)) return 0;
  while (len != 0) {
    unsigned chunk = (unsigned)(0x1000 - (addr & 0xfff));
    if ((Bit64u)chunk > len) chunk = (unsigned)len;
    pageWriteStampTable.decWriteStamp(addr, chunk);
    memcpy(BX_MEM_THIS get_vector(addr), data, chunk);
    addr += chunk; data += chunk; len -= chunk;
  }
  return 1;
}

bx_bool BX_MEM_C::copy_from_backing_ram(bx_phy_address addr, Bit64u len, Bit8u *data)
{
  if (data == NULL || !BX_MEM_THIS backing_ram_readable(addr, len)) return 0;
  while (len != 0) {
    unsigned chunk = (unsigned)(0x1000 - (addr & 0xfff));
    if ((Bit64u)chunk > len) chunk = (unsigned)len;
    memcpy(data, BX_MEM_THIS get_vector(addr), chunk);
    addr += chunk; data += chunk; len -= chunk;
  }
  return 1;
}

bx_bool BX_MEM_C::copy_to_ordinary_ram(bx_phy_address addr, Bit64u len, const Bit8u *data)
{
  bx_phy_address current;
  Bit64u remaining;
  unsigned chunk;
  Bit8u *memptr;
  if (data == NULL || !BX_MEM_THIS ordinary_ram_writable(addr, len)) return 0;
  current = addr; remaining = len;
  while (remaining != 0) {
    chunk = (unsigned)(0x1000 - (current & 0xfff));
    if ((Bit64u)chunk > remaining) chunk = (unsigned)remaining;
    memptr = BX_MEM_THIS getHostMemAddr(NULL, current, BX_WRITE);
    if (memptr == NULL) return 0;
    pageWriteStampTable.decWriteStamp(current, chunk);
    memcpy(memptr, data, chunk);
    current += chunk; data += chunk; remaining -= chunk;
  }
  return 1;
}

bx_bool BX_MEM_C::ordinary_ram_readable(bx_phy_address addr, Bit64u len)
{
  bx_phy_address current, last, page_last;
  struct memory_handler_struct *memory_handler;
  if (len == 0 || len > (Bit64u)((bx_phy_address)-1) ||
      addr > ((bx_phy_address)-1) - (bx_phy_address)len) return 0;
  last = addr + (bx_phy_address)len - 1;
  current = addr;
  while (1) {
    page_last = current | 0xfff;
    if (page_last > last) page_last = last;
    if (A20ADDR(current) != current || A20ADDR(page_last) != page_last ||
        current >= BX_MEM_THIS len) return 0;
    memory_handler = BX_MEM_THIS memory_handlers[current >> 20];
    while (memory_handler) {
      if (memory_handler->begin <= page_last && memory_handler->end >= current) return 0;
      memory_handler = memory_handler->next;
    }
#if BX_SUPPORT_MONITOR_MWAIT
    if (BX_MEM_THIS is_monitor(current & ~((bx_phy_address)0xfff), 0xfff)) return 0;
#endif
    if (BX_MEM_THIS getHostMemAddr(NULL, current, BX_READ) == NULL) return 0;
    if (page_last == last) break;
    current = page_last + 1;
  }
  return 1;
}

bx_bool BX_MEM_C::copy_from_ordinary_ram(bx_phy_address addr, Bit64u len, Bit8u *data)
{
  bx_phy_address current;
  Bit64u remaining;
  unsigned chunk;
  Bit8u *memptr;
  if (data == NULL || !BX_MEM_THIS ordinary_ram_readable(addr, len)) return 0;
  current = addr; remaining = len;
  while (remaining != 0) {
    chunk = (unsigned)(0x1000 - (current & 0xfff));
    if ((Bit64u)chunk > remaining) chunk = (unsigned)remaining;
    memptr = BX_MEM_THIS getHostMemAddr(NULL, current, BX_READ);
    if (memptr == NULL) return 0;
    memcpy(data, memptr, chunk);
    current += chunk; data += chunk; remaining -= chunk;
  }
  return 1;
}
