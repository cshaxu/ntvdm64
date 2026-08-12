# M0 T96 S3 demgset Provider Disposition 001

## Source boundary

OpenNT demgset.c is one source component but not one reusable CLI provider.
It reads the registry and system clock, mutates the host current directory and
system clock, probes physical drives, dereferences VDM pointers, and writes
DOS data structures. Direct linking would violate the CLI/adapter boundary.

## Service disposition

| Service | Original handler | Key historical dependency | Current disposition |
| ---: | --- | --- | --- |
| 13 | demGetBootDrive | registry BootDir plus fixed-drive test, with C fallback | source-derived candidate: declared CLI boot-drive policy may supply the same value; no registry read |
| 14 | demGetDriveFreeSpace | drive query and demClientError | deferred pending projected-drive capacity/error ABI |
| 15 | demGetDrives | Win32 drive probing and physical-drive filter | admitted source-derived drive-projection provider; immutable snapshot only |
| 16 | demGSetMediaID | VDM buffer and volume-media query | deferred pending projected-volume identity ABI |
| 20 | demQueryDate | local clock | CLI capability candidate; requires explicit clock policy and fixed-width result contract |
| 21 | demQueryTime | local clock | CLI capability candidate; requires explicit clock policy and fixed-width result contract |
| 25 | demSetDate | SetLocalTime and privilege exception | deferred: CLI profile must not mutate host time; no virtual clock contract admitted |
| 26 | demSetDefaultDrive | VDM string and SetCurrentDirectoryOem | deferred: CLI profile must not mutate host current directory; source failure path needs bounded guest-string ABI |
| 27 | demSetDTALocation | four VDM pointers and DOSWOW/SFT layout | source-derived candidate only after one checked multi-range registration ABI |
| 28 | demSetTime | SetLocalTime and privilege exception | deferred: CLI profile must not mutate host time; no virtual clock contract admitted |
| 37 | demGetDPB | VDM DPB pointer and drive geometry | deferred with DPB/drive-projection group |
| 65 | demGetComputerName | host computer name and VDM output pointer | deferred: host identity exposure needs an explicit CLI policy and checked output ABI |
| 70 | demGetDPBList | VDM DPB array and physical-drive types | source-derived candidate only after DPB/drive-projection group closes |

## Consequences

The existing adapter boot-drive, drive-list, DTA, and DPB-list endpoints are
not automatically admitted. They are candidates only where their eventual
input/output ABI can be proven against the source table. Date/time and
computer-name reads also require an explicit CLI capability policy; absence of
host mutation does not authorize ambient host-state exposure.

The remaining drive-projection decision is GETBOOTDRIVE: OpenNT falls back to
C when its registry value is unusable, but C can be excluded from the CLI
projection. A declared guest boot-drive policy is therefore required before
that service can be admitted; it must not silently reuse the old C fallback.

## GETDRIVES provider slice

bx_ntvdm_dem_drive_provider_v1 implements only service 15 through the common
DEM ingress and provider-selection records. Its input is a previously captured
and CLI-filtered drive snapshot; it performs no host query. It retains the
original demGetDrives counting rule: A/B establish a two-drive baseline, then
only contiguous removable/fixed/CD/RAM entries from C onward increase AX.
The provider resumes after the BOP, writes AX, and clears CF.

Focused tests cover both contiguous projected C/D/E drives and a gap that
stops the original-style count. The provider does not own GETBOOTDRIVE,
free-space, DPB, or any host mutation service.
