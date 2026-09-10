# NT4 DOS Guest Build Inventory

Status: historical source/build-input inventory, reconciled 2026-08-08.

## Scope

This inventory concerns the fixed OpenNT source input only. It neither copies
nor consumes any prebuilt guest binaries found in the upstream checkout. A
future research output must be rebuilt from tracked source and must retain its
own provenance manifest.

## Historical Build Topology

`base/mvdm/dos/v86/makefile` drives five guest build groups in order:

```text
doskrnl -> dev -> lib/xmm -> cmd -> redir
```

The common `make.inc` defines a distinct 16-bit toolchain: `masm`, `cl16`,
`link16`, `reloc`, `convert`, `compress`, `salut`, `nosrvbld`, `buildmsg`,
`buildidx`, and `mapsym`. The ordinary modern GCC/clang-cl host compilers are
not substitutes for this output format or its relocation/message pipeline.

## Source Availability

| Guest component | Historical source/build root | Source status | Build dependency class |
| --- | --- | --- | --- |
| NTIO.SYS | `base/mvdm/softpc.new/bios/` | Assembly sources and link script are present | MASM + LINK16 + RELOC + message-class generation |
| NTDOS.SYS | `base/mvdm/dos/v86/doskrnl/dos/` | Assembly sources, link script, and message skeleton are present | MASM + LINK16 + RELOC + message-class generation |
| COMMAND.COM | `base/mvdm/dos/v86/cmd/command/` | Assembly sources, link script, message skeleton, and `buildmsg.c` are present | MASM + LINK16 + CONVERT + message tools |
| HIMEM.SYS | `base/mvdm/dos/v86/dev/himem/` | Assembly sources and makefile are present | MASM + LINK16 + RELOC + STRIPDD |
| DOSX.EXE | `base/mvdm/dpmi/` | 486 assembly sources, link scripts, and definitions are present | MASM + LINK16 + MAPSYM |
| REDIR.EXE | `base/mvdm/dos/v86/redir/` | Assembly sources and makefile are present | MASM + LINK16 + message tools |
| KEYB/KEYBOARD.SYS | `base/mvdm/dos/v86/cmd/keyb/` and `dev/keyboard/` | Assembly sources, layouts, and link scripts are present | MASM + LINK16 + localization inputs |
| QBasic bundle / EDIT.COM | `base/mvdm/dos/v86/cmd/qbasic/` | The makefile only `binplace`s existing artifacts; no QBasic or EDIT source build is present | prebuilt upstream artifact only; not eligible for rebuild output |

Several source directories also retain historical `*.SYS`, `*.COM`, `*.EXE`,
`*.MAP`, and `*.SYM` files. They are evidence of names and build products,
not admissible inputs to the research runner or its distribution.

## Current Closure

The historical toolchain lane has since completed 22 unchanged-source direct
DOS builds and five separate WOW16 device-driver builds. Seven direct payload
files are static data companions; `BACKUP.EXE`, `QBASIC.EXE`, `MSHERC.COM`,
and `EDIT.COM` remain original `binplace` inputs with no source-build rule in
the fixed tree. The current authority is
`M76-NT4-GUEST-CLOSURE-EVIDENCE-AUDIT.md` and
`HISTORICAL-TOOLCHAIN-REBASE-2026-08-07.md`.

Guest-toolchain closure and a governed software execution backend remain
independent work: source-to-image evidence does not establish runtime support.
