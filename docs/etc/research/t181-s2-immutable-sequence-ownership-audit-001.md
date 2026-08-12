# T181 S2 immutable sequence ownership audit 001

Date: 2026-08-12  
Packet: M0 T181 S2  
Disposition: design/audit complete; a contained implementation decision is justified, but no implementation is admitted here.

## Current one-target closure

The current `v4` profile has one mandatory `target` component, one
`guest_target_placement`, and exactly four search-metadata identities.  The
CLI accepts exactly one DOS target, checks it against that component, creates
one `byob_launch_declaration_v1`, and passes its kind/tail in two explicit
child-environment entries.  At installation the adapter loads one target
image, validates that the environment declaration matches its name, appends
one target to the immutable namespace, and creates a `cmd_get_next` state
with a `delivered` boolean.  Its service then hard-codes `TARGET.COM` /
`TARGET.EXE` and namespace file slot 3.

These are one coherent prior product closure, not defects to patch in place.
They identify the exact replacement set for a sequence provider:

| Current owner/surface | One-target assumption | Sequence replacement responsibility |
| --- | --- | --- |
| `byob_profile.[ch]` | `target`, target placement and four metadata records | Parse/validate a closed v5 two-slot declaration and copy a bounded slot array. |
| `ntdos64-run.c`, target selection and launch declaration | One caller target and two environment fields | Build the complete immutable launch plan once; user arguments apply only to slot 0. |
| adapter installation | One image and one launch object | Identity-load all declared slot images and reject any plan/profile mismatch before install. |
| boot namespace provider | `append_target`, single target metadata/file slot | Materialize both declared slots with unique fixed DOS paths and immutable metadata. |
| COMMAND provider | `delivered` boolean and fixed target path | Implement the S1 `next_slot`/pending/commit state machine over one slot descriptor. |

`BYOB_PROFILE_MAX_COMPONENTS == 16` is already sufficient for the two new
profile components, but no existing capacity or file-array size is relied on
as an implicit contract.  The implementation declares its own explicit
two-slot maximum and tests it.

## Proposed closed v5 declaration

The only initial v5 profile identity is
`ntdos64-byob-profile-v5` / `nt4-en-us-command-smoke-v5`.  Older profiles
remain accepted by their current contracts and never acquire sequence
behavior.  A v5 record has all current v4 core fields plus:

- exactly one `target` component, still restricted to `TARGET.COM` or
  `TARGET.EXE`, and exactly one `terminal-quit` component named `QUIT.COM`;
- `guest_declared_targets`, an ordered array of exactly two records:
  `target` at its existing canonical target path followed by `terminal-quit`
  at `\\QUIT.COM`, both on the COMMAND drive;
- an empty literal tail for `terminal-quit`; the sole caller-supplied ASCII
  tail belongs to `target` and retains the existing 118-byte maximum;
- exactly five immutable search-metadata records: `command`, `target`,
  `terminal-quit`, `config`, and `autoexec`.

No optional slot, duplicate role/path, `BAT`/`PIF`, relative path, directory,
wildcard, caller-provided DOS path, nonempty terminal tail, unrecognized
component name, or different terminal identity is valid.  The terminal's
three-byte identity is the S1 hash, not merely a filename convention.

The runner continues to own the process-local handoff, but it must replace
the independent `LAUNCH_KIND`/`LAUNCH_TAIL` pair with one versioned bounded
launch-plan serialization.  That serialization contains only slot order,
the selected target kind and copied tail; it names no host path and has no
queue operation.  The adapter cross-checks every serialized field against the
already profile-validated selection before it loads any guest image.  This is
a transport of a fixed declaration, not an ambient environment capability.

## Rejection and lifecycle matrix

| Case | Required result |
| --- | --- |
| Missing/extra/reordered slot, role or metadata record | Profile rejected before engine launch. |
| Wrong path/drive, duplicate DOS path or target/QUIT identity mismatch | Profile rejected before image load. |
| Terminal image wrong size/hash, or nonempty terminal tail | Profile/plan rejected before install. |
| Caller target does not equal the selected `target` component | Runner rejects before child process creation. |
| Plan absent, malformed, mismatched or contains a second caller command | Adapter rejects installation. |
| First `54:01` | Commits only slot 0. |
| Second ordinary `54:01` after slot 0 commits | Commits only slot 1. |
| Re-entrant or duplicate completion before queue acceptance | No slot advance and no duplicate write transaction. |
| Third/exhausted or malformed request | Decline, no state/CPU/memory change. |
| `C4 C4 FE` after slot 1 | Existing controlled-stop behavior; no `54:11` or DOS-status inference. |

## Admission boundary

S1 and this audit define one bounded implementation unit: a v5 profile/plan
reader, a two-slot immutable namespace input, and a replacement COMMAND
sequence provider.  It may reuse the existing copied gather/multi-write
transport and controlled-stop service, but may not alter their generic
mechanics.  It must be independently unit-tested first; fresh engine relink
or guest runtime observation requires a separately approved S3 decision.
