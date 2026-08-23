/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2012  The Bochs Project
//
//  I/O port handlers API Copyright (C) 2003 by Frank Cornelis
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
/////////////////////////////////////////////////////////////////////////

#include "iodev.h"

#define LOG_THIS bx_devices.

bx_devices_c bx_devices;

// BX-MANTLE-065-BEGIN
// Original no-device object lifetime and port dispatch, separated from the
// full product/device lifecycle in iodev/devices.cc.
bx_devices_c::bx_devices_c()
{
  put("devices", "DEV");

  read_port_to_handler = NULL;
  write_port_to_handler = NULL;
  io_read_handlers.next = NULL;
  io_read_handlers.handler_name = NULL;
  io_write_handlers.next = NULL;
  io_write_handlers.handler_name = NULL;
  init_stubs();

  for (unsigned i=0; i < BX_MAX_IRQS; i++) {
    irq_handler_name[i] = NULL;
  }
}

bx_devices_c::~bx_devices_c()
{
  // nothing needed for now
  timer_handle = BX_NULL_TIMER_HANDLE;
}

void bx_devices_c::init_stubs()
{
  pluginPciBridge = &stubPci;
  pluginPci2IsaBridge = &stubPci2Isa;
  pluginPciIdeController = &stubPciIde;
#if BX_SUPPORT_PCI
  pluginACPIController = &stubACPIController;
#endif
  pluginKeyboard = &stubKeyboard;
  pluginDmaDevice = &stubDma;
  pluginFloppyDevice = &stubFloppy;
  pluginCmosDevice = &stubCmos;
  pluginVgaDevice = &stubVga;
  pluginPicDevice = &stubPic;
  pluginHardDrive = &stubHardDrive;
  pluginSpeaker = &stubSpeaker;
#if BX_SUPPORT_IODEBUG
  pluginIODebug = &stubIODebug;
#endif
#if BX_SUPPORT_APIC
  pluginIOAPIC = &stubIOAPIC;
#endif
#if BX_SUPPORT_GAMEPORT
  pluginGameport = &stubGameport;
#endif
#if BX_SUPPORT_PCIUSB
  pluginUsbDevCtl = &stubUsbDevCtl;
#endif
#if BX_SUPPORT_SOUNDLOW
  pluginSoundModCtl = &stubSoundModCtl;
#endif
#if BX_NETWORKING
  pluginNetModCtl = &stubNetModCtl;
#endif
}
// BX-MANTLE-065-END

// BX-IO-025-BEGIN
bx_bool bx_devices_c::init_empty_port_space(void)
{
  unsigned i;
  const char def_name[] = "Default";

  if (read_port_to_handler != NULL || write_port_to_handler != NULL ||
      io_read_handlers.next != NULL || io_read_handlers.prev != NULL ||
      io_read_handlers.handler_name != NULL || io_write_handlers.next != NULL ||
      io_write_handlers.prev != NULL || io_write_handlers.handler_name != NULL) {
    return 0;
  }

  register_default_io_read_handler(NULL, &default_read_handler, def_name, 7);
  io_read_handlers.next = &io_read_handlers;
  io_read_handlers.prev = &io_read_handlers;
  io_read_handlers.usage_count = 0;

  register_default_io_write_handler(NULL, &default_write_handler, def_name, 7);
  io_write_handlers.next = &io_write_handlers;
  io_write_handlers.prev = &io_write_handlers;
  io_write_handlers.usage_count = 0;

  read_port_to_handler = new struct io_handler_struct *[PORTS];
  write_port_to_handler = new struct io_handler_struct *[PORTS];
  for (i = 0; i < PORTS; i++) {
    read_port_to_handler[i] = &io_read_handlers;
    write_port_to_handler[i] = &io_write_handlers;
  }
  return 1;
}

// This defines the builtin default read handler,
// so Bochs does not segfault if unmapped is not loaded
Bit32u bx_devices_c::default_read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
  UNUSED(this_ptr);
  return 0xffffffff;
}

// This defines the builtin default write handler,
// so Bochs does not segfault if unmapped is not loaded
void bx_devices_c::default_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
  UNUSED(this_ptr);
}

// Registration of default handlers (mainly be the unmapped device)
bx_bool bx_devices_c::register_default_io_read_handler(void *this_ptr, bx_read_handler_t f,
                                               const char *name, Bit8u mask)
{
  io_read_handlers.funct = (void *)f;
  io_read_handlers.this_ptr = this_ptr;
  if (io_read_handlers.handler_name) {
    delete [] io_read_handlers.handler_name;
  }
  io_read_handlers.handler_name = new char[strlen(name)+1];
  strcpy(io_read_handlers.handler_name, name);
  io_read_handlers.mask = mask;

  return 1;
}

bx_bool bx_devices_c::register_default_io_write_handler(void *this_ptr, bx_write_handler_t f,
                                               const char *name, Bit8u mask)
{
  io_write_handlers.funct = (void *)f;
  io_write_handlers.this_ptr = this_ptr;
  if (io_write_handlers.handler_name) {
    delete [] io_write_handlers.handler_name;
  }
  io_write_handlers.handler_name = new char[strlen(name)+1];
  strcpy(io_write_handlers.handler_name, name);
  io_write_handlers.mask = mask;

  return 1;
}

bx_bool bx_devices_c::cleanup_empty_port_space(void)
{
  unsigned i;

  if (read_port_to_handler == NULL && write_port_to_handler == NULL &&
      io_read_handlers.next == NULL && io_read_handlers.prev == NULL &&
      io_read_handlers.handler_name == NULL && io_write_handlers.next == NULL &&
      io_write_handlers.prev == NULL && io_write_handlers.handler_name == NULL) {
    return 1;
  }
  if (read_port_to_handler == NULL || write_port_to_handler == NULL ||
      io_read_handlers.next != &io_read_handlers || io_read_handlers.prev != &io_read_handlers ||
      io_write_handlers.next != &io_write_handlers || io_write_handlers.prev != &io_write_handlers) {
    return 0;
  }
  for (i = 0; i < PORTS; i++) {
    if (read_port_to_handler[i] != &io_read_handlers ||
        write_port_to_handler[i] != &io_write_handlers) {
      return 0;
    }
  }

  delete [] read_port_to_handler;
  delete [] write_port_to_handler;
  read_port_to_handler = NULL;
  write_port_to_handler = NULL;
  delete [] io_read_handlers.handler_name;
  delete [] io_write_handlers.handler_name;
  io_read_handlers.next = NULL;
  io_read_handlers.prev = NULL;
  io_read_handlers.funct = NULL;
  io_read_handlers.this_ptr = NULL;
  io_read_handlers.handler_name = NULL;
  io_read_handlers.usage_count = 0;
  io_read_handlers.mask = 0;
  io_write_handlers.next = NULL;
  io_write_handlers.prev = NULL;
  io_write_handlers.funct = NULL;
  io_write_handlers.this_ptr = NULL;
  io_write_handlers.handler_name = NULL;
  io_write_handlers.usage_count = 0;
  io_write_handlers.mask = 0;
  return 1;
}

// BX-MANTLE-082-BEGIN
// Original per-port registration and cleanup used by the one native PIC.
bx_bool bx_devices_c::register_io_read_handler(void *this_ptr, bx_read_handler_t f,
                                               Bit32u addr, const char *name, Bit8u mask)
{
  addr &= 0xffff;

  if (!f)
    return 0;

  /* first check if the port already has a handlers != the default handler */
  if (read_port_to_handler[addr] &&
      read_port_to_handler[addr] != &io_read_handlers) { // the default
    BX_ERROR(("IO device address conflict(read) at IO address %Xh",
              (unsigned) addr));
    BX_ERROR(("  conflicting devices: %s & %s",
              read_port_to_handler[addr]->handler_name, name));
    return 0;
  }

  /* first find existing handle for function or create new one */
  struct io_handler_struct *curr = &io_read_handlers;
  struct io_handler_struct *io_read_handler = NULL;
  do {
    if (curr->funct == f &&
        curr->mask == mask &&
        curr->this_ptr == this_ptr &&
        !strcmp(curr->handler_name, name)) { // really want the same name too
      io_read_handler = curr;
      break;
    }
    curr = curr->next;
  } while (curr->next != &io_read_handlers);

  if (!io_read_handler) {
    io_read_handler = new struct io_handler_struct;
    io_read_handler->funct = (void *)f;
    io_read_handler->this_ptr = this_ptr;
    io_read_handler->handler_name = new char[strlen(name)+1];
    strcpy(io_read_handler->handler_name, name);
    io_read_handler->mask = mask;
    io_read_handler->usage_count = 0;
    // add the handler to the double linked list of handlers
    io_read_handlers.prev->next = io_read_handler;
    io_read_handler->next = &io_read_handlers;
    io_read_handler->prev = io_read_handlers.prev;
    io_read_handlers.prev = io_read_handler;
  }

  io_read_handler->usage_count++;
  read_port_to_handler[addr] = io_read_handler;
  return 1; // address mapped successfully
}

bx_bool bx_devices_c::register_io_write_handler(void *this_ptr, bx_write_handler_t f,
                                                Bit32u addr, const char *name, Bit8u mask)
{
  addr &= 0xffff;

  if (!f)
    return 0;

  /* first check if the port already has a handlers != the default handler */
  if (write_port_to_handler[addr] &&
      write_port_to_handler[addr] != &io_write_handlers) { // the default
    BX_ERROR(("IO device address conflict(write) at IO address %Xh",
              (unsigned) addr));
    BX_ERROR(("  conflicting devices: %s & %s",
              write_port_to_handler[addr]->handler_name, name));
    return 0;
  }

  /* first find existing handle for function or create new one */
  struct io_handler_struct *curr = &io_write_handlers;
  struct io_handler_struct *io_write_handler = NULL;
  do {
    if (curr->funct == f &&
        curr->mask == mask &&
        curr->this_ptr == this_ptr &&
        !strcmp(curr->handler_name, name)) { // really want the same name too
      io_write_handler = curr;
      break;
    }
    curr = curr->next;
  } while (curr->next != &io_write_handlers);

  if (!io_write_handler) {
    io_write_handler = new struct io_handler_struct;
    io_write_handler->funct = (void *)f;
    io_write_handler->this_ptr = this_ptr;
    io_write_handler->handler_name = new char[strlen(name)+1];
    strcpy(io_write_handler->handler_name, name);
    io_write_handler->mask = mask;
    io_write_handler->usage_count = 0;
    // add the handler to the double linked list of handlers
    io_write_handlers.prev->next = io_write_handler;
    io_write_handler->next = &io_write_handlers;
    io_write_handler->prev = io_write_handlers.prev;
    io_write_handlers.prev = io_write_handler;
  }

  io_write_handler->usage_count++;
  write_port_to_handler[addr] = io_write_handler;
  return 1; // address mapped successfully
}

bx_bool bx_devices_c::unregister_io_read_handler(void *this_ptr, bx_read_handler_t f,
                                         Bit32u addr, Bit8u mask)
{
  addr &= 0xffff;

  struct io_handler_struct *io_read_handler = read_port_to_handler[addr];

  //BX_INFO(("Unregistering I/O read handler at %#x", addr));

  if (!io_read_handler) {
    BX_ERROR((">>> NO IO_READ_HANDLER <<<"));
    return 0;
  }

  if (io_read_handler == &io_read_handlers) {
    BX_ERROR((">>> CANNOT UNREGISTER THE DEFAULT IO_READ_HANDLER <<<"));
    return 0; // cannot unregister the default handler
  }

  if (io_read_handler->funct != f) {
    BX_ERROR((">>> NOT THE SAME IO_READ_HANDLER FUNC <<<"));
    return 0;
  }

  if (io_read_handler->this_ptr != this_ptr) {
    BX_ERROR((">>> NOT THE SAME IO_READ_HANDLER THIS_PTR <<<"));
    return 0;
  }

  if (io_read_handler->mask != mask) {
    BX_ERROR((">>> NOT THE SAME IO_READ_HANDLER MASK <<<"));
    return 0;
  }

  read_port_to_handler[addr] = &io_read_handlers; // reset to default
  io_read_handler->usage_count--;

  if (!io_read_handler->usage_count) { // kill this handler entry
    io_read_handler->prev->next = io_read_handler->next;
    io_read_handler->next->prev = io_read_handler->prev;
    delete [] io_read_handler->handler_name;
    delete io_read_handler;
  }
  return 1;
}

bx_bool bx_devices_c::unregister_io_write_handler(void *this_ptr, bx_write_handler_t f,
                                          Bit32u addr, Bit8u mask)
{
  addr &= 0xffff;

  struct io_handler_struct *io_write_handler = write_port_to_handler[addr];

  if (!io_write_handler)
    return 0;

  if (io_write_handler == &io_write_handlers)
    return 0; // cannot unregister the default handler

  if (io_write_handler->funct != f)
    return 0;

  if (io_write_handler->this_ptr != this_ptr)
    return 0;

  if (io_write_handler->mask != mask)
    return 0;

  write_port_to_handler[addr] = &io_write_handlers; // reset to default
  io_write_handler->usage_count--;

  if (!io_write_handler->usage_count) { // kill this handler entry
    io_write_handler->prev->next = io_write_handler->next;
    io_write_handler->next->prev = io_write_handler->prev;
    delete [] io_write_handler->handler_name;
    delete io_write_handler;
  }
  return 1;
}
// BX-MANTLE-082-END

// BX-IO-025: fixed-width mantle callers need to distinguish a native
// registered device endpoint from the retained original default handler.
// This query neither changes dispatch nor gives the caller an object pointer.
bx_bool bx_devices_c::ntdos64_port_is_registered(Bit16u addr, bx_bool write) const
{
  if (write)
    return write_port_to_handler != NULL &&
      write_port_to_handler[addr] != &io_write_handlers;
  return read_port_to_handler != NULL &&
    read_port_to_handler[addr] != &io_read_handlers;
}

// BX-MANTLE-065-DISPATCH-BEGIN
Bit32u BX_CPP_AttrRegparmN(2)
bx_devices_c::inp(Bit16u addr, unsigned io_len)
{
  struct io_handler_struct *io_read_handler;
  Bit32u ret;

  BX_INSTR_INP(addr, io_len);

  io_read_handler = read_port_to_handler[addr];
  if (io_read_handler->mask & io_len) {
    ret = ((bx_read_handler_t)io_read_handler->funct)(io_read_handler->this_ptr, (Bit32u)addr, io_len);
  } else {
    switch (io_len) {
      case 1: ret = 0xff; break;
      case 2: ret = 0xffff; break;
      default: ret = 0xffffffff; break;
    }
    if (addr != 0x0cf8) {
      BX_ERROR(("read from port 0x%04x with len %d returns 0x%x", addr, io_len, ret));
    }
  }

  BX_INSTR_INP2(addr, io_len, ret);
  BX_DBG_IO_REPORT(addr, io_len, BX_READ, ret);

  return(ret);
}

void BX_CPP_AttrRegparmN(3)
bx_devices_c::outp(Bit16u addr, Bit32u value, unsigned io_len)
{
  struct io_handler_struct *io_write_handler;

  BX_INSTR_OUTP(addr, io_len, value);
  BX_DBG_IO_REPORT(addr, io_len, BX_WRITE, value);

  io_write_handler = write_port_to_handler[addr];
  if (io_write_handler->mask & io_len) {
    ((bx_write_handler_t)io_write_handler->funct)(io_write_handler->this_ptr, (Bit32u)addr, value, io_len);
  } else if (addr != 0x0cf8) {
    BX_ERROR(("write to port 0x%04x with len %d ignored", addr, io_len));
  }
}
// BX-MANTLE-065-DISPATCH-END
// BX-IO-025-END
