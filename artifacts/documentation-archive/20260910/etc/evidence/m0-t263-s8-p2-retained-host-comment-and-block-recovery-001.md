# M0 T263 S8 P2 — retained host comment and block recovery

Date: 2026-08-24

## Question

Can the four currently composed OpenNT host sources retain their original
readable module/algorithm structure while remaining limited to the currently
connected provider blocks?

## Inputs

- `refs/opennt/base/mvdm/softpc.new/host/src/nt_error.c`
- `refs/opennt/base/mvdm/vdmredir/{vrinit.c,vrnmpipe.c,vrputil.c}`
- the matching four production files below `src/opennt-host/`
- formal Ninja roots `build/M0-T262-S5/001` (historical object check) and
  `build/M0-T263-S8/r001` (fresh focused fixture closure)

## Procedure

1. Restore the original module headers and retained routine documentation.
2. Restore the direct-access error body shape: per-thread category read,
   duplicate suppression, category record, two resource loads, formatting,
   then the host error dialog.
3. Keep only the external NT4 seams statement-local and registered as
   `HOST-DIV-012` through `HOST-DIV-020` and `HOST-DIV-022` through
   `HOST-DIV-024`.
4. Recompile the four changed host translation-unit object targets through the
   formal MSVC/Ninja configuration.

## Observations

- `nt_error.c` now retains the original direct-access-error local variables,
  comments, resource-load fallback and source ordering.  The NTVDM TLS slot
  and private resource table are the only calls redirected through
  `adapter-win32`.
- `vrputil.c`, `vrinit.c`, and `vrnmpipe.c` retain their original module-level
  descriptions and the routine documentation for the currently recovered
  content.  `vrnmpipe.c` explicitly retains the original JFN/host-handle
  explanation, while its VDD/ICA asynchronous product paths remain deferred.
- `vrinit.c` now spells its retained definitions as the original
  `VrInitialized`, `VrInitialize`, and `VrUninitialize`; a translation-unit
  local macro maps only the emitted static-composition symbols to the existing
  provider linkage names.
- `vrnmpipe.c` now uses the original `RtlZeroMemory` spelling through the
  same-shaped `adapter-win32` compiler facade; `nt_error.c` likewise retains
  its original `sprintf` and `strcpy` calls after the source-order resource
  adaptation.
- The formal object targets rebuilt at `2026-08-24 12:32` local time:
  `vrinit.c.obj`, `vrnmpipe.c.obj`, and `vrputil.c.obj`; `nt_error.c.obj`
  rebuilt again at `12:44`. No compiler diagnostic was emitted.
- The former project-only `HOST-DIV-021` cleanup in
  `VrTerminateNamedPipes(DosPdb)` has been removed. The mirror now keeps the
  original empty hook; the deferred asynchronous Redirector/VDD package owns
  any future process-level cleanup.
- A second control-flow comparison of `VrReadNamedPipe` and
  `VrWriteNamedPipe` restored the original event, pending-I/O,
  `ERROR_MORE_DATA`, `GetOverlappedResult`, empty-read and extended-error
  comments/order.  The only remaining omitted branches are cancellation-queue
  ownership and timeout-time raw-handle closure: those require the absent VDD
  asynchronous owner and the session handle-lifetime boundary respectively.
- The historical object check used its inherited graph only for the four
  translation units. A new formal root now links and runs the focused host
  mirror fixture; its result is recorded in P3. This packet still makes no
  claim for unadmitted mailslot, remote or VDD-owned Redirector features.

## Interpretation

The recovered source now follows the source-first form required by S8: original
comments and control/data/failure structure are retained for each connected
block, while unconnected complete product blocks are neither silently
rewritten nor presented as runnable.  Confidence is high for compilation of
the four translation units and limited to the existing admitted provider
surface.

## Follow-up

The focused S8 fixture closure is now recorded in P3. When a later
Redirector/VDD owner package is admitted, restore the deferred whole blocks
through their original interfaces rather than extending the current synchronous
helper.
