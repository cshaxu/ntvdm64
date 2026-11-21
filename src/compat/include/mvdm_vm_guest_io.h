#ifndef COMPAT_MVDM_VM_GUEST_IO_H
#define COMPAT_MVDM_VM_GUEST_IO_H

#include <windows.h>
#include <stdint.h>

/* CPU-side bounded descriptor transport, reached only after bridge signature
 * and ring checks. Uses the same continuation owner as native publication. */
DWORD mvdm_vm_application_bop(uint32_t operation, uint32_t vm,
    uint32_t descriptor_address, uint32_t descriptor_extent);

/* CPU-only generation low/high and active OEM code page. Transfer one
 * converted BIOS word/repeat count; failed conversion keeps the record. */
DWORD mvdm_vm_executor_return_key_bop(uint32_t vm, uint32_t address,
    uint32_t extent, uint32_t *word, uint32_t *repeat);

#endif
