#ifndef MVDM_REDIRECTOR_POINTER_SCOPE_H
#define MVDM_REDIRECTOR_POINTER_SCOPE_H

#include <stdint.h>

/* Same-shaped bridge for original Redirector bodies that use transient
 * GetVDMAddr/Sim32GetVDMPointer aliases.  A pointer exists only between begin
 * and end on the calling thread; it is never a guest value or durable token. */
int mvdm_redirector_pointer_scope_begin(void);
int mvdm_redirector_pointer_scope_end(int commit);
void *mvdm_redirector_pointer_from_real_mode(uint16_t segment, uint16_t offset);
void *mvdm_redirector_pointer_from_source_address(uint32_t address, uint32_t protected_mode);

#endif
