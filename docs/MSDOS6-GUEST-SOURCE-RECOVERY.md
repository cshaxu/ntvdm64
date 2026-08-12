# MS-DOS 6 Guest Source Recovery

## Imported Source Input

`src/ms-dos-6` is a byte-exact, source-only import from the local MS-DOS 6
tree. It contains the DOS 6 `BACKUP` source closure and the QB 4.5 source tree
needed to research `QBASIC.EXE`, `EDIT.COM`, and `MSHERC.COM`. The generated
`SOURCE-MANIFEST.json` records every imported file. Historical tools and
prebuilt/generated binary files are intentionally excluded from `src`.

The exact QB 4.5 historical tool closure is separately vendored at
`tools/msdos6-qb45`, with its own hash manifest.

## Reference Identity

The fixed MS-DOS 6 distribution copies of `QBASIC.EXE`, `EDIT.COM`, and
`MSHERC.COM` have the same SHA-256 values as the OpenNT NT4 `binplace` inputs.
This establishes that they are the correct reference family for source recovery.

## Build and Comparison Matrix

| Target | Historical build result | Comparison result | Honest status |
| --- | --- | --- | --- |
| `MSHERC.COM` | Assembles and links with the recovered source closure | 6,947-byte build differs from 6,934-byte reference | Blocked on missing `fmakeres.src` |
| `QBASIC.EXE` | Original `build.bat USA` starts in a clean DOSBox stage | No output is produced | Blocked on missing `runtime/tl/bldkit` |
| `EDIT.COM` | Same original QB 4.5 build path as QBASIC | No output is produced | Blocked on missing `runtime/tl/bldkit` |
| `BACKUP.EXE` | C and ASM objects compile; final historical link cannot start | No final executable exists | Blocked on matching libraries and message-generation inputs |

## MSHERC Result

The original `runtime/herc/makefile` source closure assembles and links with
both OpenNT tools16 and the MS-DOS 6 MASM/LINK510 pair. Both produce the same
6,947-byte output, which does not match the 6,934-byte distribution binary.

The first difference is the help text emitted by `fmakeres.asm`. The QB build
script expects to generate that file from `runtime/herc/fmakeres.src` and the
language-specific `herc.tok` input. `fmakeres.src` is absent from the local
MS-DOS 6 snapshot, while the checked-in `fmakeres.asm` contains a different
help string. No local source overlay is admitted to hide that missing input.

## QBASIC and EDIT Result

`qb5/qbas/build.bat USA` explicitly builds `QBASIC.EXE` and `EDIT.COM`. A clean
DOSBox stage can execute that batch script using the vendored historical tool
closure, but it cannot complete the runtime preparation because the script
calls `runtime/tl/bldkit`, which is absent from the local MS-DOS 6 snapshot.
No output binary is claimed from that probe.

An exact-name search across the local MS-DOS 4/MS-DOS 6, OpenNT, OpenNT 4.5,
Windows 2000, and Windows XP source trees found no `bldkit` input. This is a
source/tool recovery blocker, not a reason to replace the historical runtime
step with a local script.

## BACKUP Result

MS-DOS 6 provides `cmd/backup/backup.c`, `_parse.asm`, `_msgret.asm`, and a
makefile whose intended output is `BACKUP.EXE`. A DOSBox probe using only the
vendored MS-DOS 6 QB 4.5 tools successfully produced these independently
compilable objects:

| Object | Tool | SHA-256 | Bytes |
| --- | --- | --- | ---: |
| `backup.obj` | MS-DOS 6 `CL.EXE` | `47C6178A04CDDDA06D5CC028D270D14F78F3852DC9E0A2E01FB73A6A86FB52DB` | 28,933 |
| `_parse.obj` | MS-DOS 6 `MASM.EXE` | `13563256F9FB3BC29CDB0F4C654E597D04DB36B643752E917F6C4AF74E3D7346` | 4,142 |

The final link additionally requires `mapper.lib`, a matching `comsubs.lib`,
and country-message generation inputs. The local-tree search found none of
the former two for MS-DOS 6; it found only an MS-DOS 4 `COMSUBS.LIB`, which is
not used as a substitute. The same search found no `MAPPER.LIB` or
`FMAKERES.SRC`. No final NT4-compatible `BACKUP.EXE` is claimed until the
matching historical inputs are recovered.
