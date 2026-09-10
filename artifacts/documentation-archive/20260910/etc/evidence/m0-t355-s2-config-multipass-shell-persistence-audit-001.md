# M0 T355 S2 — CONFIG multi-pass `SHELL` persistence audit

Date: 2026-09-01

## Question

The one permitted short-root observation in S1 proved all of the following:

- the selected root `O:\ntvdm\mvdm` produces a 60-character `shell=` value,
  inside the original 63-visible-character `commnd` limit;
- original `cmdconf.c` wrote that value into the temporary configuration file;
- NTDOS nevertheless later opened its unchanged default `C:\COMMAND.COM`.

This audit reconstructs the original source cohort between that generated text
and the later `dfil` call.  It makes no product, guest, firmware, BOP, or
runtime change.

## Inputs and identity

The selected mirrors are source-identical to the OpenNT baseline for the
reached guest cohort:

```text
sysinit1.asm  0a0126881dfb6a487d8702bc90c248b57a4079ac1c13452861c5b4f4d5e4b37a
sysinit2.asm  3b05b35c6c555adb68301dab7df1e3cc886ca160068366326e3d1c88643a37e8
sysconf.asm   60bba5ffc49c7c995a33819ce6bc1d3111a4a693b7972d81bb3b146f600bcd7e
```

The staged `O:\ntvdm\mvdm\NTIO.SYS` also equals the selected guest binary:

```text
cfc8be16576bb6acd16bb8fc9b2d9a080f544bbfdd2d2d2df07ed908b3ab4937
```

`cmdconf.c` is a registered modified mirror.  Its only relevant divergence is
`MVDM-HOST-DIV-158`: it obtains the app-selected MVDM root instead of the NT4
Windows installation root, then retains the original short-path normalization
and the original `shell=` format.  The S1 temporary file proves that this
binding produced the intended text.

## Original control and data flow

1. `cmdGetConfigSys` calls `ExpandConfigFiles(TRUE)` and copies only the
   temporary filename into the original 64-byte guest `DS:DX` output buffer
   (`cmdconf.c:65-83`).  `ExpandConfigFiles` writes the generated `shell=`
   record using the original `%s=%s%s /p %s\\system32` form after acquiring a
   short system-root spelling (`cmdconf.c:198-228`).
2. `sysconf.asm::doconf` opens that temporary file, reads it into the transient
   `confbot` allocation, then enters `getcom`, which calls
   `sysinit2.asm::organize` (`sysconf.asm:528-616,651-653`).
3. `organize` uppercases and compacts the source input into the reparse buffer.
   Its original command table recognizes `shell`; the `S` indicator takes the
   explicit `org_file` branch, which preserves the filename and terminates it
   for later passes (`sysinit2.asm:328-400,416-440`).  `org_count` records the
   resulting buffer extent.
4. The initial pass is deliberately not the shell-consuming pass.  With
   `multi_pass_id == 0`, `tryi` goes to `multi_try_doshi`; therefore seeing no
   `commnd` change during this pre-scan is source-correct, not a failure
   (`sysconf.asm:696-715`).
5. `sysinit1.asm` increments `multi_pass_id` to one and calls `multi_pass`
   before subsequent device/IFS/install passes (`sysinit1.asm:1349-1352,
   1381-1400`).  `multi_pass` reuses `confbot` and `org_count`, resets its
   parser cursor, and returns to the ordinary command loop
   (`sysconf.asm:633-668`).
6. In that ordinary loop, the `S` entry reaches `trys`.  It clears the default
   parameter byte, copies the assembled source filename into `commnd`, writes
   a terminator, then separately copies shell parameters into `command_line`
   (`sysconf.asm:1779-1820`).  The only reached source writer that can replace
   `commnd` is therefore `trys`; the default remains its `sysinit2.asm`
   initializer only when the first normal multi-pass does not consume the
   organized `S` entry or when that write fails to persist.
7. `ConfigDone` occurs after all admitted multi-passes and before the later
   permanent-command `dfil` handoff.  It issues source-defined
   `BOP_NOTIFICATION` with `AL == 0` (`sysinit1.asm:1425-1431`).  The original
   host handler `MS_bop_E` accepts code zero, calls `UMBNotify(0)` and
   `demDasdInit()`, and otherwise has no `commnd` semantics
   (`nt_bop.c:722-742`).

## Interpretation

The source and staged-media facts rule out the S1-long-root explanation.
They do not select among the remaining source-owned alternatives:

- the normal `multi_pass_id == 1` call did not reach its parser loop;
- the reorganized buffer did not contain or expose the expected `S` entry;
- `trys` wrote a different value or its write was lost before `dfil`;
- `dfil` read an unexpected guest context despite a correct write.

No DEM result, path projection, or host `COMMAND.COM` workaround is justified
by this audit.  The required discriminator is guest state at an already
source-defined boundary, not another execution with modified inputs.

## Selected next disposition

The next bounded cohort is a default-off, read-only observation at original
`ConfigDone` / `BOP_NOTIFICATION(5Eh, AL=0)`.  It must copy—under a synchronous
checked guest-memory lease—the exact selected `NTIO.SYS` map locations for
`multi_pass_id` and the bounded 64-byte `commnd` storage, then release the
lease before returning to the unchanged `MS_bop_E` semantics.  It must neither
retain a guest pointer nor alter CPU, BOP, UMB, DEM, or DOS state.

This distinguishes a skipped multi-pass from a later persistence/context loss
in one source-positioned observation.  Any result that requires changing
`trys`, the parser, `dfil`, or machine execution is deferred to the S selected
by that evidence.
