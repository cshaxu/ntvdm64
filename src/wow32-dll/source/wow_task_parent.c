#include "wow_task_parent.h"
#include "wow_callback_frame_lease.h"
#include "mvdm/inc/tdb16.h"
#include <stddef.h>
#include <string.h>

BOOL wow_task_parent(DWORD task, WORD *parent)
{
    wow_callback_frame_lease view;
    WORD signature, value;
    if (!parent || !task || task > 0xffffu) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    *parent = 0;
    /* Use the retained original declaration, not a guessed layout. Reading
     * through TDB_sig also proves this selector names a task, not a module. */
    if (!wow_callback_frame_acquire_protected_vp(task << 16,
            (uint32_t)(offsetof(TDB, TDB_sig) + sizeof(signature)),
            GUEST_MEMORY_ACCESS_READ, &view)) {
        SetLastError(ERROR_INVALID_ADDRESS); return FALSE;
    }
    memcpy(&signature, view.bytes + offsetof(TDB, TDB_sig), sizeof(signature));
    memcpy(&value, view.bytes + offsetof(TDB, TDB_Parent), sizeof(value));
    if (!wow_callback_frame_release(&view, 0)) {
        SetLastError(ERROR_INVALID_ADDRESS); return FALSE;
    }
    if (signature != TDB_SIGNATURE || value == task) {
        SetLastError(ERROR_INVALID_DATA); return FALSE;
    }
    *parent = value;
    return TRUE;
}
