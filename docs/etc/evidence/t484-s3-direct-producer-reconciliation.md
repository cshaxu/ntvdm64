# T484 S3 P199: reconcile remaining direct compiler commands

## Scope correction

The previous13-entry supplemental review is complete, but did not cover all
actual producer scripts. A broader literal CL/Add-Type scan of test/tools
finds30 additional unregistered paths, now added to the SAME disposition ledger.
These are review candidates, not30 proven producers. Do not infer global
completion from the frozen355 or any supplemental subset. Source review of
one candidate, test-wow32-build-migration.py, confirms it only reads Ninja
-t commands and compares existing binaries; retain as standalone audit.29 of
this new set remain to classify. Scan names are retained in the ledger and
build/t484-s3-direct-producer-reconciliation/scan.json.

## Actual changes

Retain native-hook packet assertions, unchanged C source. Shared CMD/CMake
build owns the existing x86/x64 sources, Unicode/Windows definitions and C11
profile; the PowerShell runner only invokes the producer and asserts results.
Both architectures report valid=4 rejected=13.

Retire Build-BrokerCallbackBarrier.mjs: its fixed T412 source
src/app/basesrv_entry.c no longer exists. It has no current tool/test caller;
only historical m0-t412-s10-final-verification.md describes its old use.
Preserve that evidence and the separate lifecycle runner. No claim that a
modern callback cancellation test was rerun or replaced by the packet test.

## Verification and limits

Owned background23864 runs build/t484-s3-direct-producer-reconciliation/run.ps1.
Both actual packet binaries pass. The same run requires cached full host build
and all11 raw/22 published identities. Governance and changed-script parsing
precede commit/push. No product, original guest, mirror or package changes;
exact image identity avoids redundant overwrite. No Full acceptance claim.

The original316/355 disposition and23 supplemental preparation/staging chains
are unchanged. S3 cannot close while these producer-review candidates remain;
S4 publication and S5 clean acceptance are still required.
