# T202 S2: DEM session CWD profile context

## Question

Can all DEM current-directory/default-drive work share one profile-governed
session object before any `50:13`, `50:18`, or `50:1A` leaf is connected?

## Inputs

- OpenNT `demdir.c` (`demQueryCurrentDir`, `demSetCurrentDir`) and `demgset.c`
  (`demSetDefaultDrive`);
- T201 shared mutation profile and overlay lifecycle;
- T202's admitted-root directory validation primitive.

## Procedure

`bx_ntvdm_dem_cwd_context_v1` owns per-drive, root-relative CWD state and the
default drive. It registers and resolves only the DEM `session context` class:

| Profile result | CWD set/default-drive result |
| --- | --- |
| direct | validate beneath the admitted root and update session-private state |
| readonly | explicit rejection |
| overlay | update the common bounded overlay record |
| virtual | update separate virtual session state |

No branch invokes `SetCurrentDirectory`, creates a virtual boot volume, or
exposes a host path/handle. A new generic overlay `replace` operation preserves
the existing append-only `record` operation while allowing a legitimate
stateful capability to update its already-authorized key (for example repeated
`CD`).

## Observations

Fresh MSVC x64 `/MT` probes passed:

- `t201-s2-mutation-overlay-r3`: record remains non-overwriting; the new
  authorized replace operation updates one owned key and retains teardown and
  cross-owner rejection;
- `t202-s2-dem-cwd-context-r4`: direct selected-root validation, readonly
  rejection, overlay update, virtual state, malformed-relative-path rejection,
  and default-drive state all passed.

The CWD fixture uses a temporary directory and user-mode root handle only; it
does not execute a guest or modify system configuration.

## Interpretation

This establishes the shared package mechanism, not BOP closure. The existing
leaf recognizers still need to be replaced together with copied guest-memory
CDS/path ABI and source-shaped error results. The profile object, rather than
a selector or trace hit, now decides all four capability outcomes.

## Follow-up

Bind this context to the DEM session lifecycle; then implement the complete
current-directory/default-drive family and its package regression against
`demdir.c`/`demgset.c`.
