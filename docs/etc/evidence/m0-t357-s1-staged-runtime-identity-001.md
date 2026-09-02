# M0 T357 S1 — staged runtime identity

## Result

S1 created a new, task-owned executable-relative runtime package at
`build/M0-T357/S1/runtime-stage` without changing product source or media.

The staged `original-softpc-process.exe` SHA-256 is
`c92e4cc21f872adc6abc766da94f583fdb751b416d1ed19ce73c457b8328fa65`.
It exactly matches the T356 S3 formal CPU40/x86 product and the stage
manifest's product identity.

The format-3 `runtime-manifest.json` records 16 immutable media entries.  The
stage contains sibling `mvdm/` media, including `NTIO.SYS`, `NTDOS.SYS`,
`COMMAND.COM`, configuration media, selected DOS utilities, and the selected
SoftPC ROM/CMOS/profile inputs.  Their recorded SHA-256 values were reviewed
against the stage manifest after staging.

## Scope result

This proves only package identity and the app-visible executable-relative
layout.  It is not an execution, guest-loader, COMMAND, or DOS result.  The
single frozen observation remains S2 work.

