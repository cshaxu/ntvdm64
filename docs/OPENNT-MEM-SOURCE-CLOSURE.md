# OpenNT MEM.EXE Source-Closure Record

## Scope

This record covers `base/mvdm/dos/v86/cmd/mem`, whose makefile declares
`mem.exe`. It establishes source, EN-US message, and application-object
closure, then records the C16 runtime ABI blocker. No `MEM.EXE` candidate is
claimed.

## Stage and Object Results

`toolchain-probe/mem-stage-v1` copies the tracked command, `inc`, `h`, `lib`,
and `messages` trees. The local source-built `buildmsg-host.exe` successfully
processed `mem.skl` against `messages/usa-ms.msg`, producing `MEM.CL1`,
`MEM.CL2`, `MEM.CLA`, and `MEM.CLB`.

Open Watcom C16 compiled the five original C modules in an explicit small
memory model: `mem`, `membase`, `memex`, `submsg`, and `memctrlc`. JWasm
assembled `_msgret`, `_parse`, and `chkvdisk`. `chkvdisk` emits two historical
segment-model warnings. `memctrlc` emits two warnings when passing its local
interrupt-frame registers to the old near-pointer `intdos` declaration.

The stage makes one narrowly scoped source adjustment in both `mem.c` and
`memctrlc.c`: the existing `OldCtrlc` interrupt-vector declaration receives
the `_CDECL` qualifier already required by the tracked `dos.h` declaration of
`_dos_getvect`/`_dos_setvect`. It changes only the compiler-visible pointer
type and preserves the legacy Windows-1252 encoding of `mem.c`.

## Link Blocker

The original `mem.lnk` selects `xmm` plus an old C16 runtime library. A link
with the tracked `slibce.lib` and an Open Watcom C16 runtime produces multiple
starting addresses: both libraries provide startup code. Removing the tracked
runtime avoids the duplicate entry point but leaves Microsoft-C runtime and
DOS service symbols unresolved because Open Watcom's C16 object/runtime ABI is
not symbol-compatible with it.

This is a runtime/toolchain boundary, not a missing OpenNT application object
or an invitation to implement the unresolved services as stubs. A successful
future build needs one coherent historical Microsoft C16 compiler, startup
object, and runtime-library set, or a separately verified ABI-preserving port.

## Qualification

Until that boundary is closed, no `MEM.EXE` output is created. A later runtime
test must exercise conventional, UMB, HMA, and XMS reporting paths plus the
Ctrl-C handler that temporarily changes UMB linkage.
