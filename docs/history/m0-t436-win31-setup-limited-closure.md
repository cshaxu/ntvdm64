# M0 T436 — Windows 3.1 limited delivery closure

## Owner disposition

The owner directed T436 to close and moved the remaining ordinary Windows 3.1
Setup protected-mode transition work to a new queue-tail candidate. This is a
limited closure, not a claim that normal original Setup or enhanced Windows
3.1 installation now succeeds.

## Delivered and retained facts

- S1 records the owner-accepted, unstable enhanced-mode research frontier and
  its approved recoverable `KRNL386`/`WIN386` candidate adaptations.
- S2 delivers the independent `MOUSE31.DRV`, its release identity, and the
  actual standard-mode mouse/exit proof.
- S3 delivers the Windows 3.1 PATCH package, explicit `winstd`/`win386`
  launch profiles, checked recovery behavior, and installation-local runtime
  files with no repository dependency after installation.
- S4 proves the necessary pre-copy sequencing: Setup copies compressed
  `KRNL386.EX_` and `WIN386.EX_` before the first Windows load, so an
  installed-PATCH-only operation is too late. The build-owned derived media
  copy changes only those two approved compressed representations, retains
  `SETUP.EXE` and `SETUP.INF`, and round-trips to the exact candidate hashes.

The detailed records are [S1](../etc/evidence/m0-t436-s1-win31-launch-profile.md),
[S2](../etc/evidence/m0-t436-s2-win31-mouse-driver.md),
[S3](../etc/evidence/m0-t436-s3-win31-setup-package.md), and
[S4 derived media](../etc/evidence/m0-t436-s4-derived-media-precopy.md).

## Closure verification

The closeout rebuilt `ntvdm.exe` with the retained, default-off S4 trace
hooks and passed the focused package tests: derived-media exact expansion,
identity-bound `KRNL386` adaptation, `SETUP.CMD` result propagation, and the
self-contained PATCHSET/recovery path.  The PATCHSET fixture intentionally
uses a short build-owned installation root because the historical PIF fields
have fixed path-length limits.  These checks prove package construction and
recovery only; they do not exercise Setup's unresolved post-copy transition.

## Explicit non-pass

The owner-hand normal path reaches the original post-copy message that Setup
is loading Windows and then stalls. The retired no-COM/no-LPT declaration did
not change that result. The direct-disk BOP dialog can continue through its
original Ignore result, but it is not the reported post-copy blocker.

Two late automated observer attempts exited before guest execution with `87`
and `68` because the observer invocation was invalid. They neither identify
the guest boundary nor change the hand-test result.

## Deferred package

[Windows 3.1 ordinary-Setup protected-mode transition recovery](../proposals/proposal-win31-ordinary-setup-protected-mode-transition-001.md)
is appended at the queue tail. It must first restore a valid reproducible
normal Setup witness, then identify the exact source-owned DOSX/first-Windows
handoff condition. It may not use `/I`, modify retail media or Setup files, or
present a candidate repair as an installation pass without a real normal Setup
run.
