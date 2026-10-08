# M0 T437 — installed Windows 3.1 portable launch closure

## Delivered scope

T437 delivers two deliberately separate, installed-tree tools:

- `tools/win31-launch\APPLY.CMD` prompts once for an installed Windows 3.1
  root and creates its self-contained `PATCH` compatibility payload.  It
  writes only `WINSTD.CMD/.PIF`, `WIN386.CMD/.PIF`, shared `CONFIG.NT` and
  `AUTOEXEC.NT`, the released mouse driver, and identity-bound recoverable
  `KRNL386`/`WIN386` adaptations.
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
