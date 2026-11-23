# T484 S3 P69 mouse runners and obsolete Win101 producers

B0089/B0095 already delegated all compilation to CMD/native addon targets in
P1. Reconciliation confirms the remaining PowerShell/Python calls inspect NE
ABI and write independent test evidence; neither is a prerequisite of native
configuration, generation, compilation or product publication. Evidence copies
consume native binaries, never become build inputs. Retain both assertion
runners and their exact ABI checks; add owned background CMD PID/start/exit
records and a120-second deadline instead of synchronous untracked compilation.

Both runners passed: Win101 export ordinals1/2/3 and data segment2; Win31 NE
version, module flags, five exports including WEP, no imports, fixed resident
code/data. All18 evidence binaries (two authored drivers and16 authored probes)
match native outputs. Repeated native builds preserve hashes and timestamps.
The two driver hashes equal assets/release;11 host inputs and22 published host
hashes remain unchanged. No fresh Windows/guest interaction was run or claimed.

B0091/B0092 have no live code callers. They copy complete historical T434/T435
compiler caches, rewrite Ninja paths and select historical image identities or
runtime snapshots. Retire both rather than retain obsolete parallel build and
release recipes. Current CMD product/addon targets and current tests remain.
This removes no production source, test assertion or supported release target.

Raw evidence: build/t484-s3-mouse-reconcile. Concise receipts:
artifacts/build-migration/t484/s3-mouse-reconcile. Product/build inputs remain
unchanged; P67 cached full host build remains applicable, with new native addon
build/ABI/cache and publication hash checks in this delivery.

Fixed355 inventory:145 completed dispositions,210 remaining. Completed:
23 migrated,117 retired,four retained native entries,one standalone audit.
Remaining:51 pending producer chains,159 awaiting disposition reconciliation.
