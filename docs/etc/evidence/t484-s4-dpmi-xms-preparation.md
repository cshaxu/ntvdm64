# T484 S4 DPMI and XMS preparation

P15 completes three supplemental preparation chains using existing CMD owners.
DPMI debug stages WATCH/LIFE directly from the native cache; INT21 and XMS use
prepare-test-inputs.cmd for required DOS names and TestFiles for physical-stage
placement. Remove redundant interpreted copies and directory creation. Net
source reduction is8 lines; no new tool, product source or original guest
change is introduced.

XMS GO.DAT remains a runtime test handshake: it is written only after the
probe signals ENTER.DAT and the runner holds the exact worker handle. This
is test interaction, not configuration, compilation or package preparation.
Original failure ownership and non-return assertions remain unchanged.

## Verification

build/t484-s4-dpmi-xms records owned PID6748,
2026-11-24T17:48:15Z-17:51:47Z,exit0, using the existing modern pwsh/Userenv
runner. Cached full build passes. Real DPMI debug delivery/removal and two
normal handle exits pass; original DOSX status-query inversion remains outside
this witness. INT21 low/high buffers each pass direct and nested COMMAND routes,
including data/state, EOF/chunking and temporary-file removal.

All four real XMS cases pass: Boundaries, Existing, InvalidRange and
InvalidDescriptor. The fault cases assert worker ERROR_INVALID_ADDRESS487,
no guest AFTER.DAT, and the distinct unfinished-launcher broker result1067.
These are expected negative contracts, not silently ignored failures.

Final11 raw host inputs and22 dual-published hashes are unchanged. No changed
product image, new Full regression or original guest rebuild is claimed.
The existing CMD copy/path negatives from P14 and physical-stage contracts
remain applicable because their implementation is unchanged.

## Accounting

The frozen census remains343/355 disposed,12 pending. These three entries
belong to the supplemental preparation ledger:24 pending becomes21. They are
not added to, or subtracted again from, the355 original paths. S4 continues;
S5 clean interpreter-free acceptance is still outstanding.
