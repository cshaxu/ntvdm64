# T202 S2: DEM GSET filtered-drive projection

## Question

Does `50:0F demGetDrives` preserve explicitly admitted host drive letters when
CLI policy excludes an intermediate letter?

## Source-derived adaptation

OpenNT's `demGetDrives` counts a contiguous physical-drive prefix. Its host
inventory had no policy holes. NTDOS64 intentionally permits a projection such
as `--include-drives=c,e` or `--exclude-drives=d`; applying the historical
contiguous count directly would report only C: and make admitted E: unreachable.

The GSET provider now returns the highest admitted physical DOS drive letter
plus one (with the original A:/B: lower bound). It only considers admitted
removable/fixed/CD/RAM drive types. This preserves guest letter identity while
continuing to hide excluded drives; it neither compacts nor fabricates a drive.

## Evidence

MSVC x64 `/MT` probe `t202-s2-dem-drive-provider-r1` passed without host I/O
or guest execution. It covers a contiguous C:/D:/E: projection and a C:/E:
projection with D: absent; both return the DOS bound 5 in AX with CF clear.

## Volume-observation failure reconciliation

The same immutable snapshot now supplies one source-shaped failure group for
`50:0E demGetDriveFreeSpace`, `50:10 demGSetMediaID` when `AL=0`, and
`50:25 demGetDPB`.  In `demgset.c`, each successful host-volume path calls
`demClientError` when its underlying query fails.  `demerror.c` preserves a
normal Win32 error in AX and sets CF; a drive excluded by CLI admission is
therefore represented as `ERROR_INVALID_DRIVE` (15), not the unrelated
generic `ERROR_ACCESS_DENIED` (5) provider fence.

The source-specific `50:10 AL!=0` branch remains unchanged: OpenNT does not
support setting media information and sets CF without assigning AX.  No host
volume write is admitted.

The source-built `t202-s2-volume-failure-r1` package fixture passed under
MSVC x64 `/MT`.  It proves that an excluded drive yields AX=15/CF=1 for all
three observation services; media and DPB destination bytes remain unchanged.
The fixture uses an immutable in-process snapshot and performs no host I/O or
guest execution.

## Follow-up

Continue the same immutable GSET snapshot package: volume/DPB/free-space
queries must never rescan an ambient host drive. File/handle/FCB/search
remains outside this subpackage.
