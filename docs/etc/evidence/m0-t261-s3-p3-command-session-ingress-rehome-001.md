# M0 T261 S3 P3 — COMMAND session ingress re-home

## Question

Does the native COMMAND session state machine belong to `opennt-host`, or is
it BOP/session composition that must be owned by `opennt-bop`?

## Change

- Moved `command_native_session_shim.{c,h}` to
  `src/opennt-bop/ingress/command_native_session.{c,h}` without changing its
  state layout, dispatch ABI, or control flow.
- Renamed its local include and redirected the runtime-session, generic-UD
  bridge, fixtures, and formal component manifest to the new owner/path.
- Corrected the moved header to include the still-live transitional COMMAND
  compatibility declaration through its component-qualified path.  This is an
  include-root correction only; it is not an imported-source divergence.
- Removed the obsolete `HOST-DIV-011` entry: the PIF layout bridge is now
  owned and registered by `opennt-bop` as `BOP-DIV-032`, while the re-rooted
  OpenNT PIF triplet has no body divergence in `opennt-host`.

## Rationale

The moved code consumes a generic `#UD` event, selects COMMAND invocation,
creates a call record, and produces a typed resume/stop result.  Those are
BOP ingress and one-session continuation semantics, not independently useful
OpenNT host capability.  It therefore implements the S2-ledger's required
`COMPOSITION` disposition for the former native-session shim.

## Verification

- Fresh formal graph: `build/M0-T261-S3/r003`.
- The exact target
  `obj/opennt-bop/src_opennt_bop_ingress_command_native_session_c.obj`
  compiled successfully; a second Ninja invocation reported `no work to do`.
- The two affected fixture objects (`t236-s2-native-cmdexec32` and
  `t231-s10-command-native-session`) compiled successfully; their repeated
  Ninja invocation also reported `no work to do`.
- `Test-T260S8ComponentManifest.ps1` passed: 291 production sources, 78
  fixtures, one formal target.
- `Test-T260S8ProductionTree.ps1` passed.
- A live-source/test/formal-manifest search found no remaining
  `command_native_session_shim` reference.
- `git diff --check` passed.

## Scope

This is a production ownership correction.  It neither enables a COMMAND BOP
nor alters its provider behavior.  The remaining mixed `command_misc`,
environment, configuration, and binary seams require their separately mapped
facade/composition splits before S3 can close.
