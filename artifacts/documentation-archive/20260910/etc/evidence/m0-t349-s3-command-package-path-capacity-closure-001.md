# M0 T349 S3 — COMMAND package-path capacity closure

Date: 2026-09-01

## Verification

- The focused disposable Ninja Win32/x86 package-layout target compiled and
  passed both its nonexistent-root rejection and staged-root acceptance
  invocations.
- The selected formal CPU40 Win32/x86 graph was regenerated with the required
  Node 22 GDP generator and linked `original-softpc-process.exe` successfully:
  425 Ninja actions in `build/M0-T349/S3/record-x86`.
- Original-source warnings remained emitted by the formal graph.  No warning
  suppression, guest rewrite or unrelated warning change was introduced by
  this package.

## Closed contract

The executable-relative `mvdm` package is admitted only if its directory can
be represented by the unchanged original COMMAND 64-byte short-root contract
and its default system-media names fit the original caller capacities.  A
failure maps to the existing app media-rejected startup result before original
host startup.  Original PIF override, media existence checks, `cmdconf.c`
preprocessing and all guest behavior remain unchanged.

This is not evidence of guest bootstrap or COMMAND execution continuity.  The
next queue package is the complete CPU40/NTDOS post-`SVC_CMDSETINFO`
guest-bootstrap owner cohort.
