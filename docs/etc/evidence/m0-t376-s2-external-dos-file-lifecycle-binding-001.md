# M0 T376 S2 — External DOS file-lifecycle binding closure

## Result

The selected original DEM file-lifecycle boundary is already present in the
current CPU40/x86 closure and passes its focused mapping contract.  The only
new build/package input is the immutable original `FASTOPEN.EXE` MZ workload;
it has been declared by the stage tool and verified in a new task-local stage.
No DOS loader, BOP provider, guest-media rewrite or host-native execution path
was added.

## Source and adapter disposition

- `demfile.c::demOpen` remains the original provider body and preserves its
  original pathname canonicalization, `CreateFile`, error and `AX:BP` handle
  result sequence.  Its registered `MVDM-HOST-DIV-195` change is limited to a
  bounded `mvdm_guest_location` pathname copy before that source flow.
- `demhndl.c::demRead` remains the original provider body and preserves its
  `ReadFile`/named-pipe/error/`CF:AX` sequence.  The same registered divergence
  leases its destination only for the synchronous call and commits only on
  original success.
- `demClose` and `demChgFilePtr` retain the original `AX:BP` handle form and
  resolve it through the single existing session host-resource identity
  manager.  MZ seeking is therefore an extension of the identical ownership
  contract, not another handle implementation.
- The original kernel-only fast-read path remains deliberately `CF=1` in the
  selected user-mode profile; unchanged guest `handle.asm` falls back to
  `SVC_DEMREAD`.  This is source-defined failure routing, not a slow-path
  substitute or a new provider.

## Verification

1. Existing task-local x86 fixture
   `build/M0-T365/S2/dem-file-lifecycle-x86/dem_file_lifecycle_lease_fixture.exe`
   exited `0`.  It proves one session-owned host-resource `HANDLE` identity,
   a bounded real-mode path copy, successful write lease/read/commit, and an
   invalid guest-range refusal without changing the prior payload.
2. `tools/build/Stage-OriginalSoftpcRuntime.mjs` now declares the selected
   original MZ `FASTOPEN.EXE` beside the pre-existing original `LOADFIX.COM`.
   A new task-local `build/M0-T376/S2/stage` was produced without overwriting
   any fixed runtime.  Its format-3 manifest contains 18 immutable media
   assets and records `FASTOPEN.EXE` with SHA-256
   `69dabbdb754b358ac4fe4b22de04c0e4c93076816f14bb0730caa9fd223996fc`;
   the staged byte hash matches.

## S3 handoff

S3 may make exactly one console-owning fixed observation for the already
staged immutable `LOADFIX.COM` workload.  It must use the current formal
CPU40/x86 product and current fixed `O:\\ntvdm64` stage; it may not reuse the
task-local MZ stage or vary a runtime input.  A successful `.COM` child and
original return only authorizes the separately frozen MZ observation in S4.
