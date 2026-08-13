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

bx_devices_c bx_devices;

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
// BX-IO-025-END
