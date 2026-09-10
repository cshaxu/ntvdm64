# M0 T261 S8 P5 — remaining VDMREDIR source-owner intake

## Question

Which remaining original VDMREDIR units must replace the temporary
selector/session composition before S8 can claim source-fidelity closure?

## Result

- `vrdisp.c` is the original 0x00–0x31 BOP-service table and its `VrDispatch`
  register/service selection.  It belongs in `opennt-bop/vdmredir/`, not
  `opennt-host`: dispatch and service meaning are BOP ownership even though
  the historical product put them in VDMREDIR.DLL.
- `vrmslot.c` contains both public Win32 mailslot calls and direct CCPU/SAS
  register/guest-pointer operations.  Its guest-facing BOP bodies therefore
  also belong in `opennt-bop/vdmredir/`; its machine-shaped imports must be
  provided by `adapter-softpc`, and its Win32 calls by `adapter-win32` only
  where the historical API spelling is unavailable.
- The already re-rooted `opennt-host/vdmredir/vrnmpipe.c` and `vrinit.c`
  remain the host-only reached helper subset.  Their remaining historical
  VDD/ICA/DLC completion product shell is not silently recreated.

## Required recovery order

1. Import the original `vrdisp.c` table and a minimal same-shaped dispatch
   facade in `opennt-bop/vdmredir/`; retain unsupported service failure until
   each original owner is composed.
2. Import `vrmslot.c` with its original record/list/16-bit handle allocation
   order.  Bind host `HANDLE` lifetime through the existing session handle
   manager, never through a new family-private mapper.
3. Recover only the CCPU/SAS accessors reached by those original bodies in
   `adapter-softpc`; recover public mailslot calls through `adapter-win32`.
   VDD callback broker, ICA interrupt ownership, DLC/NetBIOS and remote
   Redirector providers remain explicit source-owner limitations until their
   own original interfaces can compose.
4. Delete the overlapping self-authored mailslot/dispatch bodies from
   `opennt-bop/ingress/redir_native_session.c` only after source-shaped local
   regression proves their replacement.

## First mechanical prerequisite

`adapter-softpc/opennt_ccpu_sas_facade.h` now exposes a neutral alias surface
over the existing call-scoped CCPU/SAS implementation.  It introduces neither
a second call record nor a second guest-pointer/handle mapping manager.  The
Redirector fixture statically verifies the alias has the identical call-frame
size and passes its existing lifecycle regression.  This is only a mechanical
precondition; it does not claim that either original `vrdisp.c` or
`vrmslot.c` is composed yet.

## Dispatcher recovery progress

The reached `vrdisp.c` service-table structure is now compiled at
`opennt-bop/vdmredir/vrdisp.c` and is the actual `57:xx` routing source:
the existing ingress binds one copied frame and calls `VrDispatch(service)`.
The original 0x00–0x31 order and range rejection are retained.  Each table
entry presently forwards to the existing bounded service body through the
single synchronous callback; this is `BOP-DIV-056`, not a claim that all 50
original provider bodies have been recovered.  Fresh formal r009 compiled and
linked the target fixture in 312/312 edges, and that fixture passed.

## Non-conclusion

The passing P4 fixture proves only the re-rooted named-pipe helper lifecycle
and the current bounded composition. It does **not** prove `vrdisp.c` or
`vrmslot.c` direct-source composition, so S8 remains active.
