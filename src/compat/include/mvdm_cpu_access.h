#ifndef COMPAT_MVDM_CPU_ACCESS_H
#define COMPAT_MVDM_CPU_ACCESS_H
#include <windows.h>
#include <stdint.h>
#include "guest_location.h"

/* Immutable CCPU operations, installed before guest execution. The provider
 * owns CPU state and mappings; B retains only process-lifetime function pointers.
 * Raw memory views are synchronous borrows on the CPU owner; they never
 * cross threads or outlive the existing mapping/host-context lifetime. */
enum { MVDM_CPU_ACCESS_USER = 0x2, MVDM_CPU_ACCESS_V86 = 0x4 };

typedef struct mvdm_cpu_access {
    void (__cdecl *capture_debug)(ULONG *, BOOL *);
    void (__cdecl *restore_debug)(const ULONG *, BOOL);
    void (__cdecl *apply_debug)(const ULONG *);
    ULONG (__cdecl *memory_size)(void);
    int (__cdecl *paging)(void);
    int (__cdecl *virtual_mode)(void);
    unsigned int (__cdecl *privilege)(void);
    BOOL (__cdecl *selector_outside)(USHORT, ULONG *);
    void (__cdecl *descriptor_fields)(ULONG, ULONG *, ULONG *, USHORT *);
    int (__cdecl *translate)(ULONG, unsigned char, ULONG *);
    int (__cdecl *read_memory)(uint32_t, uint8_t *, uint32_t);
    USHORT (__cdecl *machine_status)(void);
    ULONG (__cdecl *flags)(void);
    BOOL (__cdecl *delay_interrupt)(int, int, ULONG);
    void (__cdecl *cancel_interrupt)(ULONG, ULONG);
    int (*acquire_location)(const mvdm_guest_location *, uint32_t, uint32_t,
        mvdm_guest_location_lease *);
    int (*release_location)(mvdm_guest_location_lease *, int);
    int (*copy_to_guest)(const mvdm_guest_location *, const uint8_t *, uint32_t);
    /* Original SAS, register and allocation operations used by WOW page views. */
    ULONG (__cdecl *read_dword)(ULONG address);
    int (__cdecl *twenty_bit_wrapping)(void);
    void (__cdecl *fill_bytes)(ULONG address, BYTE value, ULONG count);
    void (__cdecl *store_dword)(ULONG address, ULONG value);
    void (__cdecl *store_word)(ULONG address, USHORT value);
    void (__cdecl *load_bytes)(ULONG address, BYTE *bytes, ULONG count);
    void (__cdecl *store_bytes)(ULONG address, BYTE *bytes, ULONG count);
    BYTE * (__cdecl *linear_pointer)(ULONG address);
    BYTE * (__cdecl *physical_pointer)(ULONG address);
    ULONG (__cdecl *cr0)(void);
    ULONG (__cdecl *cr3)(void);
    void (__cdecl *set_cr0)(ULONG value);
    void (__cdecl *set_cr3)(ULONG value);
    ULONG (__cdecl *gdt_base)(void);
    USHORT (__cdecl *gdt_limit)(void);
    void (__cdecl *set_gdt)(ULONG base, USHORT limit);
    LONG (__cdecl *allocate_virtual_memory)(ULONG *address, ULONG bytes, BOOL commit);
    LONG (__cdecl *free_virtual_memory)(ULONG address);
    LONG (__cdecl *add_virtual_memory)(ULONG host_address, ULONG bytes, ULONG *intel_address);
    LONG (__cdecl *remove_virtual_memory)(ULONG address);
    /* Register frame used by the synchronous BOP adapter. */
    ULONG (__cdecl *eax)(void);
    ULONG (__cdecl *ecx)(void);
    ULONG (__cdecl *edx)(void);
    ULONG (__cdecl *esi)(void);
    ULONG (__cdecl *ebx)(void);
    int (__cdecl *protected_mode)(void);
    void (__cdecl *set_eax)(ULONG value);
    void (__cdecl *set_ebx)(ULONG value);
    void (__cdecl *set_edx)(ULONG value);
    void (__cdecl *set_esi)(ULONG value);
    void (__cdecl *set_carry)(int value);
    void (__cdecl *unhandled_bop)(void);
    /* Original ICA serialization, keyboard mapping and synchronous VM access. */
    void (__cdecl *ica_lock)(void);
    void (__cdecl *ica_unlock)(void);
    WORD (__cdecl *reserve_irq)(HANDLE, WORD);
    BOOL (__cdecl *release_irq)(HANDLE, WORD);
    BOOL (__cdecl *register_eoi)(int, void (*)(int, int));
    BOOL (__cdecl *remove_eoi)(int, void (*)(int, int));
    void (__cdecl *raise_interrupt)(ULONG, ULONG, LONG);
    BOOL (__cdecl *bios_key)(PKEY_EVENT_RECORD);
    USHORT (__cdecl *ip)(void);
    void (__cdecl *set_ip)(USHORT);
    void (__cdecl *call_far)(unsigned long [2]);
    int (__cdecl *acquire_memory)(uint32_t, uint32_t, uint32_t, guest_memory_lease **, uint8_t **);
    int (__cdecl *release_memory)(guest_memory_lease *, int);
    int (__cdecl *copy_memory_from)(uint32_t, uint8_t *, uint32_t);
    int (__cdecl *copy_memory_to)(uint32_t, const uint8_t *, uint32_t);
} mvdm_cpu_access;
BOOL mvdm_cpu_access_install(const mvdm_cpu_access *provider);
const mvdm_cpu_access *mvdm_cpu_access_current(void);
#endif
