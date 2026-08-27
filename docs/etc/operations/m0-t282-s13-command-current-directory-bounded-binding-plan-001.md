# M0 T282 S13 — COMMAND current-directory bounded mirror binding

## Question

Can the original `cmdGetCurrentDir` body write its selected current-directory
result to guest `DS:SI` without retaining the historical unbounded
`GetVDMAddr` pointer?

## Source-first disposition

1. **Original source:** retain `opennt-mvdm-host/dos/command/cmdmisc.c`, its
   original ordering, OEM environment lookup, drive validation, CF/AX result
   and root-directory fallback.
2. **Smallest same-shaped seam:** S12's `mvdm_guest_location` numeric
   descriptor and exact write lease replace only the unsafe pointer lifetime.
3. **External intrusion:** not applicable.
4. **New behavior:** not applicable. The temporary host buffer is a bounded
   transport for an original result, not a new current-directory policy.

## Exact contract

S9 proved that this output is at most
`MAXIMUM_VDM_CURRENT_DIR + 3` bytes (67 bytes in the selected source). The
source first performs its original drive and environment branches, then a
fresh descriptor write copies the exact NUL-terminated output. A descriptor
or lease failure cannot expose a pointer and takes the existing `CF=1,
AX=0` output-failure result before root-fallback environment mutation.

## Scope and exclusions

This packet changes only `cmdGetCurrentDir`. `cmdComSpec` (bounded DS:DX
input) and `cmdGetInitEnvironment` (source-calculated ES:0 output) have
separate S9 records and are not enabled or modified here. No BOP ingress,
host-drive policy, session mapper, machine mechanic or asynchronous work is
admitted.

## Evidence and closure

The original COMMAND static source graph must compose on x86 and x64, the
S12 bounded-lease fixture must remain positive/negative passing on both
architectures, and the mirror README must register the one divergence.
This is source-composition evidence only; it does not claim a callable
`54:04` guest route.
