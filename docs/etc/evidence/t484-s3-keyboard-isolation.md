# T484 S3 P192: keyboard probe retention and isolation

## Decision and actual coverage

This is an independent focused regression, not a mandatory Full entry.
Retain the original COMMAND reader's unavailable/configured/malformed/absent
outcomes and the real PIF/MEM startup contract. Retire the private detached
observer: its only live caller assumed flat O:/winnt executables, wrote that
installation and globally killed named processes. The caller now shares the
existing console-startup observer, private desktop and owned stage cleanup.
No product source, mirror, original guest or release asset is changed.

The old mandatory keyboard-registry-hit assertion is obsolete. Current
compat/console_services.c calls the exact native Console layout query; it
explicitly has no thread-HKL replacement. Original cmdkeyb.c returns through
NoInstallkb16 on query failure. ADAPTER-WIN32-034 documents that boundary.
A real MEM launch cannot prove that the registry branch was selected. The
controlled-layout fixture still links the real cmdkeyb/shadow-registry objects
and must independently record QUERY_OK, QUERY_MALFORMED and QUERY_ABSENT.
No false success is inferred from its exit code alone.

## Runs and corrections

Owned evidence root: build/t484-s3-keyboard-isolation.
All runs use the existing modern pwsh normal-user-environment launcher and
private-desktop observer; no environment values are recorded.

- Initial child14864: MEM output and exit0, but obsolete required registry-hit
  assertion failed. This is not counted as a passing combined test.
- Recheck child25068: MEM passed; reader was incorrectly placed outside a
  system32 package directory, so LOAD_ABSENT defeated the query assertions.
  The test failed, as required. The deterministic result labels alone did
  not pass the gate.
- Final child27928, 2026-11-24 12:13:02–12:13:30 UTC: exit0. Real PIF/MEM
  outputs conventional memory and exits0. All four reader labels and all
  three query trace assertions pass after restoring the previously verified
  system32 fixture layout. Original guest bytes are unchanged.

The runner restores its environment and stops/removes only its own stage.
The remaining two fixture targets preserve production-object linkage and
PIF generation. The removed observer has no remaining live source/build
reference. Source-package cleanup and delivery results are recorded below.

## Scope and next work

B0040 now has native production and a verified scoped runtime caller. Its
shared physical-copy/manifest staging remains S4 work; it is deliberately
not counted as entire-chain completion. Frozen census remains316/355
with39 unresolved chains, plus19 supplemental preparation chains. The
migration count is not a Full-test count or a task completion percentage.

## Delivery verification

Cached full build child26024 completed at12:14:58 UTC, exit0 (60 seconds).
The delivery verifier checks all11 raw host images against InputSha256 and
all22 published copies against Sha256; all remain exactly identical. Existing
assets/release and O:/winnt/system32 publication is therefore retained, with
no needless overwrite or claim of newly changed product images. The final
reader test was run against this unchanged product identity.
The source-package copy was removed only after validating its absolute owned
build path and observing zero processes using it; its manifest is retained.
Governance and final Git delivery are recorded by the containing P commit.