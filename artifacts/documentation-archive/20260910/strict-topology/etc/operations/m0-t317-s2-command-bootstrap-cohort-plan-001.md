# M0 T317 S2 — COMMAND Base VDM bootstrap cohort recovery

## Selected original cohort

`cmd.c`, `cmddata.c`, `cmddisp.c`, `cmdmisc.c`, `cmdenv.c`, `cmdconf.c`, and
`cmdkeyb.c`, exactly as selected by the original COMMAND `sources` manifest.

## Required recovery order

1. Reconcile each original `GetNextVDMCommand` use with the bounded Base VDM
   local protocol: request record, capacity result, retry/re-entry input and
   source-shaped failure result.
2. Reconcile every guest pointer access in the cohort with the existing
   session-owned SoftPC mapping lease; preserve direct original data copying
   only inside the lease lifetime.
3. Formally compose all seven original units with their common `cmddata` state
   and original dispatcher; do not replace any table entry.
4. Add focused bootstrap tests for original request/capacity/retry and one
   source-shaped unavailable console/key-layout outcome.

## Explicit transfers

- `cmdexec.c` and `cmdexit.c`: S3 child lifecycle.
- `cmdredir.c` and `cmdpif.c`: S3 local child endpoints; Redirector/PIF
  product branches remain later owner packages.
- DEM and guest program execution: later T317 cohorts and queue packages.
