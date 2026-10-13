#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "wow32-dll/include/wow_task_parent.h"
#include "wow32-dll/include/wow_callback_frame_lease.h"
#include "mvdm/inc/tdb16.h"
#include <stddef.h>

#define CHECK(x) do { if (!(x)) { printf("FAIL line %d\n", __LINE__); return 1; } } while (0)
static TDB guest;
static unsigned acquired, released;
static int reject_acquire, reject_release, bad_request;

int wow_callback_frame_acquire_protected_vp(uint32_t vp, uint32_t bytes,
    uint32_t access, wow_callback_frame_lease *view)
{
    acquired++;
    if (vp != 0x12340000u || bytes != offsetof(TDB, TDB_sig) + sizeof(WORD) ||
        access != GUEST_MEMORY_ACCESS_READ) bad_request = 1;
    if (reject_acquire) return 0;
    memset(view, 0, sizeof(*view));
    view->bytes = (uint8_t *)&guest;
    return 1;
}

int wow_callback_frame_release(wow_callback_frame_lease *view, int commit)
{
    released++;
    if (commit || view->bytes != (uint8_t *)&guest) bad_request = 1;
    memset(view, 0, sizeof(*view));
    return !reject_release;
}

int main(void)
{
    WORD parent = 99;
    guest.TDB_sig = TDB_SIGNATURE;
    guest.TDB_Parent = 0x4321;
    CHECK(wow_task_parent(0x1234, &parent) && parent == 0x4321);
    CHECK(acquired == 1 && released == 1 && !bad_request);
    guest.TDB_Parent = 0;
    CHECK(wow_task_parent(0x1234, &parent) && !parent);
    CHECK(!wow_task_parent(0, &parent));
    CHECK(!wow_task_parent(0x11234, &parent));
    CHECK(!wow_task_parent(0x1234, NULL));
    CHECK(acquired == 2 && released == 2);
    guest.TDB_sig = 0;
    CHECK(!wow_task_parent(0x1234, &parent) && !parent);
    guest.TDB_sig = TDB_SIGNATURE;
    guest.TDB_Parent = 0x1234;
    CHECK(!wow_task_parent(0x1234, &parent) && !parent);
    reject_acquire = 1;
    CHECK(!wow_task_parent(0x1234, &parent) && !parent);
    CHECK(acquired == 5 && released == 4);
    reject_acquire = 0;
    reject_release = 1;
    guest.TDB_Parent = 0x4321;
    CHECK(!wow_task_parent(0x1234, &parent) && !parent);
    CHECK(acquired == 6 && released == 5 && !bad_request);
    puts("PASS original TDB parent layout, root, bounds, signature, self-parent and lease failures");
    return 0;
}
