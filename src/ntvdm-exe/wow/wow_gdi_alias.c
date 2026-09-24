#include "wow_gdi_alias.h"

#define MVDM_WOW_GDI_SLOT_COUNT 0x4000u

typedef struct mvdm_wow_gdi_alias_table {
    SRWLOCK lock;
    HANDLE native[MVDM_WOW_GDI_SLOT_COUNT];
} mvdm_wow_gdi_alias_table;

static mvdm_wow_gdi_alias_table table = { SRWLOCK_INIT };

WORD __cdecl mvdm_wow_gdi_handle_from_native(HANDLE native)
{
    DWORD index;
    DWORD free_index = 0;

    if (native == NULL) return 0;
    AcquireSRWLockExclusive(&table.lock);
    for (index = 1; index < MVDM_WOW_GDI_SLOT_COUNT; ++index) {
        if (table.native[index] == native) {
            ReleaseSRWLockExclusive(&table.lock);
            return (WORD)(index << 2);
        }
        if (free_index == 0 && table.native[index] == NULL)
            free_index = index;
    }
    if (free_index != 0) {
        table.native[free_index] = native;
        ReleaseSRWLockExclusive(&table.lock);
        return (WORD)(free_index << 2);
    }
    ReleaseSRWLockExclusive(&table.lock);
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
    return 0;
}

HANDLE __cdecl mvdm_wow_gdi_handle_to_native(WORD guest)
{
    DWORD index = (DWORD)(guest >> 2);
    HANDLE native;

    if (index == 0) return NULL;
    AcquireSRWLockShared(&table.lock);
    native = table.native[index];
    ReleaseSRWLockShared(&table.lock);
    /* An unallocated nonzero index must remain invalid to native GDI.  NULL
     * is a meaningful DC input (screen DC) and would change original failure
     * behavior for a stale/free guest slot. */
    return native ? native : INVALID_HANDLE_VALUE;
}

void __cdecl mvdm_wow_gdi_handle_retire(WORD guest)
{
    DWORD index = (DWORD)(guest >> 2);

    if (index == 0) return;
    AcquireSRWLockExclusive(&table.lock);
    table.native[index] = NULL;
    ReleaseSRWLockExclusive(&table.lock);
}
