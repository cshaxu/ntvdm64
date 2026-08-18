# M0 T229 S1 — DEM Source/ABI/Failure Audit Plan

## Question

Before any new DEM implementation, can every original `50:00..48` service be
bound to its OpenNT owner/source, callable ABI and failure rule, current
bx-vdm route, Direct/Readonly disposition and recovery-rung action?

## Inputs

- `src/opennt/base/mvdm/dos/dem/demdisp.c` and its original DEM callees;
- `src/opennt/overlay/base/mvdm/dos/dem/` only where the tracked OpenNT
  overlay is the recorded source input;
- `src/opennt/base/mvdm/inc/bop.h` and `BOP.INC` selector/service definitions;
- `src/bx-vdm/bx_ntvdm_bop_ingress_v1.*`, provider registry/catalogue, DEM
  plane/package/provider routes and mutation-profile consumers;
- T200/T225/T226/T227 ledgers and T228 P30 evidence.

## Procedure

1. Extract the original `DemDispatch` service table and enumerate its 73
   canonical service values without inferring ownership from trace order.
2. For each service, record original caller/callee, fixed register or copied
   guest-memory ABI, dispatch order and observable failure rule.
3. Inspect the bound local route from ingress through plane/provider/composition
   code; classify it as original reuse, smallest source-derived seam,
   registered external intrusion, or newly authored behavior.
4. Record Direct and Readonly outcome separately, including explicit original
   unavailable/deferred behavior.  Identify retained Virtual code and Overlay
   ABI paths without implementing either profile.
5. Reconcile cardinality and duplicate ownership, then derive the complete S2
   package scope and whole-family regression matrix.

## Result form

The result will be an indexed ledger plus a concise narrative.  It will not
claim provider completion merely because a typed route exists, and it will not
run native guest trace as a service-selection mechanism.
