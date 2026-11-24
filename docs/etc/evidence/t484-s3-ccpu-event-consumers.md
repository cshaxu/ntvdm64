# T484 S3 P210: current CCPU event consumers

The old Verify-CcpuEventConsumption.mjs still contains useful event-consumption
assertions, but its compiler/source-generation recipe predates the selected
PROD guard and current hardware-interrupt callback. Retain the behavior checks;
do not preserve historical-revision rebuilding as a parallel build system.

The existing cpu-tests.cmake now uses the shared SourceProjection helper for
four current c_main.c consumer blocks. Atomic helpers and masks reuse the
existing notification_body.inc producer. The authored fixture lives beside
CCPU tests; no production or original guest source changes. The current PROD
profile excludes the unsupported debug branch. Local wait/PIC/timer/hook stubs
are explicit: this is not a full CPU/PIC or guest runtime acceptance.

Preserved assertions: HALT observes RESET without consuming it, RESET occurs
once, unrelated timer state survives RESET, timer/SAD consume once, enabled
IRQ wakes HALT without premature consumption, IF=0 does not acknowledge IRQ,
negative acknowledge does not deliver, and reassertion during acknowledge is
not lost. The current host hook is tested both unhandled and handled, with no
second dispatch when handled. Empty event waiting is not exercised here.

Initial owned runs PID6416 and17608 rejected ambiguous source boundaries at
configuration, before compilation. The shared extractor deliberately requires
unique endpoints; HALT's following PIG boundary and IRQ's original diagnostic
boundary now make both exact. No parser relaxation or product fix was needed.
Final owned PID19576 started2026-11-24T14:06:50.8345781Z using
build/t484-s3-ccpu-event-consumers/run-verified.ps1. Fixture assertions pass;
result-verified.json records the full-build/publication-identity exit. The run
requires cached full host build and11 raw/22 dual-publication hash checks.
Unchanged images are verified rather than overwritten. closure-result.json
records tracker/documentation/diff checks before commit and push.

One supplemental direct compiler candidate remains, the historical Run16 entry
audit. Frozen316/355 and23 supplementary preparation/staging chains remain.
No S3, S4/S5 or previously failed simultaneous-output closure is claimed.
