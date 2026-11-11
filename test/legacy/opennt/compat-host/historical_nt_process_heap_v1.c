#include <windows.h>

/*
 * Modern ntdll no longer exports the historical RtlProcessHeap entry point.
 * The original DEM caller passes this handle straight to RtlFreeHeap after
 * RtlDosPathNameToNtPathName_U. GetProcessHeap is the documented modern
 * process-heap handle and preserves that ownership relation.
 */
PVOID NTAPI RtlProcessHeap(VOID)
{
    return GetProcessHeap();
}
