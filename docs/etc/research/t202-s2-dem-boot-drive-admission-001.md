# T202 S2: DEM boot-drive selected-host admission

## Question

Can `demGetBootDrive` preserve an OpenNT-style boot-drive result without
silently manufacturing C: when CLI policy excludes it?

## Source and adaptation

`demgset.c:demGetBootDrive` reads the historical BootDir registry value and
falls back to C:. The modern contained composition instead already has a
CLI-admitted startup drive and immutable host-drive snapshot. The new v2
provider therefore selects, in order:

1. the admitted CLI startup drive;
2. actual admitted fixed C:, if present;
3. the first actual admitted fixed drive.

If no eligible fixed drive exists it declines rather than return a fabricated
drive letter. The old v1 C fallback remains only for retained legacy adapter
surfaces; normal DEM package dispatch uses v2.

## Evidence

MSVC x64 `/MT` probe `t202-s2-dem-boot-drive-r1` passed with no host I/O or
guest execution. It proves that an admission snapshot containing only D:
returns DOS drive 4, that an admitted preferred D: is retained, and that an
empty admitted set does not return C:.

## Follow-up

Keep the snapshot/volume facts immutable across direct and readonly profiles.
The next package work reconciles the remaining GSET volume observation paths
and their excluded-drive failures; it does not enter file/handle/FCB/search.
