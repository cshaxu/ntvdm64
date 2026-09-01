# M0 T355 S14 P1 — FastRead user-mode recovery and reachability limit

## Implemented boundary

The original NT4 x86 product handles `SVC_DEMFASTREAD` in
`base/ntos/vdm/x86/rdwr.c::NTFastDOSIO`, not in the visible user-mode DEM
table.  The non-invasive CPU40/x86 product cannot compose the kernel trap
handler, VDM TIB or native kernel I/O calls.

For the reachable normal-file ABI, `dos/dem/demdisp.c` now selects the
existing original `demRead` body for `0x42`.  This is a one-entry
source-derived composition seam, registered as `MVDM-HOST-DIV-188`; it does
not create a second provider.  `demRead` already retains the same register
form (`AX:BP`, `DS:DX`, `CX`, `BX:SI`, ZF), resolves the opaque resource with
the session host-resource mapping manager, uses a bounded synchronous guest
lease, and returns the original `AX`/carry result.  A carry result still lets
the unmodified DOS `$READ` follow its original error/fallback control flow.

Console, standard-handle, pipe, trap-frame, VDM-TIB, IRQL and kernel-native
branches remain explicitly outside this user-mode normal-file replacement.

## Formal verification

The formal CPU40/x86 Ninja target rebuilt only the changed original DEM table,
re-archived `original-mvdm-dem.lib`, and re-linked
`original-softpc-process.exe` successfully.  Existing upstream warning C4201
and C4996 remained visible; no warning was suppressed.

## One permitted fixed-container observation

The one S14 run used the unchanged `O:\ntvdm` media and rebuilt product.  It
did **not** reach `50:42`; it exited with `0xC0000005` after the earlier
bootstrap markers through `54:05`.  Therefore this run is not a FastRead
success claim and does not test transferred CONFIG bytes.

The copied original exception record has loaded return `0x00a5d5a1` with
image base `0x009a0000`.  Its image-relative offset (`0x000bd5a1`) resolves in
the exact rebuilt map to `softpc.new/base/cvidc/accessfn.c::sas_overwrite_memory`.
That wrapper dereferences `Sas.Sas_overwrite_memory`; the current selected
generated C-VID vector leaves that slot null.  This is a pre-FastRead CCPU/
C-VID SAS-vector owner condition, not evidence against the S14 source route
or a reason to add another runtime observation.

## Status

S14 P1 has source, mapping and formal-link closure.  S14 itself remains open:
the one permitted product observation was preempted before `50:42`.  Any
runtime claim needs the existing CCPU/C-VID SAS-vector owner to resolve the
null `Sas_overwrite_memory` contract as a whole, followed by a separately
admitted observation; it must not be repaired as a FastRead or CONFIG leaf.
