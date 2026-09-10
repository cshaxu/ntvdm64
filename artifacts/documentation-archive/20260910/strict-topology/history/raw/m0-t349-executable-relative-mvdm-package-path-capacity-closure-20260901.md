# M0 T349 closure — Executable-relative MVDM package-path capacity

Date closed: 2026-09-01

T349 closed the bounded path-capacity seam without altering original COMMAND
configuration source.

- S1 traced original `cmdconf.c` short-root, PIF default selection, DEM and
  NTIO caller-buffer contracts against the current app/session binding.
- S2 added one app-only admission check for an executable-relative `mvdm`
  root that original COMMAND can represent.  It preserves PIF override and
  all original missing-file paths.
- S3 passed focused package-layout acceptance/rejection evidence and a fresh
  425-action formal CPU40/Win32/x86 link.

No guest content, imported MVDM algorithm, host installation state, mapping
manager, BOP family or runtime trace was changed.  The successor is the
CPU40/NTDOS post-`SVC_CMDSETINFO` guest-bootstrap continuity package.

Evidence:

- `docs/etc/operations/m0-t349-s1-mvdm-package-path-capacity-ledger.tsv`
- `docs/etc/evidence/m0-t349-s1-original-command-package-path-capacity-audit-001.md`
- `docs/etc/evidence/m0-t349-s2-command-package-path-admission-001.md`
- `docs/etc/evidence/m0-t349-s3-command-package-path-capacity-closure-001.md`
