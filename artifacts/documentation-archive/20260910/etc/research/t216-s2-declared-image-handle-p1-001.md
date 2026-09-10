# T216 S2 P1: Declared Image Handle Visibility

## Question

Can the installed direct DEM provider expose the already-declared
`COMMAND.COM` bytes through the same OpenNT-shaped readonly open/token path
as the attached startup images, without changing BOP ingress or host fallback?

## Change

The readonly namespace predicate now accepts every ready declared file slot
rather than only CONFIG/AUTOEXEC slots 1 and 2.  Its opaque-token ownership
follows the same all-slot declaration.  The existing direct DEM namespace and
handle routes therefore retain their original copied `50:12`, `50:00`,
`50:16`, `50:02`, `50:08`, write-refusal and mutation-refusal paths; no new
handler, selector or EXEC behavior was added.

## Verification

`Invoke-T202S4DemWholeProviderProbe.ps1` built the affected package with MSVC
x64 `/MT` at `artifacts/build/t216-s2-declared-images-r2` and passed.  The
extended fixture creates a namespace-owned `COMMAND.COM` image whose name is
not a host file and proves `50:12` returns its readonly token and seven-byte
size.  Existing CONFIG read, write refusal and close checks also remain in the
same probe.

## Limitation

The installed direct search partition still enumerates the host namespace
instead of a merged host-plus-declared snapshot.  S2 remains active: this P1
does not claim path/FCB search closure or native startup success.
