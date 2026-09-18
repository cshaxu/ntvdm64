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

## Deployed regression

The current `O:\winnt` package was exercised through its real ConPTY Console
path. `VerifyCvidIntegrated.ps1` with log prefix
`t420-s22-cvid-integrated-r2` passed all six video witnesses (`80x25`,
`45x34`, `60x50`, `120x30`, mouse and resize), plus five independent
short-window `EDIT -> MEM` sequences. The observer captured guest text for
each run; this is not an exit-code-only result.

`Verify-CommandExitStatus.ps1` with log prefix
`t420-s22-command-text-r1` then passed all 17 established Console-text-gated
routes: empty/native success and failure, stream/EOF handling, direct and
nested `COMMAND`, direct and nested `MEM`, the verified `G7.COM` guest exit
fixture, `COMMAND /C MEM`, and `EDIT`. Thus the deployed package has a fresh
direct and interactive `COMMAND/MEM/EDIT` regression witness while this
capability audit continues. These runs prove that the selected C-VID
publication boundary did not regress the existing Console product path; they
do not claim that every EGA memory writer was individually exercised, which
remains S23's responsibility.

## Current disposition

The 26 already-recorded SPC386 non-PIG/non-SPC486 null slots remain a
source-profile question, not missing provider candidates. S22 next needs to
map every remaining selected vector/timing consumer to real guest evidence or
a source-proven profile-null/no-caller disposition. This baseline does not
close S22.
