# T484 S3 P193: native/DOS output probe preparation

## Retention and changes

Retain this focused reproducer: it tests simultaneous native output while a
real COMMAND batch is active, with independent ready/done gates. Ordinary
startup/exit tests do not replace this contract. It is not a direct Full entry.

The old caller expected a flat package, checked for all NTSRV processes, and
materialized a BAT with PowerShell replacements. It now uses the shared owned
physical stage/private-desktop observer and current system32 directory. The
native probe had concatenated the current directory without a separator;
it now starts COMMAND.COM and tests/CG.BAT relative to its explicit working
directory. The batch uses the same relative tests directory. CMake prepares
its CRLF output from the authored LF source; no interpreted generator remains.
No original guest, product source, release asset or winnt.zip changed.

## Actual outcome: runtime FAILED, not a migration pass claim

Owned root: build/t484-s3-native-output-isolation. Modern pwsh supervisor16324
launched child20412 with the existing normal-user-environment helper (no values
recorded). Run 2026-11-24 12:18:55–12:19:22 UTC ended1.

The fixture compiled. Native outer request returned37 and COMMAND returned0.
The native after-native-during snapshot contains NATIVE-DURING-DOS. After DOS
returns, the native Console snapshot and final frontend screenshot text lack
that marker; both contain DOS-RESUMED and NATIVE-AFTER-DOS. The strict runner
rejects the missing visible output and remains failing. No sleep, assertion
waiver or product change was added. This is a real observed overwrite, not
proof of a particular internal cause.

Historical m0-t423-s12-ntw32-backend.md also records a failing simultaneous
output check with different missing markers. That history does not prove
this occurrence has the same cause. Current architecture selects an ordinary
hidden Console, not the older ConPTY backend. Product investigation belongs
to a separately admitted scope; the strict current reproducer and retained
snapshots remain the entry point. TODO records this unresolved result.

B0034 stays pending for shared S4 physical/package staging. The prepared test
is runnable and reports the actual failure; it is not counted as functional
acceptance. Frozen census remains316/355;19 supplemental chains remain.

## Build and publication identity

Cached full child17016 completed12:23:10 UTC, exit0,61 seconds. All11 raw
host inputs and22 dual-location published hashes remain identical. Existing
publication is retained; no newly changed product package is claimed.
The authored BAT's native configured output is exactly its CRLF conversion,
with no remaining substitution token. Package-copy cleanup verified the owned
absolute build path and zero live owners before removal; the manifest and
strict failure snapshots remain. Governance is checked before the P commit.
## Expanded census follow-up

An expanded CL/NASM/Add-Type scan found13 additional unclassified files.
Their exact paths are recorded in the existing dispositions ledger under
supplemental-producer-review. These are review candidates, not thirteen proved
required tests. Retention and actual build effect must be established before
retirement or migration. They are distinct from the19 already migrated
supplemental producer chains awaiting preparation/staging.
