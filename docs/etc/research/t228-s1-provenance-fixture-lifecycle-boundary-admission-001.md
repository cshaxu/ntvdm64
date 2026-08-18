# T228 S1 P27 — Explicit-Provenance Fixture Lifecycle Boundary Admission

## Finding

P26 generated a fresh MSVC x64 `/MT`, CPU5 explicit-provenance graph and
linked `t228-s1-budget-terminal-position-fixture.exe`.  The fixture exited
with `0xC0000005` before any immutable guest run.  Its final synthetic
CS-transition wrap test runs after `bx_ntvdm_machine_stage_v1_reset`; under
`BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE=1`, each artificial
transition invokes the already registered ordinary-RAM provenance copy.  No
machine remains active at that point.

This is a fixture lifecycle error.  It does not show a guest, BOP, provider,
CPU, or provenance ABI defect.  The separate P1 fixture exercises the same
explicit-provenance copy while the minimal machine is active and passed in
both its source-recorded full CPU5 configurations.

## Selected Repair

The budget-terminal fixture's synthetic transition-ring wrap check is a
scalar-history check.  It will remain enabled in its default/scalar graph and
will be excluded only when the explicit provenance macro is on.  That avoids
inventing inactive-memory semantics for a test that does not test provenance.
P1 retains the positive provenance lifecycle coverage.

No product source, Bochs source, ABI, host capability, guest image, or
runtime behavior may change.
