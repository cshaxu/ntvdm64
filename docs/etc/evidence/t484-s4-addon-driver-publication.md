# T484 S4 unified addon driver publication

## Scope

P4 extends the existing CMD utility publisher into `tools/publish-addons.cmd`:
all5 authored drivers and9 AMD64 tools use one staging, copy, hashing and manifest
owner. The previous utility-only name and the two PowerShell driver publishers
are removed, not retained as competing implementations. No original guest
binary or product source is changed.

CMake declares source identities and two metadata fragments per driver.
CMD compares current source/tool receipts with those captured by the actual
build, hashes the staged driver and checks its build receipt. Only unchanged
source and binary identities select the prior acceptance/evidence fields.
Changed identities select candidate metadata without stale scope/evidence.
Compat95 includes its three source hashes. Utilities retain native AMD64 PE
validation. Every file is prepared before any published file is replaced.

Publication does not grant new verified status. The real four-case Compat95
runtime assertions remain in `test/integration/verify-win95-bootlogo.ps1`;
reviewed identity-matching runtime evidence is required before upgrading
qualification. There is no publisher switch that bypasses that requirement.

## Observations and tests

`test/build/Test-AddonPublication.ps1` exercises the real CMD entry in a fresh
build-owned output. It checks all14 image hashes and source/qualification fields;
rejects a changed authored driver with an unmatched build receipt before touching
the output; then supplies the matching test receipt and proves the changed image
is a candidate with no stale evidence. The test restores the build image and
receipt in finally. It also rejects missing output metadata and exercises a
locked destination, which must fail without a successful completion manifest.
These controlled image mutations are test-only authored build copies, not guest
modifications or behavioral driver tests.

Run records are under `build/t484-s4-addon-drivers`:

- Initial PID16668 failed copying because the expanded filename list still had
  the utility-only `.exe` suffix appended. Fixed in the single copy loop.
- PID8236 found a real historical Compat95 source-hash mismatch. The test now
  checks the current source hashes and candidate rule instead of demanding stale
  metadata preservation. The binary itself is unchanged.
- PID17396 passed all publication assertions,15:51:45Z-15:52:01Z on2026-11-24.
- `delivery/launch.json` and `delivery/result.json` retain final cached full
  build, publication assertions, official addon publication,14 released hashes
  and11 raw/22 dual-published host hash checks, with captured logs.

The old Compat95 primary source hash was
`FB198CB2B0D4D74764B22DBB010A7C12577D0F2AC5EAD4265D4E2D292CD5FF11`.
The new manifest records the current source hash from the native build inputs;
Compat95 remains candidate. No fresh Full runtime pass is claimed.

## Review and limits

The publication failure contract remains explicit: partial file replacement is
possible, but a failed copy removes the completion manifest. This is not an
atomic directory replacement or automatic rollback. Existing qualification is
not proof of a newly built, changed artifact. Driver build inputs/tool receipts,
artifact identity and runtime acceptance remain separate facts.

Current inventory becomes327/355 disposed,28 pending, plus24 overlapping
preparation chains. Shared physical-stage preparation and its consumers remain
S4 work; final clean interpreter-free acceptance remains S5.
