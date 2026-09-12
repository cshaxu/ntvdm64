# M0 T405 S1 initial audit baseline

## Question and procedure

Establish comparison coverage before counting semantic replacements.
On 2026-09-12 inspected Git status/HEAD, CURRENT, Queue, the proposal, and
tracked membership using git ls-files grouped by source root. Static audit
only: no product execution, source repair or runtime deployment.

## Initial observations

Starting HEAD: `3d127962c82133cb6adad50f52733250bac5f11a`, clean worktree.
Ancestor `1daff0ace` preserves unaccepted research code: clean Git state is
not functional acceptance.

Independently read deployed O:\ntvdm64\ntvdm32.exe with Get-FileHash:
SHA-256 `27f8d7b5bb074838e9484877954a38dcbcfa0be64282a4a2d05c776867655107`.
This identifies the retained runtime artifact, not a rebuild of current HEAD
or a fresh runtime regression result. No deployment was changed.

| Root | Tracked files, all kinds |
| --- | --- |
| mvdm-host | 1977 |
| mvdm-host-overlay | 8 |
| mvdm-softpc-patch | 6 |
| opennt-host | 20 |
| adapter-mvdm-host-out | 112 |
| adapter-opennt-host | 1 |

These are initial membership counts, not executable-file counts, line diffs,
hack counts or the final audit denominator. App/session, ABI declarations,
build selection and provenance still need reconciliation. No tracked
opennt-host-overlay result does not imply absent non-MVDM replacements;
those bodies can be in other roots.

## Follow-up and completion limitation

Reuse existing provenance/selection ledgers, resolve each selected upstream
file, separate declarations/generated/data inputs and map actual providers
before computing upstream diffs and replacement totals. Record runtime
hashes independently. The initial seven findings are leads, not exhaustive
coverage. S1 remains active: admission and this initial record do not close it.
