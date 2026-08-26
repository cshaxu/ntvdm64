#ifndef RUNTIME_BOP_SHIM_SOFTPC_PRINTER_OPENCLOSE_SHIM_H
#define RUNTIME_BOP_SHIM_SOFTPC_PRINTER_OPENCLOSE_SHIM_H

#include <stdint.h>

/* Compatibility seam for the original no-I/O `host_lpt_dos_open/close`
 * lifecycle.  The host LPT HANDLE, buffer and port device are deliberately
 * outside this first subpackage. */
int runtime_softpc_printer_openclose_begin(uint16_t si, uint16_t dx);
int runtime_softpc_printer_openclose_end(void);
uint16_t runtime_softpc_printer_get_si(void);
uint16_t runtime_softpc_printer_get_dx(void);
void runtime_softpc_printer_host_lpt_dos_open(int adapter);
void runtime_softpc_printer_host_lpt_dos_close(int adapter);
/* OpenNT softpc.new/host/src/nt_lpt.c:688.  Keep the historical global
 * spelling so imported COMMAND code reaches this selector-blind facade. */
void host_lpt_flush_initialize(void);
void runtime_softpc_printer_unsupported(void);
void runtime_softpc_printer_openclose_reset(void);
int runtime_softpc_printer_dos_open_copy(uint16_t adapter, uint32_t *opened);

#endif
