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

## Follow-up

Continue the same immutable GSET snapshot package: volume/DPB/free-space
queries must retain excluded-drive failure and never rescan an ambient host
drive. File/handle/FCB/search remains outside this subpackage.
