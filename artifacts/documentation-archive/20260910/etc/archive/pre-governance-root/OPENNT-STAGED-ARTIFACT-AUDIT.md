# OpenNT Staged Artifact Audit

## Purpose and Boundary

This is a filesystem audit of local `toolchain-probe` staging outputs on
2026-08-06. It is deliberately narrower than a historical build or runtime
claim: it records whether a presently stored file has a plausible DOS loading
shape. It does not execute guest code, copy an artifact into a release area,
or establish retail identity.

The audit closes a bookkeeping gap in the campaign records. A stage manifest
can still have a matching hash while the staged bytes are unsuitable for their
declared loader format. The later `COMMAND.COM` stage is the useful example:
its manifest hash is current, but its first non-zero byte is at offset `0x100`.
DOS would start executing the zero prefix, so that file is not a loadable
`.COM` candidate despite the historical object/link record.

## Structural Check

For an MZ executable, this audit requires the `MZ` signature and requires the
header `e_cp`/`e_cblp` length to equal the physical file length. For `.COM`
and headerless system images it records a non-zero entry byte at offset zero.
For DOS device images it records the expected linked-device leading shape
(`FFFF` next-driver pointer or a device-name header); this is a plausibility
check, not a driver-load test.

The selected, current files below were read directly. Per-stage hashes belong
in a later reproducible build record; no binary contents are reproduced here.

| Output | Selected stage | Bytes | Observed structure | Audit state |
| --- | --- | ---: | --- | --- |
| `NTIO.SYS` | `ntio-stage-v6` | 33,760 | starts `E9`; headerless bootstrap image | structural stage image |
| `NTDOS.SYS` | `ntdos-stage-v19` | 28,002 | starts `E9`; manifest byte count agrees | structural stage image |
| `HIMEM.SYS` | `himem-stage-v6-original` | 4,784 | starts `FFFF`; device-chain shape | pristine MASM386 source-to-image structural stage image |
| `DEBUG.EXE` | `debug-stage-v1` | 22,919 | MZ declared length = physical length | structural stage image |
| `EDLIN.EXE` | `edlin-stage-v1` | 14,281 | MZ declared length = physical length | structural stage image |
| `EXE2BIN.EXE` | `exe2bin-stage-v5-pristine` | 12,480 | MZ declared length = physical length | pristine MASM386 managed-input structural stage image |
| `APPEND.EXE` | `append-managed-input-audit-v1` | 12,034 | MZ declared length = physical length; two relocations; entry `0000:1B4B` | structural stage image |
| `SHARE.EXE` | `share-managed-input-audit-v1` | 295 | MZ declared length = physical length; zero relocations; entry `0000:0100` | structural stage image; intentional NT no-op success stub, runtime trace pending |
| `SETVER.EXE` | `setver-stage-v8` | 11,586 | MZ declared length = physical length | structural stage image |
| `FASTOPEN.EXE` | `fastopen-managed-input-audit-v1` | 295 | MZ declared length = physical length; zero relocations; entry `0000:0100` | structural stage image |
| `KB16.COM` | `kb16-stage-v1` | 14,710 | starts `E9` at offset zero | structural stage image |
| `ANSI.SYS` | `ansi-stage-v1` | 9,029 | starts `FFFF`; device-chain shape | structural stage image |
| `COUNTRY.SYS` | `country-stage-v4` | 27,097 | device-name header starts `FF COUNTRY` | structural stage image |
| `KEYBOARD.SYS` | `keyboard-stage-v5` | 40,665 | device-name header starts `FF KEYB` | structural stage image |
| `REDIR.EXE` | `redir-stage-v1` | 2,906 | MZ declared length = physical length | structural stage image; host contract blocked |

These rows are not runtime validation. In particular, they do not establish
that startup layout, relocation behaviour, BOP/DEM services, filesystems,
console handling, NLS, or device drivers work with the research runner.

## Rejected or Non-Runtime Stages

| Output/stage | Direct evidence | Required treatment |
| --- | --- | --- |
| `COMMAND.COM`, `command-stage-v17` and `-v18` | 50,708 bytes; first non-zero byte at `0x100`; manifest SHA-256 is `8461A5E632769305899C681549D266A2087D9230AD66F501107A46E969BEFEA6` | linker/layout evidence only, never passed to a DOS loader |
| `COMMAND.COM`, `command-stage-v20` and raw v21 | v20 changes staged guest text for JWasm; raw v21 proves original MASM object closure but WLink reports unsupported `rdata.obj` LIDATA relocations | no image candidate; require historical-LINK-compatible linker |
| `LOADFIX.COM`, `loadfix-stage-v1` | 1,387 bytes; first non-zero byte at `0x100` | superseded historical stage; current `loadfix-tools16-opennt-v1` is the source-built record |
| `QUIT.COM`, `quit-stage-v1` | exactly three bytes: `C4 C4 FE` | intentional monitor BOP probe, not a general DOS payload or startup-success evidence |
| `GRAPHICS.COM` | older WLink stage loses iterated-data relocations | superseded historical stage; current `graphics-tools16-opennt-v1` is the source-built record |
| `NLSFUNC.EXE` | older preprocessing stage was unavailable | superseded historical stage; current `nlsfunc-tools16-opennt-v1` is the source-built record |

## Campaign Gate After This Audit

The removed DOSX row described the older WLink-only stage. The current
`dosx-tools16-historical-v6` record supersedes it with an unchanged-source
OpenNT tools16 build of `DOSX.EXE`. This audit still makes no DPMI runtime
claim; see `HISTORICAL-TOOLCHAIN-REBASE-2026-08-07.md`.

The artifact matrix may use only these labels:

1. **source closure**: inputs and build recipe were located; no output is implied.
2. **object/link evidence**: relevant compilation or link work occurred; an output may still be malformed or absent.
3. **structural stage image**: the particular staged file passed the checks above. It stays in the research staging area.
4. **runtime validated**: a later bounded historical-chain test records the exact input hash and observed behaviour. There are no rows at this level.

Before any BYOB experiment consumes a staged image, rerun this audit from a
fresh stage and add its full hash, source closure, exact commands, and loader
result to the per-artifact record. A later stage number never supersedes an
earlier one merely by being newer.
