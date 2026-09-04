# M0 T390 S3 — original PIF fallback and predecessor proof

## Result

The source-shaped PIF route now has both required outcomes. A valid selected
profile keeps the pure-DOS configuration; a missing selected PIF falls back
through the unchanged original OpenNT PIF/default selection and re-enters the
staged default `REDIR.EXE` / `DOSX.EXE` bootstrap.

## Procedure

The test temporarily renamed only
`O:\\ntvdm64\\profiles\\pure-dos\\pure-dos.pif`, ran the fixed non-debug
console-owning `command.com` declaration for 15 seconds, and restored the
exact file in a `finally` path. No product, configuration, guest-media,
keyboard, BOP or CPU input changed.

The missing-PIF report records `pif-bytes=42`: BaseVDM faithfully delivers the
declared PIF pathname even though the original PIF reader cannot open it. The
original fallback then opens `REDIR.EXE` and `DOSX.EXE`. This is the expected
default configuration result, not a substitute PIF parser.

With the restored profile, the formal CPU40/x86 product remains
`79855efa812d50d5f9cd70ad40b3009fb43c4123bfbdedd8e5f634a3e05db2b8`; the
same fixed container records `pif-bytes=42`, opens `HIMEM.SYS`, `COUNTRY.SYS`
and `COMMAND.COM`, and does not open `REDIR.EXE` or `DOSX.EXE`.

## Disposition

S3 is closed. The PIF carrier preserves original missing-file fallback and
the named profile removes the DPMI/WOW predecessor. Neither observation proves
interactive `COMMAND.COM` behavior.
