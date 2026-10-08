# M0 T437 — installed Windows 3.1 portable launch closure

## Delivered scope

T437 delivers two deliberately separate, installed-tree tools:

- `tools/win31-launch\APPLY.CMD` prompts once for an installed Windows 3.1
  root and creates its installation-local `PATCH` compatibility payload. CMD
  owns the recovery copies, mouse replacement, profiles and configuration;
  `PATCH386.EXE` owns only the two identity-bound `KRNL386`/`WIN386` byte
  transforms. Generic utilities and the mouse driver are consumed from
  `assets/release`, not duplicated in the tool directory.
- `tools/win31-path\APPLY.CMD` prompts once for an installed root and repairs
  only proven relocatable textual and Program Manager paths.  It does not
  install launch compatibility files.

`tools/win101-setup\APPLY.CMD` now follows the same one-prompt interaction,
while accurately identifying its target as installation media rather than an
installed Windows tree.

## Evidence and runtime boundary

The focused construction, identity, idempotence, malformed-input and
relocation evidence is recorded in
[M0 T437 S1 installed Windows 3.1 tools](../etc/evidence/m0-t437-s1-installed-win31-tools.md).
The final utility ownership consolidation is recorded in
[M0 T437 S8](../etc/evidence/m0-t437-s8-addon-utility-consolidation.md).

## S9--S11 corrective closures

S9 removed the remaining duplicate Win1.01 mouse driver from its tool package
and made the released add-on the single source. S10 corrected Win3.1 driver
selection to `SYSTEM\MOUSE.DRV`. S11 replaced the older `PATCH\*.ORIG`
recovery layout with adjacent `.BAK` files and added authenticated
`UNAPPLY.CMD`. Their evidence is [S9](../etc/evidence/m0-t437-s9-win101-release-driver-source.md),
[S10](../etc/evidence/m0-t437-s10-win31-system-mouse-path.md), and
[S11](../etc/evidence/m0-t437-s11-win31-adjacent-backups.md).

On a valid short-path installed fixture, both generated PIFs started through
plain `run16` in bounded fifteen-second probes.  Each created responsive
owned `run16`, `ntsrv`, `ntcon`, and `ntvdm` processes before scoped cleanup.
This closes the stated installed-profile startup criterion.

## Explicit limits

This closure does not claim normal original Setup completion, visible Program
Manager readiness, clean guest exit, or reliable enhanced-mode operation.
The original Setup protected-mode transition remains the separately queued
recovery package.  No host runtime component or release package changed in
this tooling-only task.

## S2 entrypoint cleanup

The owner found the obsolete Win1.01 `apply-setup.cmd` wrapper beside the
newer `APPLY.CMD`. S2 removed the wrapper after confirming that no active
automation requires it. The comparable Win3.1 launch and path tools already
had exactly one `APPLY.CMD`; build-only `win31-setup` has no repository-side
interactive batch launcher. The focused sweep is recorded in
[T437 S2 single tool entrypoint](../etc/evidence/m0-t437-s2-single-tool-entrypoint.md).

## S3 reusable utility foundation

S3 adds the AMD64 `PIF.EXE` and `HASH.EXE` tools under their own add-on source
homes. They contain only PIF mechanics and SHA-256 printing respectively; they
do not take over Win1.01 Setup policy. Their focused format, negative and
machine-identity evidence is [T437 S3 PIF and hash utilities](../etc/evidence/m0-t437-s3-pif-hash-utilities.md).

## S4 CMD-only Win1.01 Setup orchestration

S4 removes the media-PATCH PowerShell/templates and replaces them with the
inspectable `SETUP.CMD` flow. It stages only a private temporary copy of flat
original media, replaces only that copy's `MOUSE.DRV`, invokes original Setup
through `run16`, records the true result, and removes the temporary copy on
both normal and negative return. After an owner-confirmed successful Setup,
it creates the installed `PATCH` profile without repository/media dependencies.

`PIF.EXE` and `HASH.EXE` are published in `assets/release/` through the shared
`addon-manifest.json`, and are deployed to `O:\winnt\system32`.
The focused CMD, profile-generation, cleanup, negative-result, PIF-structure,
and release-identity evidence is [T437 S4](../etc/evidence/m0-t437-s4-win101-cmd-orchestration.md).

Original guest Setup completion remains a manual acceptance boundary. This
tooling delivery neither patches immutable source media nor claims that a
specific original Setup run completed.

## S5 reversible selected-media replacement

The owner revised the Win1.01 media rule: the selected user media is now the
intended recoverable delivery surface. `APPLY.CMD` moves root `MOUSE.DRV` to
`MOUSE.DRV.BAK` and puts the released replacement at the original root path.
It recognizes an already-applied replacement, keeps the backup unchanged on
repeat execution, and refuses conflicting backup/current state.

The media `PATCH` now has exactly `SETUP.CMD`, `PIF.EXE`, `HASH.EXE`, and
`SETVER.EXE`; it has neither a mouse-driver duplicate nor a persistent work
area. `SETUP.CMD` builds only transient launch settings, which it removes on
success or failure. The source, repeat, conflict, cleanup and profile tests
are recorded in [T437 S5 evidence](../etc/evidence/m0-t437-s5-win101-reversible-media-mouse.md).

## S6 prepared-media recovery

S6 adds `tools/win101-setup\UNAPPLY.CMD`. It refuses unprepared media,
conflicting current drivers, and unrecognised same-named helpers. On an exact
prepared state it restores `MOUSE.DRV.BAK` at the original media-root path,
consumes that backup, and removes only the four authenticated helper files.
Unknown `PATCH` content remains in place. The focused restoration and
preservation result is [T437 S6 evidence](../etc/evidence/m0-t437-s6-win101-media-unapply.md).
