# M0 T347 S2 — Scalar-location adapter closure

## Question

Can the existing session guest-memory lease machinery carry the complete
three-location COMMAND/DEM/SoftPC scalar contract without exposing a durable
guest pointer or creating a second mapper?

## Implementation

`adapter-mvdm-host-out/softpc/mvdm_command_guest_state` now records, atomically
with the existing SCS location, numeric real-mode locations for:

- `SCS_ToSync` (byte);
- `SCS_Is_Dos_Binary` (byte); and
- `SCS_FDACCESS` (little-endian word).

The adapter exposes only source-required scalar operations: byte write, word
read/write, and modular word add. Every operation reacquires a session guest
memory lease at the access point and releases it before returning. The state
also captures the session epoch and active guest-memory epoch at registration;
later cross-session, inactive-memory and restarted-memory access is rejected.

`mvdm_command_guest_state_set_scs` remains source-compatible for the existing
partial `SCSINFO` caller and intentionally marks the two new scalar locations
invalid. S3 alone will switch the original `cmdSetInfo` call site to the new
all-location registration API and migrate original consumers.

## Verification

Focused x86 fixture, built and run from the disposable task root:

```text
cmd.exe /d /c call O:\repos.hobby\ntvdm64\build\M0-T347\S2\run-command-scalar-state-fixture.cmd
```

Observed result: exit code 0. It proves byte write, word little-endian
read/write, `WORD` wrap from `ffff + 1` to zero and back via modular decrement,
cross-session rejection, inactive-memory rejection, restarted-memory epoch
rejection and out-of-range two-byte rejection.

Formal selected CPU40/x86 product link:

```text
cmd.exe /d /c call O:\repos.hobby\ntvdm64\build\M0-T343\S3\record-x86\run-ninja-parallel.cmd original-softpc-process.exe
```

Observed result: `mvdm_command_guest_state.c` rebuilt, `softpc-bindings.lib`
rebuilt and `original-softpc-process.exe` relinked successfully. Existing
warnings from original COMMAND mirrors remained visible; this S neither
suppresses nor changes them.

## Interpretation

The selected same-shaped adapter seam is sufficient for the scalar mechanism.
It retains fixed-width guest data and original modulo arithmetic, while making
session/memory lifetime explicit. It does not establish any original caller as
migrated; the raw `pIsDosBinary` and `pFDAccess` aliases remain intentionally
present until the all-consumer S3 migration.

## Follow-up

T347/S3 must migrate the original `cmdSetInfo`, COMMAND, DEM and SoftPC
retention/dereference sites as one source-ordered cohort, then delete those
two raw durable aliases. It must register each mirror divergence and prove
that no selected consumer retains a guest pointer.
