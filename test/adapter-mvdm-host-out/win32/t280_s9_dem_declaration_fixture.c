#include <nt.h>
#include "dem.h"

_Static_assert(sizeof(DEVICE_TYPE) == sizeof(ULONG), "historical DEVICE_TYPE form");
_Static_assert(sizeof(DISK_GEOMETRY) >= sizeof(ULONG), "historical disk geometry visible");
_Static_assert(sizeof(DEMEXTERR) == 9u, "DOS extended-error guest layout");
_Static_assert(sizeof(DOSWOWDATA) == 36u, "DOS/WOW guest data layout");
_Static_assert(sizeof(DOSSF) == 8u, "DOS SFT header layout");
_Static_assert(sizeof(DOSSFT) == 33u, "DOS SFT entry layout");

int main(void)
{
    BOOL (*disk_space_helper)(CHAR, PDISKINFO) = GetDiskSpaceInformation;
    return disk_space_helper == 0;
}
