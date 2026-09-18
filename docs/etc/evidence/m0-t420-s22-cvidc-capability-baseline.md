# T420 S22 C-VID capability baseline

## Question

Does the selected x86 C-VID package publish one original-profile vector/state
surface, and can its original video accessors and known consumers be checked
against the current formal build rather than a stale historical build graph?

## Procedure and observations

`AuditCvidAccessorProfile.mjs` now requires an explicit current
`--build-graph`; it no longer silently reads the retired T412 graph. With
`build/M0-T420/S21/formal-x86-004/build.ninja`, the generated original
accessor fixture compiled and passed all 38 accessor pairs: generated offsets,
field values, isolated writes and scratch preservation.

The current formal C-VID binding fixture separately passed `38 pairs x 4
values, 81 slots, latches/selectors, republish, old-provider negative control,
concurrent scratch preservation`. The current product map attributes `Gdp` to
original `cvidc/ev_glue.obj`; metadata has 154 public and 55 private slots.
The CPU40 quick-event and direct effective-address providers are selected.
The restart getter/setter bind the selected original
`system/qevnt.c::qevJumpRestart` (initial value 100), rather than retaining a
second adapter-owned restart variable.  The binding fixture's set/read and
republish check passed against that owner.  The separate S24 system-capability
unit still owns end-to-end quick-event timing and lifecycle evidence.

`Audit-T420S22CvidcSlotDisposition.mjs` reads the generated binding used by
the current formal graph and the current `ntvdm.exe` map.  It accounted for
all 209 CPU-table slots: 180 resolve to linked original providers, three are
the finite generated-timing endpoints, and the remaining 26 are the frozen
SPC386 non-PIG/non-SPC486 profile-null set.  It fails on a missing provider,
a changed null set, an unrecognised adapter endpoint, or a restored duplicate
restart variable.

The production-library C-VID fixture now also invokes every valid original
table-publication family: all mode-0/mode-1 chain reads plus disabled/simple
reads, all four mark selections, and dither, unchained, chain-2, chain-4 and
simple write selections.  Every selected read/write/mark endpoint was
non-null after publication.  This proves C-VID's table-selection boundary; it
does not claim execution of those EGA memory functions, which remains S23's
real guest-video workload obligation.

## Selected consumer ownership

The selected-profile source census finds no unowned C-VID consumer.  C-VID is
the table/state publisher; a caller's operational workload remains with the
package that contains that caller.

| Consumer cohort | Selected reference files | Capability owner |
| --- | --- | --- |
| C-VID publication/forwarding | `accessfn.c`, `ev_glue.c` and their generated headers | S22: binding, `Gdp`, vector/table publication and profile-null disposition. |
| CCPU execution | `c_getset.c`, `ntstubs.c`, `cpu4gen.h`, `evidgen.h` | S21: CPU execution/descriptor/SAS provider behavior. |
| EGA/CGA/VGA writers | 11 source files from `cga.c` through `video.c` | S23: real guest video-memory read/write/display behavior. |
| PIC and quick events | `ica.c`, `qevnt.c`, `quick_ev.c` | S24: timer/PIC/reset and full quick-event lifecycle. |
| Keyboard integration | `keyba.c` | S25: keyboard-controller lifecycle. |
| Startup/termination/effective address | `main.c`, `terminat.c`, `xt.c` | S27: worker startup/termination and support-state behavior. |
| NT host bridges | `nt_aorc.c`, `nt_cpu.c`, `nt_msscs.c`, `nt_thred.c`, `nt_timer.c`, `stubs.c` | S32: real worker/host-provider lifecycle. |

Thus S22 must prove that every selected pointer/table is published with one
correct owner and can be consumed.  It must not relabel another package's
guest workload as a C-VID-only pass.  The table rows above are mandatory
incoming dependencies for those later capability units.

Nine historical short-vector consumer translation units were preprocessed with
the same current formal flags. None retained a direct `C_Video`/`Video` field
access after preprocessing. This is a consumer census, not proof that every
C-VID vector has guest execution coverage.

That assertion was re-run from the current formal graph in
`build/M0-T420/S22/accessor-consumer-audit-r1`, rather than inherited from the
earlier layout work. The generated original-accessor fixture passed all 38
pairs for field offset, value, isolated write and generated-scratch
preservation. The nine preprocessed consumers (`mouse_io`, `trace`,
`sascdef`, `nt_ega`, `nt_vga`, `nt_graph`, `nt_mouse`, `nt_fulsc` and
`nt_det`) each had zero evaluated direct `C_Video`/`Video` field access. They
therefore cannot select an incompatible short layout behind the C-VID vector
publication boundary. Their actual operational paths remain owned by the
respective package rows above.

## Deployed regression

The current formal x86 graph rebuilt `product-programs` and relinked
`ntvdm.exe` plus `VDMREDIR.dll`. The resulting `run16.exe`, `basesrv.exe`,
`ntvdm.exe`, `dtmgr.exe` and `VDMREDIR.dll` SHA-256 values each matched the
deployed `O:\winnt` package before runtime acceptance. That package was then
exercised through its real ConPTY Console path. `VerifyCvidIntegrated.ps1`
with log prefix `t420-s22-cvid-integrated-r3` passed all six video witnesses (`80x25`,
`45x34`, `60x50`, `120x30`, mouse and resize), plus five independent
short-window `EDIT -> MEM` sequences. The observer captured guest text for
each run; this is not an exit-code-only result.

`Verify-CommandExitStatus.ps1` with log prefix
`t420-s22-command-text-r2` then passed all 17 established Console-text-gated
routes: empty/native success and failure, stream/EOF handling, direct and
nested `COMMAND`, direct and nested `MEM`, the verified `G7.COM` guest exit
fixture, `COMMAND /C MEM`, and `EDIT`. Thus the deployed package has a fresh
direct and interactive `COMMAND/MEM/EDIT` regression witness while this
capability audit continues. These runs prove that the selected C-VID
publication boundary did not regress the existing Console product path; they
do not claim that every EGA memory writer was individually exercised, which
remains S23's responsibility.

## Residual mirror-diff disposition

The current normalized C-VID comparison has five changed mirror files. One
unneeded line was removed in this S22 pass: `ev_glue.c` no longer includes its
unused direct `<stdlib.h>`; its exact current formal `ev_glue.obj` command
compiled successfully after that removal, and the freshly relinked C-VID
publication fixture passed. The remaining differences are not independent
C-VID rewrites:

| File | Remaining reason |
| --- | --- |
| `ev_glue.c` | C-VID's single strong `Gdp` owner and the call at the original setup point to the missing historical generated vector binder; `<stdio.h>` is required by its original `printf` calls. |
| `evidfunc.h` | The fourteen mark declarations match the real generated `sevid001.c` ABI: `IUH` return plus four `IUH` arguments. The upstream K&R declarations conflict with those bodies under the selected compiler. |
| `evidgen.h` | CPU40 selects the original typed direct `vglob.c` accessors rather than the incompatible generated scratch-table macros; the full 81-slot C-VID layout remains intact. |
| `j_c_lang.c` | `<stdio.h>` declares its original diagnostic `printf` calls. |
| `j_c_lang.h` | Declares `CrulesRuntimeError`, which is called by the generated `sevid*.c` bodies and implemented by original `ev_glue.c`. |

This leaves no unclassified C-VID mirror difference in the selected profile.

## Current disposition

The 26 SPC386 non-PIG/non-SPC486 null slots are source-profile dispositions,
not missing provider candidates. The 180 original providers, three finite
timing endpoints, 38 typed accessors, 81 selected table slots and all
layout-sensitive consumers now have current formal or deployed witnesses and
one explicit owner. This closes S22's C-VID publication capability. Actual
guest EGA/CGA/VGA memory effects and their Console presentation remain S23's
separate selected-video workload; timer/PIC execution remains S24's separate
system workload. Neither is a missing C-VID publication implementation.
