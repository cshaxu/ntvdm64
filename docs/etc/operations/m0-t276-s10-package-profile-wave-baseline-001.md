# M0 T276 S10 package/profile dependency-wave baseline

## Result

All 23 selected MVDM package roots now have one source-root role, a non-final
original build/profile disposition, predecessor set and a candidate recovery
wave. The waves are derived from original package/build structure and the S8/S9
interface evidence; they do not authorize builds or select providers.

The detailed ledger is
[`m0-t276-s10-package-profile-wave-ledger.tsv`](m0-t276-s10-package-profile-wave-ledger.tsv).

## Candidate waves

- `W0-source-carriers`: `dirs`, `makefil0`, `inc`.
- `W1-support`: `oemuni`, `suballoc`, `vdmutils`.
- `W2-machine-boundary`: `softpc.new`, including its explicit Bochs-replacement
  exclusion review.
- `W3-monitor-memory`: `sim16`, `sim32`, `v86`, `xms.486`.
- `W4-host-service`: `dos`, `dpmi`, `dpmi32`.
- `W5-product-extensions`: `vdmredir`, `wow32`, `fax`, `vdd`, `ieuvddex`,
  `bde`, `dbg`, `vdmdbg`, `vdmexts`.

## Limit

This groups original packages for subsequent review. A `candidate_wave` is not
a build order claim: subpackage cycles, direct imports and architecture-specific
compilation are deliberately left to S11's normalized package/interface/build
ledger closure.
