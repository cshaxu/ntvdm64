# M0 T310 S8 P4 — `nt_mem` Page-Span ABI Audit

## Scope and source finding

This record covers the x64 `IHPE -> IU32` compiler diagnostic in original
`mvdm-host/softpc.new/host/src/nt_mem.c:exclusiveAllocPages`.

The helper receives native backing addresses only to calculate VirtualAlloc /
VirtualFree page boundaries.  Its `allocSize` output remains the original
fixed-width SoftPC memory-allocation ABI.  Every range is derived from the
one original `IU32 MaxIntelMemorySize` reservation, so the calculated page span
has the same upper bound before it is returned through `IU32`.

## Disposition

The existing `MVDM-HOST-DIV-034` private-native-address exception now carries
the complete local proof: preserve page arithmetic in `IHPE`, assert the
source allocation bound, and explicitly return the bounded result as `IU32`.
No native pointer crosses a public boundary at this point.

This is not a mapping-manager seam.  The separate original
`VdmAddVirtualMemory` external-page route remains `MVDM-HOST-DIV-035` and uses
the existing session mapping manager because its historical `HostAddress`
parameter represented an external host backing identity.

## Verification

The formal `original-softpc-candidate` Ninja graph was regenerated and
executed on x64 and x86.  Both compile and link the changed original
`nt_mem.c`; the selected x64 `nt_mem.c:1441` narrowing diagnostic is gone.
Other diagnostic rows remain separately classified owner-package work and were
not suppressed or changed.
