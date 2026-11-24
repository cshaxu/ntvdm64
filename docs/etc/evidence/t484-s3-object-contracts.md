# T484 S3 P92 shared SoftPC object contracts

B0153 keeps eight distinct current-object contracts in one existing assertion
runner. Retention review found no replacement for their scheduling/order,
I/O dispatch, floppy provider, key conversion, resource bounds, UMB ownership,
VDD controls and NIDDB instance-lifetime assertions. NIDDB allocation failure
is separately covered by the retained no-retry fixture, not duplicated here.
Remove unused historical WorkerBuild/SelectedObject overrides; current cache
and actual product objects are the only supported provider selection.

One native CMake helper defines eight positive links and eight independent
wrong-ABI compile targets. CMD softpc-contracts builds all positive outputs;
the runner invokes native negative targets and checks C2118, never emits CL
arguments or compiler scripts. Input identity, strict runtime markers and
NIDDB three-VM reuse/stale/out-of-range/active-stale controls remain. Original
guest, mirror and product code are unchanged. Compilation has a120s diagnostic
deadline and owned process cleanup; runtime limits remain30s.

First PID27144 failed in UMB: the old standalone profile omitted V7VGA and
selected an excluded SAS header branch. Actual nt_umb product COMPILE_FLAGS
include V7VGA; add it only to that fixture's positive/negative profile. Do not
change the original header or relax its exclusion. Corrected PID31136 exited0.

| Contract | Checks | Result |
| --- | ---: | --- |
| QEVNT | 37 | Pass |
| I/O | 70 | Pass |
| GFI | 433 | Pass |
| Keycode | 268322 | Pass |
| NLS | 3645 | Pass |
| UMB | 4240 | Pass |
| VDD | 305 | Pass |
| NIDDB | 15 | Pass; three-VM reuse and three invalid identity exits verified |

All eight wrong-ABI builds failed at C2118, as required. Total277067 checks.
These are controlled providers around real cached objects, not guest/full
lifecycle acceptance. UMB optional strict-bounds and VDD optional deinstall/
DMA-boundary modes are not covered by this runner's unchanged default matrix;
no new pass is claimed for those independent investigative modes.

[Receipts](../../../artifacts/build-migration/t484/s3-object-contracts/result.json)
retain run identity, outcome and hashes. Raw input/compile logs remain
build/t484-s3-object-contracts. One shared inventory entry is complete, not
eight new entries:187 complete/168 remaining of355. Retrospective retention
review and S4/S5 remain open.

Final gate PID23620 exited0: cached full host build, eight stable native
fixture hashes/mtimes, x64 group rejection2 and11 raw/22 publication hashes
passed. Product bytes are unchanged; existing publication was verified.
