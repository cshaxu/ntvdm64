# M0 T327 — Minimal DOS guest execution and controlled return closure

T327 does not claim a completed DOS program, `COMMAND /C EXIT`, `54:01`,
`54:00`, `EXEC`, or parent return.

It closed the complete original COMMAND configuration package sufficiently to
remove it as the immediate startup blocker. The original 64-byte `cmdconf.c`
system-root contract rejected the previous 81-byte staged DOS root. The same
immutable media at a 37-byte root crossed `54:0C` and its original DEM
file-operation cohort. The next result is again the original CPU40/FDC
`wait_int` continuation's zero-address exception.

The task therefore uses its approved narrow-owner-transfer exit: the next
vertical-slice package begins with original SoftPC FDC/INT15/CPU40 waiting and
callback control flow. It may not treat the observed file BOPs as a patch
backlog or pretend the declared command executed.

Paired x86/x64 formal product links remain a required successor verification;
x64 link is never a guest-runtime claim.

Evidence: [S3 configuration-package and owner-transfer result](../etc/evidence/m0-t327-s3-command-configuration-owner-transfer-001.md).
