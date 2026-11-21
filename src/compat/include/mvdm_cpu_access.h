#ifndef COMPAT_MVDM_CPU_ACCESS_H
#define COMPAT_MVDM_CPU_ACCESS_H
#include <windows.h>
#include <stdint.h>

/* Immutable CCPU operations, installed before guest execution. The provider
 * owns CPU state and mappings; B retains only process-lifetime function pointers.
 * Guest addresses and descriptor outputs are numeric, never borrowed objects. */
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
} mvdm_cpu_access;
BOOL mvdm_cpu_access_install(const mvdm_cpu_access *provider);
const mvdm_cpu_access *mvdm_cpu_access_current(void);
#endif
