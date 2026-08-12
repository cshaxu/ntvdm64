# OpenWatcom Toolchain Admission Record

Status: downloaded for local research-tool audit; not installed or admitted to
any runtime, 2026-08-07.

## Purpose

Gate 0 requires a reproducible 16-bit-compatible build toolchain for the
managed OpenNT guest-artifact stages. This record concerns host-side
compatibility tools only. It does not add a dependency to `ntdos64-run`, copy
guest media into source, or authorize a system-wide install.

## Acquired Archive

| field | value |
| --- | --- |
| origin | `https://openwatcom.org/ftp/install/open-watcom-c-win32-1.9.exe` |
| local research location | `toolchain-probe/downloads/open-watcom-c-win32-1.9.exe` |
| byte count | 84,012,543 |
| official MD5 | `6316F454F732B0705EBFE2A278DC1E59` |
| observed MD5 | `6316F454F732B0705EBFE2A278DC1E59` |
| observed SHA-256 | `464C1B718BB560F9B3B3C7AED15419A9EFA241AF25D2C7990E1365A38F4498E8` |

The first interrupted download was rejected after its MD5 did not match the
official checksum. The resumed archive above matched before any attempt to
inspect or run it.

## Intended Audit Scope

If extracted to a local disposable directory, inspect only whether the
distribution contains compatible host utilities such as `wlink`, `exe2bin`,
and `wasm`, their versions, and their relevant command-line behavior. Keep the
tool path explicit in every future artifact-stage manifest.

No installer may alter `PATH`, registry, system directories, startup files,
services, or global configuration. The archive is not a project release input.
It must remain inside research staging and is removable with the stage that
uses it.

## Local Extraction Audit

The installer was not run. A portable archive reader extracted the SFX ZIP
payload directly to `toolchain-probe/openwatcom-1.9-local`. The local `binnt`
directory contains OpenWatcom 1.9 `wlink.exe`, `exe2bin.exe`, `wasm.exe`,
`wmake.exe`, and associated compiler support files.

`binnt/exe2bin.exe` successfully converted the v18 COMMAND MZ input. Its
output is byte-for-byte equal to the narrowed `opennt-reloc` compatibility
output; the exact hash is recorded in
`OPENNT-COMMAND-LAYOUT-RECOVERY.md`. This execution used explicit process-local
`WATCOM` and `PATH` values only. No installer, registry mutation, system path
change, service, or startup-file update occurred.

## WASM Compatibility Probe

`binnt/wasm.exe` is **not admitted** as the COMMAND assembler. A first probe
from the normal disposable stage could not create its default `.obj` output.
The same manifested input was then staged at the deliberately shallow,
disposable paths `D:\\home\\repos\\c19` and `D:\\c19`. At `D:\\c19`, with the
historical include roots supplied, WASM progressed into source diagnostics.
Its initial `comequ.asm` probe reported unsupported/legacy 16-bit syntax;
the successful include lookup removes the earlier `curdir.inc` lookup failure
as a source-compatibility signal.

The progressively shallower controls are consistent with a historical
output-path limitation, but the failing source means they do not independently
prove successful object creation. They do establish a real MASM-dialect
incompatibility before object output. This does not modify the source input or
justify source rewrites to make WASM accept COMMAND. Further use is limited to
recorded compatibility probes until the original assembler contract or an
equivalent, evidence-backed assembler configuration is established.

## Relation To COMMAND.COM

The OpenWatcom public tools manual documents the special `exe2bin` behavior
for a zero-relocation `CS:IP=0000:0100` MZ input. That documented rule is the
basis of the narrowly tested `opennt-reloc` compatibility path recorded in
`OPENNT-COMMAND-LAYOUT-RECOVERY.md`. A local OpenWatcom extraction may provide
an independent executable cross-check; it does not turn the source-built
helper into the historical OpenNT `reloc` binary.
