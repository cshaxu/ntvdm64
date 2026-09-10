# M76 NT4 Guest Closure Evidence Audit

Status: current local artifact-evidence reconciliation, 2026-08-08.

## Scope

This audit answers only whether a fixed OpenNT NT4 guest output has a retained
local source-to-image record. It is not evidence of retail-byte identity,
bootability, a working VDM host, or permission to redistribute any artifact.

## Authority and Counting

The current authority is
`docs/HISTORICAL-TOOLCHAIN-REBASE-2026-08-07.md`, backed by the per-stage
manifests under `artifacts/toolchain-runs/`. It supersedes earlier exploratory
assembler/linker reports whenever they disagree.

The direct NT4 guest payload is exactly 33 files:

| Classification | Count | Current result |
| --- | ---: | --- |
| unchanged-source, historical-tool source build | 22 | Every stage records empty `GuestSourceTransforms`; OpenNT 4.5 MASM 4.00 and LINK16 are the formal assembler/linker inputs. `MEM.EXE` and `SETVER.EXE` additionally use the documented MS-DOS 4.0 C compiler/CRT fallback. |
| static source-snapshot companion/data | 7 | `GRAPHICS.PRO`, QBasic/EDIT help, and BASIC samples are data files, not claimed compiler outputs. |
| original `binplace` input, not source rebuildable from fixed NT4 tree | 4 | `BACKUP.EXE`, `QBASIC.EXE`, `MSHERC.COM`, `EDIT.COM`; their manifest explicitly records `SourceBuilt: false`. |

The 22 source-built direct artifacts include the execution-relevant
`NTIO.SYS`, `NTDOS.SYS`, `COMMAND.COM`, `HIMEM.SYS`, `DOSX.EXE`, and
`REDIR.EXE`, plus the declared utilities/devices enumerated in
`NT4-GUEST-ARTIFACT-CLOSURE.md`. For example, the retained NTDOS stage has a
zero-exit LINK16 manifest, post-link `STRIPZ` manifest, current output hash
`957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84`, and an
empty transform list.

Separately, five WOW16 guest-adjacent drivers (`MOUSE.DRV`, `VGA.DRV`,
`KEYBOARD.DRV`, `COMM.DRV`, `SOUND.DRV`) have independent original-source
MASM/LINK16/RC16 build records. They are not counted as the 33-file DOS
payload and do not constitute a rebuilt WOW host runtime.

## Non-Claims and Remaining Work

1. The four `binplace` artifacts are known missing source closures, not
   failed compilation tasks. Existing MS-DOS 6 probes are research evidence
   only and cannot be relabeled as NT4 provenance-matching builds.
2. The wider MVDM descriptor inventory has 75 declared outputs, most of which
   are host, WOW runtime, development, sample, or separately blocked product
   targets. The 33-file closure is not a claim to have rebuilt all 75.
3. Guest artifact availability does not unblock execution: M70's two governed
   backend capabilities and M75's profile-data/trace gates remain controlling.

## Documentation Disposition

`NT4-DOS-GUEST-BUILD-INVENTORY.md` and `OPENNT-GUEST-BUILD-INVENTORY.md` are
updated by this task to point at the completed closure rather than describe it
as future work. Older intermediate LINK diagnostics remain historical records,
but cannot override the retained stage manifests or the current rebase record.
