#include <nt.h>
#include "dem.h"

_Static_assert(sizeof(DEVICE_TYPE) == sizeof(ULONG), "historical DEVICE_TYPE form");
_Static_assert(sizeof(DISK_GEOMETRY) >= sizeof(ULONG), "historical disk geometry visible");

int main(void)
{
    BOOL (*disk_space_helper)(CHAR, PDISKINFO) = GetDiskSpaceInformation;
    return disk_space_helper == 0;
}
