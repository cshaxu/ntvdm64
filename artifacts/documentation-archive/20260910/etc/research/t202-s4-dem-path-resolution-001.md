# T202 S4: DEM DOS path resolution

## Question

Can the forthcoming one-provider DEM file family share one bounded conversion
from OpenNT's OEM DOS pathname ABI to the adapter-private, admitted-root
relative pathname contract?

## Inputs and procedure

The resolver consumes a bounded NUL-terminated OEM string, the existing
session-private DEM CWD/default-drive context, and no host path or handle.  It
uses the current OEM code page only to copy into a fixed wide buffer, selects
an explicit drive or the session default, and normalizes each component into
one uppercase DOS 8.3 component.

`Invoke-T202S4DemPathProbe.ps1` builds the resolver, profile/CWD support and
focused test with MSVC x64 `/MT`.

## Observations

`artifacts/build/t202-s4-dem-path-r7` compiled and returned zero.  The test
proves absolute `C:\\DOS\\TEST.TXT` becomes drive index 2 plus
`DOS\\TEST.TXT`; a relative name uses the default drive; and dot-segment and
long-name inputs are rejected before host access.

## Interpretation and follow-up

This is an adapter-private package primitive, not a BOP provider and not a
general host pathname bridge.  It preserves the current short-name policy
already used by the host-namespace boundary and gives the later direct,
readonly and overlay file paths one common input contract.  The next S4 step
binds it to the root-relative open and opaque file session; only then can the
old boot/readonly open leaf be displaced by the whole provider.
