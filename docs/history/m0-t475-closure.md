# M0 T475 — shared ownership and process-hook closure

T475's independent shared-ownership worktree completed its recorded local
packets and the owner closed S5 after the C process-hook replacement. Its
51-commit range was subsequently integrated into main under T474 S1; the
[integration record](../etc/evidence/m0-t474-s1-shared-range-integration.md)
maps the commits and records the mainline build and delivery. This historical
index does not close or change main's separately active T474 S2.

The worktree records are [S1 Win32 libraries](../etc/evidence/m0-t475-s1-win32-rpc.md),
[S2 dependency inventory](../etc/evidence/m0-t475-s2-component-dependency-inventory.md),
[S3 Worker/NTVDM remediation](../etc/evidence/m0-t475-s3-worker-ntvdm-remediation.md),
[S4 NTHook cleanup](../etc/evidence/m0-t475-s4-nthook-dependency-cleanup.md),
and [S5 process-hook replacement](../etc/evidence/m0-t475-s5-process-hook-import.md).
The separate NTVDM PE runtime-import boundary remains queued; it was not part
of the S3 source/static-link closure.
