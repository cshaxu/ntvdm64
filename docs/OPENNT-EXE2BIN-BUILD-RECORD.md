# OpenNT EXE2BIN Build Record

## Scope

This record covers the text-only source closure for the historical OpenNT
`base/mvdm/dos/v86/cmd/exe2bin` command. It does not claim a retail-equivalent
or guest-runnable executable.

The managed source island contains the eleven original command inputs:
`display.asm`, `e2bchng.inc`, `e2bequ.inc`, `e2binit.asm`, `e2bmacro.inc`,
`e2bparse.inc`, `e2btable.inc`, `exe2bin.lnk`, `exe2bin.skl`, `exe2bin.tag`,
and `makefile`. Each is an `upstream-copy` entry in
`src/opennt/SOURCE-MANIFEST.json`; the manifest has 2,399 entries and its
2026-08-07 full hash audit reported zero mismatches.

## Historical Build Shape

The original makefile requires `BUILDMSG` to turn `exe2bin.skl` and the EN-US
message inputs into `EXE2BIN.CTL`, `EXE2BIN.CLA`, `EXE2BIN.CLB`,
`EXE2BIN.CL1`, and `EXE2BIN.CL2`. Its original `exe2bin.lnk` links exactly:

1. `E2BINIT.obj`
2. `DISPLAY.obj`

The controlled `exe2bin-managed-input-audit-v1` stage verifies 201 managed
inputs before copying them into a disposable tree. The existing host BUILDMSG
compatibility tool generated all five message include files, and `display.asm`
assembled successfully.

## Resolved Toolchain Boundary

`e2binit.asm` was blocked before object creation by historical MASM macro
dialect behavior. Its included `dosmac.inc` defines an `invoke MACRO`, and
`e2bmacro.inc` defines macros with parameters named `addr` and `name`; those
were acceptable to the historical build but conflict with modern MASM-family
reserved words.

The following independent probes were made in the same managed stage, with no
upstream source edit or staging overlay:

| Assembler | Result |
| --- | --- |
| JWasm | 30 errors: `invoke`, `addr`, `name`, and subsequent macro nesting failures |
| UASM 2.53 | the same 30-error macro/reserved-word failure class |
| VS 2022 `ml.exe` with `/omf /Zm` | rejects `invoke MACRO` at `dosmac.inc:63`, then terminates on unmatched macro nesting |

The newly managed OpenNT-source-built MASM386 is that admissible route. In
fresh `exe2bin-stage-v5-pristine`, it assembled both `display.asm` and the
previously blocked `e2binit.asm` with zero warnings/errors, without an upstream
or stage-local source rewrite. OpenWatcom WLink then preserved the original
two-object order and produced `exe2bin.exe`:

| property | value |
| --- | --- |
| byte count | 12,480 |
| SHA-256 | `905721DD6117A57CE47C9FB240F30E4557347EB000D68D687AC95AD0C742F436` |
| image shape | DOS MZ (`4D 5A`), runtime validation pending |

This is a reproducible managed source-to-image build record. It does not make
the host-side OpenWatcom `exe2bin` cross-check interchangeable with this DOS
guest utility, and it does not establish guest execution or retail identity.

The v5 stage contains no guest-source transformation: it copies manifest
verified OpenNT sources, generates its EN-US message files through the
separately recorded host BUILDMSG, and uses the original `E2BINIT, DISPLAY`
link order. `OPENNT-G0-TOOLCHAIN-REBASE.md` records the shared fidelity rule.

## Next Admissible Work

1. Keep `EXE2BIN.EXE` out of runtime claims until it is guest-tested through
   the original historical runtime chain.
2. Preserve the MASM386 source closure and exact host-tool provenance for any
   future rebuild.
