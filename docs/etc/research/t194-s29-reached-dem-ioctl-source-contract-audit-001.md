# M0 T194 S29: Reached DEM IOCTL Source-Contract Audit

Packet: M0 T194 S29  
Disposition: source classification complete; current reached AL is unobserved.

## Source Facts

`dossvc.h`/`demdisp.c` bind `50:21` to `demIOCTL`. `demioctl.c` dispatches
solely on AL. The original guest `ioctl.asm` forwards only AL `08`, `09`, `0D`,
and `11` to that service.

AL `08` and `09` are the one coherent projected-drive metadata family:
`demIoctlChangeable` reads only BL's zero-based drive type. `08` returns AX=0
for removable, AX=1 otherwise, CF clear; `09` returns AL=0 and DX=1000h for
remote/CD-ROM or 0800h otherwise, CF clear. Both can consume only the immutable
CLI drive-type snapshot when separately admitted together.

AL `0D` and `11` are one raw-block-media family. Original code needs BDS
state, SI:DX guest parameter blocks, host device calls, error translation, and
mutable track/media behavior. It cannot be represented as a drive-type query
or adapter success response. Other original AL table entries are guest-DOS
local or invalid and are not adapter admission candidates.

## Current Evidence And Disposition

S28 reaches `50:21` at `9346:443c`, but the currently enabled generic BOP log
does not include copied AX/BL. The old trace record proved a different earlier
observation used AL=08/BL=02; it is useful source correlation, but is not proof
of this new post-DPB invocation. The S28 record therefore supports only
“`demIOCTL` reached and passed through.”

The existing legacy `bx_ntvdm_dem_ioctl_changeable_service` recognizes only
AX=4408 and only static profile inventory. It is not a normal CLI snapshot
provider, lacks paired AL=09, and cannot be selected on this trace without
first observing AL. It must not be extended speculatively.

## Follow-up

The registered default-off Bochs diagnostic `BX-TRACE-043` can append copied
AX/BX values to the generic observed-BOP line without branching on selector,
reading guest memory, or accepting an adapter result. A later bounded packet
may build that existing registered diagnostic into a named derivative and make
one observation after resolving the S28 watchdog-permission mechanism. Until
then no IOCTL provider, raw-media feature, host query, or retry is admitted.
