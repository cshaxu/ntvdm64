# T484 S4 CMD addon utility publication

## Scope and ownership

P3 replaces `tools/build/Stage-AddonUtilityRelease.ps1` with
`tools/publish-addon-utilities.cmd`. It builds the native addon set and publishes
all nine AMD64 utilities from the canonical build tree. The five driver images
are not replaced by this entry; their bytes must match the existing release
manifest, and their qualification fields are retained unchanged.

CMake declares source hashes, fixed utility identities and the retained driver
JSON fragment. The generated CRLF CMD implementation performs native PE checks,
copies, hashes and publication. Authored CMD/CMake source remains LF. No Python,
PowerShell or JavaScript runs in this production publication chain. The
PowerShell test is an assertion runner, not a build or packaging provider.

The previous partial-utility publication interface is retired. Current tool
instructions use the coherent nine-utility publisher; no active script caller
of the retired entry was found. Mouse and Compat95 publication remain S4 work.

## Verification

Entrypoint: `test/build/Test-AddonUtilityPublication.ps1 -RunRoot <fresh-build-path>`.
It exercises the real CMD publisher, checks nine source and binary identities,
five driver identities and every retained qualification field. It also rejects
mismatched driver bytes and a missing destination manifest before touching the
existing package. A locked utility destination fails and leaves no completion
manifest. Publication is not claimed transactional: a failed copy can leave
partial files, but cannot retain a successful package manifest.

Initial run PID26216 failed because a string comparison treated JSON field
ordering as a metadata change. The corrected test compares all values and keys
recursively; PID28232 passed. The expanded delivery run PID3020 passed,
2026-11-24T15:40:45Z through15:42:06Z. The final run, after explicitly clearing
inherited publication state, is recorded under
`build/t484-s4-addon-publication/final` (PID28560, exit0, 15:42:39Z-15:43:59Z).
The first governance run rejected trailing whitespace emitted by CMake JSON
formatting. The generator now removes trailing whitespace; the `format` run
repeats the real publication assertions and release/hash checks without
rebuilding unchanged host code. This is a formatting correction, not a waiver.

The delivery runner executes the cached full host build, the production utility
publisher, all14 released addon hashes, and the existing11 raw/22 dual-published
host identity checks. No original guest or product source changes. No fresh
Full runtime suite is claimed, and existing runtime limitations remain open.

## Review and accounting

The publisher prepares and verifies every utility before invalidating the
previous completion manifest. Each destination copy is compared bytewise and
the new manifest is copied last. A failed publication removes that completion
record. It does not change driver acceptance from candidate to verified.

This disposes one original inventory item:325/355 disposed,30 pending, plus
24 overlapping preparation chains. Supplemental test coverage is not counted
as an additional migration completion. S4 and S5 remain open.
