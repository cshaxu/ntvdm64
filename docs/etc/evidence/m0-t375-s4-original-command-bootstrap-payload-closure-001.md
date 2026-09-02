# M0 T375 S4 — Original COMMAND bootstrap payload closure

## Question

Can the fixed CPU40/SoftPC runtime complete an app-declared `EXIT` through
original `COMMAND.COM`, without adding a guest loader, a synthetic guest
command, or a trace-selected BOP provider?

## Recovered source contracts

1. Original `mvdm/dos/command/cmdmisc.c` supplies `VDMInfo.AppName` as a
   direct `CMDINFO.ExecPathSeg:ExecPathOff` alias.  The mirrored body had
   replaced this process alias with a bounded local capture but did not commit
   it back to guest RAM.  `cmdGetNextCmd` now uses the existing synchronous
   mapping-manager lease to write the post-PIF, upper-cased result before it
   stores `ExecPathSize`.  Command-tail and environment copies retain their
   existing original ordering.
2. Original BaseSrv keeps a normal DOS VDM waiting after it consumes one
   command.  A declared app CLI command has no continuing CSRSS console
   producer.  The copied BaseVDM record therefore owns a narrow app-only
   `terminal_on_command_exhaustion` flag: only after original COMMAND has
   consumed the record and requests the next command does the existing typed
   session escape complete the session.  Initial delivery, WOW records,
   ordinary no-command waits and `RETURN_ON_NO_COMMAND` remain unchanged.
3. An exploratory original `-i1` run was rejected as a product solution.  It
   selects the historical new-console DOS-session path but lacks its coupled
   BaseSrv context and exits with `0xC0000409`; it is not used by app or the
   accepted observation.

## Verification

The formal Win32/x86 graph was rebuilt outside the sandbox:

```text
build/M0-T371/S2/formal-x86:
  run-ninja-parallel.cmd original-softpc-process.exe
exit: 0
```

The existing x86 BaseVDM regression ran after the record change:

```text
build/M0-T302/S2/x86:
  ninja -j 8 test
PASS: local Base VDM broker contract
```

One final fixed, console-owning, non-debug observation then ran with the
immutable stage root `O:\ntvdm64` and input `--command EXIT`.  Product
SHA-256 was
`1b1fc86d15524bf269ff3a770e0f2ec85c85e33cf42ba21389dc33b105262c2f`; the
unchanged 17-media manifest SHA-256 was
`1ddbc5ef287194fdbf4c3074081a8bd019cf46d881c3823fb5fd24f0331d0fa9`.

The observer returned `result=exited`, `exit=0x00000000`, before its eight
second timeout.  The retained default-off report proves the expected original
sequence: first `54:01` delivery, `54:0E`, `54:04`, and the second `54:01`
request where the one-shot composition boundary completed the current session.

## Result

T375's claimed baseline is now proved: immutable original guest media reaches
original COMMAND and completes one declared built-in.  This does **not** prove
guest external `.COM`/MZ execution, PSP/parent return, native child execution,
or WOW/Win16; those are owned by later packages.
