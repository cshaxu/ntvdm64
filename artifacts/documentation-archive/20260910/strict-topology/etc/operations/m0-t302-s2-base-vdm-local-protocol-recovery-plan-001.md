# M0 T302 S2 — local Base VDM protocol recovery plan

## Selected source route

`opennt-host/base/win32/client/vdm.c:GetNextVDMCommand` retains the source
ordering for request setup, required-size feedback, wait/retry and last-error
result. A named `adapter-mvdm-host-out/basesrv` subfamily supplies only the
reached local capture/dispatch and BaseSrv record operations. The selected
server logic is the DOS-only sections of
`opennt-host/base/win32/server/srvvdm.c:130`; its CSRSS product shell is not
compiled.

## Delivery sequence

1. Add the `basesrv` adapter subfamily with copied fixed-width local request,
   result, record and event ownership. Its public edge is the historical
   client-facing function/structure form; no generic CSR API is exposed.
2. Preserve the original client path through a same-shaped capture/dispatch
   facade. Any required mirror hook is one documented `DIVERGENCE:` boundary;
   source algorithms do not move into app.
3. Rehost the reached DOS server copy/capacity/environment/no-command/reentry
   ordering in the adapter, recording every retained original block and every
   unavailable WOW/PIF/child/global branch.
4. Replace the app command-source protocol with a producer-only declaration
   input and remove the monitor command-provider callback route.
5. Add focused x86/x64 fixtures. Only after those pass, add the selected
   sources to the formal Ninja manifest and run its targeted graph.

## Fixed limits

All asynchronous data is a copied session-owned record. A `VDMINFO` pointer,
native HANDLE, capture pointer or app payload pointer never survives a call or
is stored by a worker. Cross-process transport, BaseSrv global discovery,
WOW/PIF, `ExitVDM` and normal child completion remain explicit later owners.
