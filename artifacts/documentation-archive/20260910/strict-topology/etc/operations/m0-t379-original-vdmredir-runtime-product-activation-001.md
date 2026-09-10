# M0 T379 — Original VDMREDIR runtime product activation

## Purpose

Turn the already closed T372 source/binding baseline into the first actual
original Redirector product route.  T372 deliberately did **not** package
`VDMREDIR.dll`; consequently its dynamic `MS_bop_7` route retained the
original unavailable result and cannot be described as Redirector runtime
completion.  T379 owns that precise remaining product boundary.

## Fixed architecture

- Original `mvdm-host/vdmredir` source remains the sole provider.  The task
  must not static-link `VrDispatch` into `nt_bop.c` or create an adapter
  Redirector.
- `adapter-mvdm-host-out/redir` provides only same-shaped historical imports;
  `session` owns host resources, handle identities and checked guest-memory
  leases; CPU40/SoftPC remains the only machine.
- The deliverable is an original `VDMREDIR.dll` carrying its original entry
  and exports.  Public modern Win32/NetAPI is permitted only behind the
  existing same-shaped boundary.  CSR, private RPC, RAP, NetBIOS/DLC and
  downlevel-only routes retain source-shaped unavailable behavior.
- Guest media stay immutable.  No trace-selected `57:xx` provider, second
  broker, CPU30, Bochs or x64 recovery is in scope.

## Ordered subtasks

1. **S1 — Product admission and whole export/import rebaseline.** Reconcile
   T372's original source list, `VDMREDIR.dll` entry/ten exports, every parent
   import, current formal graph and one declared immutable local-resource
   workload.  State exactly which original owner group blocks activation.
2. **S2 — Original DLL product and parent loader closure.** Formally compose
   the complete original DLL through its original `.def`/entry/export surface
   and retain `nt_bop` dynamic `LoadVdmRedir` behavior.  Every unresolved
   import receives one same-shaped source-first disposition.
3. **S3 — Local resource/lifecycle owner closure.** Complete the original
   local file/pipe/mailslot/completion owner groups selected by S1, including
   stale/cancel/disconnect directions.  Public network groups remain only
   where their complete original contract is composable.
4. **S4 — Formal activation matrix.** Link the final CPU40 product beside the
   original DLL and selected immutable workload; prove normal loader and
   source-shaped failure paths without a product run.
5. **S5 — Frozen local-resource observation.** Make one bounded non-debug
   observation of the declared workload and classify the first original
   Redirector result or an earlier owner boundary.

## Completion standard

T379 closes only with an original `VDMREDIR.dll` product identity, complete
export/import and owner disposition, formal CPU40 binding, and one fixed
local-resource observation.  It does not claim remote networking or arbitrary
`57:xx` compatibility.
